/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWAYLANDSURFACEITEM_H
#define QWAYLANDSURFACEITEM_H

#include <GreenIsland/QtWaylandCompositor/qwaylandexport.h>

#include <QtQuick/QQuickItem>
#include <QtQuick/qsgtexture.h>

#include <QtQuick/qsgtextureprovider.h>

#include <GreenIsland/QtWaylandCompositor/qwaylandview.h>
#include <GreenIsland/QtWaylandCompositor/qwaylandquicksurface.h>

Q_DECLARE_METATYPE(QWaylandQuickSurface*)

QT_BEGIN_NAMESPACE

class QWaylandInputDevice;
class QWaylandQuickItemPrivate;

class Q_COMPOSITOR_EXPORT QWaylandQuickItem : public QQuickItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QWaylandQuickItem)
    Q_PROPERTY(QWaylandView *view READ view CONSTANT)
    Q_PROPERTY(QWaylandCompositor *compositor READ compositor)
    Q_PROPERTY(QWaylandSurface *surface READ surface WRITE setSurface NOTIFY surfaceChanged)
    Q_PROPERTY(bool paintEnabled READ paintEnabled WRITE setPaintEnabled)
    Q_PROPERTY(bool touchEventsEnabled READ touchEventsEnabled WRITE setTouchEventsEnabled NOTIFY touchEventsEnabledChanged)
    Q_PROPERTY(QWaylandSurface::Origin origin READ origin NOTIFY originChanged)
    Q_PROPERTY(bool inputEventsEnabled READ inputEventsEnabled WRITE setInputEventsEnabled NOTIFY inputEventsEnabledChanged)
    Q_PROPERTY(bool focusOnClick READ focusOnClick WRITE setFocusOnClick NOTIFY focusOnClickChanged)
    Q_PROPERTY(bool sizeFollowsSurface READ sizeFollowsSurface WRITE setSizeFollowsSurface NOTIFY sizeFollowsSurfaceChanged)

public:
    QWaylandQuickItem(QQuickItem *parent = 0);
    ~QWaylandQuickItem();

    QWaylandCompositor *compositor() const;
    QWaylandView *view() const;

    QWaylandSurface *surface() const;
    void setSurface(QWaylandSurface *surface);

    QWaylandSurface::Origin origin() const;

    bool isTextureProvider() const { return true; }
    QSGTextureProvider *textureProvider() const;

    bool paintEnabled() const;
    bool touchEventsEnabled() const;

    void setTouchEventsEnabled(bool enabled);

    bool inputEventsEnabled() const;
    void setInputEventsEnabled(bool enabled);

    bool focusOnClick() const;
    void setFocusOnClick(bool focus);

    bool inputRegionContains(const QPointF &localPosition);

    bool sizeFollowsSurface() const;
    void setSizeFollowsSurface(bool sizeFollowsSurface);

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QHoverEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void touchEvent(QTouchEvent *event) Q_DECL_OVERRIDE;
    void mouseUngrabEvent() Q_DECL_OVERRIDE;

    virtual void surfaceChangedEvent(QWaylandSurface *newSurface, QWaylandSurface *oldSurface);
public Q_SLOTS:
    virtual void takeFocus(QWaylandInputDevice *device = 0);
    void setPaintEnabled(bool paintEnabled);
    void raise();
    void lower();

private Q_SLOTS:
    void surfaceMappedChanged();
    void handleSurfaceChanged();
    void parentChanged(QWaylandSurface *newParent, QWaylandSurface *oldParent);
    void updateSize();
    void updateBuffer(bool hasBuffer);
    void updateWindow();
    void beforeSync();

Q_SIGNALS:
    void surfaceChanged();
    void touchEventsEnabledChanged();
    void originChanged();
    void surfaceDestroyed();
    void inputEventsEnabledChanged();
    void focusOnClickChanged();
    void mouseMove(const QPointF &windowPosition);
    void mouseRelease();
    void sizeFollowsSurfaceChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) Q_DECL_OVERRIDE;

    QWaylandQuickItem(QWaylandQuickItemPrivate &dd, QQuickItem *parent = 0);
};

QT_END_NAMESPACE

#endif
