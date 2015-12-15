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

#ifndef GREENISLANDCLIENT_POINTER_P_H
#define GREENISLANDCLIENT_POINTER_P_H

#include <QtCore/private/qobject_p.h>

#include <GreenIsland/Client/Pointer>
#include <GreenIsland/client/private/qwayland-wayland.h>

#include <wayland-cursor.h>

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

namespace Client {

class GREENISLANDCLIENT_EXPORT PointerPrivate
        : public QObjectPrivate
        , public QtWayland::wl_pointer
{
    Q_DECLARE_PUBLIC(Pointer)
public:
    PointerPrivate();
    ~PointerPrivate();

    Seat *seat;
    wl_surface *cursorSurface;
    quint32 enterSerial;

    static PointerPrivate *get(Pointer *pointer) { return pointer->d_func(); }

protected:
    void pointer_enter(uint32_t serial, wl_surface *surface,
                       wl_fixed_t surface_x, wl_fixed_t surface_y) Q_DECL_OVERRIDE;
};

} // namespace Client

} // namespace GreenIsland

#endif // GREENISLANDCLIENT_POINTER_P_H