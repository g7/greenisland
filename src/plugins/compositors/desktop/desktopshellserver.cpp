/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (c) 2012 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
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

#include <QtCompositor/wlsurface.h>
#include <QtCompositor/waylandsurface.h>

#include "desktopshellserver.h"

const struct desktop_shell_interface DesktopShellServer::shell_interface = {
    DesktopShellServer::set_background,
    DesktopShellServer::set_surface,
    DesktopShellServer::set_geometry
};

DesktopShellServer::DesktopShellServer(Wayland::Compositor *compositor)
    : m_compositor(compositor)
    , m_surface(0)
{
    wl_display_add_global(compositor->wl_display(),
                          &desktop_shell_interface, this,
                          DesktopShellServer::bind_func);
}

DesktopShellServer::~DesktopShellServer()
{
}

void DesktopShellServer::bind_func(wl_client *client, void *data, uint32_t version, uint32_t id)
{
    Q_UNUSED(version);

    wl_resource *resource = wl_client_add_object(client, &desktop_shell_interface, &shell_interface, id, data);
    resource->destroy = destroy_resource;

    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->m_resources.append(resource);
}

void DesktopShellServer::destroy_resource(wl_resource *resource)
{
    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->m_resources.removeOne(resource);
    free(resource);
}

void DesktopShellServer::set_background(struct wl_client *client,
                                        struct wl_resource *resource,
                                        const char *uri)
{
}

void DesktopShellServer::set_surface(struct wl_client *client,
                                     struct wl_resource *resource,
                                     struct wl_resource *surface)
{
    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->m_surface = Wayland::resolve<Wayland::Surface>(surface);
    if (!self->m_surface)
        return;
    self->m_surface->waylandSurface()->setWindowProperty("special", true);
}

void DesktopShellServer::set_geometry(struct wl_client *client,
                                      struct wl_resource *resource,
                                      uint32_t x, uint32_t y,
                                      uint32_t w, uint32_t h)
{
    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    Q_UNUSED(self);
}