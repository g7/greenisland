/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import GreenIsland 1.0

WaylandCompositor {
    property var primarySurfacesArea: null
    property variant windows: []

    id: compositor
    extensions: [
        Shell {
            id: defaultShell
            onCreateShellSurface: {
                var props = {
                    "windows": Qt.binding(function() { return compositor.windows }),
                    "surface": surface
                };
                var item = chromeComponent.createObject(screen.surfacesArea, props);
                item.shellSurface.initialize(defaultShell, surface, client, id);
                windows.push(item);
            }

            Component.onCompleted: {
                initialize();
            }
        }
    ]
    onCreateSurface: {
        var surface = surfaceComponent.createObject(compositor, {});
        surface.initialize(compositor, client, id, version);
    }

    GlobalPointerTracker {
        id: globalPointerTracker
        compositor: compositor
    }

    ScreenView {
        id: screen
        compositor: compositor
    }

    Component {
        id: surfaceComponent

        WaylandSurface {}
    }

    Component {
        id: chromeComponent

        WaylandWindow {}
    }

    Component.onCompleted: {
        compositor.primarySurfacesArea = screen.surfacesArea;
    }
}
