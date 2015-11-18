/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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

#ifndef GREENISLAND_QUICKXDGPOPUPITEM_P_H
#define GREENISLAND_QUICKXDGPOPUPITEM_P_H

#include <GreenIsland/QtWaylandCompositor/private/qwaylandquickitem_p.h>

#include <GreenIsland/Server/XdgShell>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Green Island API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace GreenIsland {

namespace Server {

class GREENISLANDSERVER_EXPORT QuickXdgPopupItemPrivate : public QWaylandQuickItemPrivate
{
public:
    enum GrabberState {
        DefaultState,
        ResizeState,
        MoveState
    };

    QuickXdgPopupItemPrivate()
        : QWaylandQuickItemPrivate()
        , shellSurface(Q_NULLPTR)
    {
    }

    XdgPopup *shellSurface;
};

} // namespace Server

} // namespace GreenIsland

#endif // GREENISLAND_QUICKXDGPOPUPITEM_P_H