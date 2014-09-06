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

#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtCompositor/private/qwloutput_p.h>

#include "fullscreenshellclient.h"
#include "globalregistry.h"
#include "output.h"

FullScreenShellClient::FullScreenShellClient(quint32 id, quint32 version)
    : QtWayland::_wl_fullscreen_shell(GlobalRegistry::registry(), id, version)
    , m_id(id)
    , m_capabilities(0)
{
}

quint32 FullScreenShellClient::id() const
{
    return m_id;
}

FullScreenShellClient::Capabilities FullScreenShellClient::capabilities() const
{
    return m_capabilities;
}

void FullScreenShellClient::showOutput(Output *output)
{
    if (!output || !output->window()) {
        qWarning() << "Cannot show a null output window!";
        return;
    }

    QPlatformNativeInterface *native =
            QGuiApplication::platformNativeInterface();
    if (!native)
        qFatal("Platform native interface not found, aborting...");

    wl_surface *wlSurface = static_cast<wl_surface*>(
                native->nativeResourceForWindow("surface", output->window()));
    if (!wlSurface)
        qFatal("Unable to get wl_surface from output window, aborting...");

    wl_output *wlOutput = static_cast<wl_output *>(wl_resource_get_user_data(
                                                       output->handle()->resource()->handle));
    if (!wlOutput)
        qFatal("Unable to get wl_output from output, aborting...");

    ::_wl_fullscreen_shell_mode_feedback *feedback =
            present_surface_for_mode(wlSurface, wlOutput, output->refreshRate());
}

void FullScreenShellClient::hideOutput(Output *output)
{
    if (!output || !output->window()) {
        qWarning() << "Cannot hide a null output window!";
        return;
    }

    wl_output *wlOutput = static_cast<wl_output *>(wl_resource_get_user_data(
                                                       output->handle()->resource()->handle));
    if (!wlOutput)
        qFatal("Unable to get wl_output from output, aborting...");

    present_surface_for_mode(Q_NULLPTR, wlOutput, 0);
}

void FullScreenShellClient::fullscreen_shell_capability(uint32_t capability)
{
    switch (capability) {
    case QtWayland::_wl_fullscreen_shell::capability_arbitrary_modes:
        m_capabilities |= FullScreenShellClient::ArbitraryModes;
        break;
    case QtWayland::_wl_fullscreen_shell::capability_cursor_plane:
        m_capabilities |= FullScreenShellClient::CursorPlane;
        break;
    default:
        break;
    }
}
