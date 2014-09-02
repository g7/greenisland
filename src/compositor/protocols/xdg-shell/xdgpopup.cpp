/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCompositor/private/qwlinputdevice_p.h>
#include <QtCompositor/private/qwlpointer_p.h>
#include <QtCompositor/private/qwlsurface_p.h>

#include "xdgpopup.h"
#include "xdgpopupgrabber.h"

XdgPopup::XdgPopup(XdgShell *shell, QWaylandSurface *parent, QWaylandSurface *surface,
                   wl_client *client, uint32_t id, uint32_t serial)
    : QWaylandSurfaceInterface(surface)
    , QtWaylandServer::xdg_popup(client, id)
    , m_shell(shell)
    , m_parentSurface(parent)
    , m_surface(surface)
    , m_serial(serial)
    , m_grabber(Q_NULLPTR)
{
    // Set surface type
    setSurfaceType(QWaylandSurface::Popup);

    // Map popup
    connect(surface, &QWaylandSurface::mapped, [=]() {
        if (surface->handle()->mapped() &&
                m_grabber &&
                m_grabber->serial() == serial) {
            m_grabber->addPopup(this);
        } else {
            done();
            m_grabber->m_client = Q_NULLPTR;
        }
    });
}

XdgPopupGrabber *XdgPopup::grabber() const
{
    return m_grabber;
}

void XdgPopup::done()
{
    send_popup_done(m_serial);
}

bool XdgPopup::runOperation(QWaylandSurfaceOp *op)
{
    switch (op->type()) {
    case QWaylandSurfaceOp::Close:
        done();
        return true;
    default:
        break;
    }

    return false;
}

#include "moc_xdgpopup.cpp"
