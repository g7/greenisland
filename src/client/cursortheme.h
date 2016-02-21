/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#ifndef GREENISLANDCLIENT_CURSORTHEME_H
#define GREENISLANDCLIENT_CURSORTHEME_H

#include <QtCore/QLoggingCategory>
#include <QtGui/QCursor>

#include <GreenIsland/client/greenislandclient_export.h>

struct wl_cursor_image;

namespace GreenIsland {

namespace Client {

class Compositor;
class CursorThemePrivate;
class Seat;
class ShmPool;

class GREENISLANDCLIENT_EXPORT CursorTheme : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CursorTheme)
public:
    enum CursorShape {
        ArrowCursor = Qt::ArrowCursor,
        UpArrowCursor,
        CrossCursor,
        WaitCursor,
        IBeamCursor,
        SizeVerCursor,
        SizeHorCursor,
        SizeBDiagCursor,
        SizeFDiagCursor,
        SizeAllCursor,
        BlankCursor,
        SplitVCursor,
        SplitHCursor,
        PointingHandCursor,
        ForbiddenCursor,
        WhatsThisCursor,
        BusyCursor,
        OpenHandCursor,
        ClosedHandCursor,
        DragCopyCursor,
        DragMoveCursor,
        DragLinkCursor,
        ResizeNorthCursor = Qt::CustomCursor + 1,
        ResizeSouthCursor,
        ResizeEastCursor,
        ResizeWestCursor,
        ResizeNorthWestCursor,
        ResizeSouthEastCursor,
        ResizeNorthEastCursor,
        ResizeSouthWestCursor
    };
    Q_ENUM(CursorShape)

    CursorTheme(Compositor *compositor, ShmPool *pool, Seat *seat);

    wl_cursor_image *cursorImage(CursorShape shape);

    void changeCursor(CursorShape shape);
};

} // namespace Client

} // namespace GreenIsland

#endif // GREENISLANDCLIENT_CURSORTHEME_H
