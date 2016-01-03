/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 * Copyright (C) 2015 The Qt Company Ltd.
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL213$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/qtextstream.h>
#include <QtGui/QOpenGLContext>
#include <QtGui/qpa/qwindowsysteminterface.h>
#include <QtGui/qpa/qplatformintegration.h>
#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/private/qopenglcontext_p.h>

#include "logging.h"
#include "eglconvenience/eglconvenience.h"
#include "deviceintegration/egldeviceintegration.h"
#include "deviceintegration/eglfscursor.h"
#include "deviceintegration/eglfsintegration.h"
#include "deviceintegration/eglfswindow.h"
#include "platformcompositor/openglcompositorbackingstore.h"

namespace GreenIsland {

namespace Platform {

EglFSWindow::EglFSWindow(QWindow *w)
    : QPlatformWindow(w),
      m_backingStore(0),
      m_raster(false),
      m_winId(0),
      m_surface(0),
      m_window(0),
      m_flags(0)
{
}

EglFSWindow::~EglFSWindow()
{
    destroy();
}

static WId newWId()
{
    static WId id = 0;

    if (id == std::numeric_limits<WId>::max())
        qCWarning(lcDeviceIntegration, "EglFSWindow: Out of window IDs");

    return ++id;
}

void EglFSWindow::create()
{
    if (m_flags.testFlag(Created))
        return;

    m_winId = newWId();

    // Save the original surface type before changing to OpenGLSurface.
    m_raster = (window()->surfaceType() == QSurface::RasterSurface);
    if (m_raster) // change to OpenGL, but not for RasterGLSurface
        window()->setSurfaceType(QSurface::OpenGLSurface);

    if (window()->type() == Qt::Desktop) {
        QRect fullscreenRect(QPoint(), screen()->availableGeometry().size());
        QPlatformWindow::setGeometry(fullscreenRect);
        QWindowSystemInterface::handleGeometryChange(window(), fullscreenRect);
        return;
    }

    m_flags = Created;

    if (window()->type() == Qt::Desktop)
        return;

    // Stop if there is already a window backed by a native window and surface. Additional
    // raster windows will not have their own native window, surface and context. Instead,
    // they will be composited onto the root window's surface.
    EglFSScreen *screen = this->screen();
    OpenGLCompositor *compositor = OpenGLCompositor::instance();
    if (screen->primarySurface() != EGL_NO_SURFACE) {
        if (isRaster() && compositor->targetWindow()) {
            m_format = compositor->targetWindow()->format();
            return;
        }

#if !defined(Q_OS_ANDROID) || defined(Q_OS_ANDROID_NO_SDK)
        // We can have either a single OpenGL window or multiple raster windows.
        // Other combinations cannot work.
        qFatal("EGLFS: OpenGL windows cannot be mixed with others.");
#endif

        return;
    }

    m_flags |= HasNativeWindow;
    setGeometry(QRect()); // will become fullscreen
    QWindowSystemInterface::handleExposeEvent(window(), QRect(QPoint(0, 0), geometry().size()));

    EGLDisplay display = static_cast<EglFSScreen *>(screen)->display();
    QSurfaceFormat platformFormat = egl_device_integration()->surfaceFormatFor(window()->requestedFormat());
    m_config = EglFSIntegration::chooseConfig(display, platformFormat);
    m_format = EglUtils::glFormatFromConfig(display, m_config, platformFormat);

    resetSurface();

    screen->setPrimarySurface(m_surface);

    if (isRaster()) {
        QOpenGLContext *context = new QOpenGLContext(QGuiApplication::instance());
        context->setShareContext(qt_gl_global_share_context());
        context->setFormat(m_format);
        context->setScreen(window()->screen());
        if (!context->create())
            qFatal("EGLFS: Failed to create compositing context");
        compositor->setTarget(context, window());
    }
}

void EglFSWindow::destroy()
{
    EglFSScreen *screen = this->screen();
    if (m_flags.testFlag(HasNativeWindow)) {
        EglFSCursor *cursor = qobject_cast<EglFSCursor *>(screen->cursor());
        if (cursor)
            cursor->resetResources();

        if (screen->primarySurface() == m_surface)
            screen->setPrimarySurface(EGL_NO_SURFACE);

        invalidateSurface();
    }

    m_flags = 0;
    OpenGLCompositor::instance()->removeWindow(this);
}

// The virtual functions resetSurface and invalidateSurface may get overridden
// in derived classes, for example in the Android port, to perform the native
// window and surface creation differently.

void EglFSWindow::invalidateSurface()
{
    if (m_surface != EGL_NO_SURFACE) {
        EGLDisplay display = static_cast<EglFSScreen *>(screen())->display();
        eglDestroySurface(display, m_surface);
        m_surface = EGL_NO_SURFACE;
    }
    egl_device_integration()->destroyNativeWindow(m_window);
    m_window = 0;
}

void EglFSWindow::resetSurface()
{
    EglFSScreen *nativeScreen = static_cast<EglFSScreen *>(screen());
    EGLDisplay display = nativeScreen->display();
    m_window = egl_device_integration()->createNativeWindow(this, nativeScreen->geometry().size(), m_format);
    m_surface = eglCreateWindowSurface(display, m_config, m_window, NULL);
    if (m_surface == EGL_NO_SURFACE) {
        EGLint error = eglGetError();
        eglTerminate(display);
        qFatal("EGL Error : Could not create the egl surface: error = 0x%x\n", error);
    }
}

void EglFSWindow::setVisible(bool visible)
{
    OpenGLCompositor *compositor = OpenGLCompositor::instance();
    QList<OpenGLCompositorWindow *> windows = compositor->windows();
    QWindow *wnd = window();

    if (wnd->type() != Qt::Desktop) {
        if (visible) {
            compositor->addWindow(this);
        } else {
            compositor->removeWindow(this);
            windows = compositor->windows();
            if (windows.size())
                windows.last()->sourceWindow()->requestActivate();
        }
    }

    QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));

    if (visible)
        QWindowSystemInterface::flushWindowSystemEvents();
}

void EglFSWindow::setGeometry(const QRect &r)
{
    QRect rect;
    bool forceFullscreen = m_flags.testFlag(HasNativeWindow);
    if (forceFullscreen)
        rect = screen()->availableGeometry();
    else
        rect = r;

    QPlatformWindow::setGeometry(rect);

    // if we corrected the size, trigger a resize event
    if (rect != r)
        QWindowSystemInterface::handleGeometryChange(window(), rect, r);
}

QRect EglFSWindow::geometry() const
{
    // For yet-to-become-fullscreen windows report the geometry covering the entire
    // screen. This is particularly important for Quick where the root object may get
    // sized to some geometry queried before calling create().
    if (!m_flags.testFlag(Created) && screen()->primarySurface() == EGL_NO_SURFACE)
        return screen()->availableGeometry();

    return QPlatformWindow::geometry();
}

void EglFSWindow::requestActivateWindow()
{
    if (window()->type() != Qt::Desktop)
        OpenGLCompositor::instance()->moveToTop(this);

    QWindow *wnd = window();
    QWindowSystemInterface::handleWindowActivated(wnd);
    QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));
}

void EglFSWindow::raise()
{
    QWindow *wnd = window();
    if (wnd->type() != Qt::Desktop) {
        OpenGLCompositor::instance()->moveToTop(this);
        QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));
    }
}

void EglFSWindow::lower()
{
    OpenGLCompositor *compositor = OpenGLCompositor::instance();
    QList<OpenGLCompositorWindow *> windows = compositor->windows();
    if (window()->type() != Qt::Desktop && windows.count() > 1) {
        int idx = windows.indexOf(this);
        if (idx > 0) {
            compositor->changeWindowIndex(this, idx - 1);
            QWindowSystemInterface::handleExposeEvent(windows.last()->sourceWindow(),
                                                      QRect(QPoint(0, 0), windows.last()->sourceWindow()->geometry().size()));
        }
    }
}

EGLSurface EglFSWindow::surface() const
{
    return m_surface != EGL_NO_SURFACE ? m_surface : screen()->primarySurface();
}

QSurfaceFormat EglFSWindow::format() const
{
    return m_format;
}

EGLNativeWindowType EglFSWindow::eglWindow() const
{
    return m_window;
}

EglFSScreen *EglFSWindow::screen() const
{
    return static_cast<EglFSScreen *>(QPlatformWindow::screen());
}

bool EglFSWindow::isRaster() const
{
    return m_raster || window()->surfaceType() == QSurface::RasterGLSurface;
}

QWindow *EglFSWindow::sourceWindow() const
{
    return window();
}

const QPlatformTextureList *EglFSWindow::textures() const
{
    if (m_backingStore)
        return m_backingStore->textures();

    return 0;
}

void EglFSWindow::endCompositing()
{
    if (m_backingStore)
        m_backingStore->notifyComposited();
}

WId EglFSWindow::winId() const
{
    return m_winId;
}

void EglFSWindow::setOpacity(qreal)
{
    if (!isRaster())
        qCWarning(lcDeviceIntegration, "EglFSWindow: Cannot set opacity for non-raster windows");

    // Nothing to do here. The opacity is stored in the QWindow.
}

} // namespace Platform

} // namespace GreenIsland
