/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Compositor.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef WLSUBSURFACE_H
#define WLSUBSURFACE_H

#include "surfacerolehandler.h"
#include "wayland_wrapper/qwlsurface_p.h"

#include "qwayland-server-wayland.h"

#include <QtCore/QObject>
#include <QtCore/QLinkedList>

namespace GreenIsland {

class Compositor;
class Surface;

class SubSurface : public QObject, public QtWaylandServer::wl_subsurface, public SurfaceRoleHandler
{
public:
    SubSurface(WlSurface *surface, WlSurface *parent, wl_client *client, uint32_t id, int version);
    ~SubSurface();

    void parentCommit();

    static QString name();

protected:
    void configure(int dx, int dy) Q_DECL_OVERRIDE;

    void subsurface_destroy_resource(Resource *resource) Q_DECL_OVERRIDE;
    void subsurface_destroy(Resource *resource) Q_DECL_OVERRIDE;
    void subsurface_set_position(Resource *resource, int32_t x, int32_t y) Q_DECL_OVERRIDE;
    void subsurface_place_above(Resource *resource, ::wl_resource *sibling) Q_DECL_OVERRIDE;
    void subsurface_place_below(Resource *resource, ::wl_resource *sibling) Q_DECL_OVERRIDE;
    void subsurface_set_sync(Resource *resource) Q_DECL_OVERRIDE;
    void subsurface_set_desync(Resource *resource) Q_DECL_OVERRIDE;

private:
    void createSubView(SurfaceView *view);

    WlSurface *m_surface;
    WlSurface *m_parent;
    QPoint m_position;
    bool m_synchronized;
    QVector<SurfaceView *> m_views;
};

}


#endif // WLSUBSURFACE_H
