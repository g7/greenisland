/****************************************************************************
 * This file is part of Hawaii.
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

#ifndef XWAYLAND_H
#define XWAYLAND_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>
#include <GreenIsland/Compositor/QWaylandSurface>

Q_DECLARE_LOGGING_CATEGORY(XWAYLAND)
Q_DECLARE_LOGGING_CATEGORY(XWAYLAND_TRACE)

namespace GreenIsland {

class Compositor;
class XWaylandManager;
class XWaylandServer;

class XWayland : public QObject
{
    Q_OBJECT
public:
    XWayland(Compositor *compositor, QObject *parent = 0);
    ~XWayland();

    bool isInitialized() const;

public Q_SLOTS:
    void initialize();

private Q_SLOTS:
    void serverStarted();
    void surfaceCreated(QWaylandSurface *surface);

private:
    Compositor *m_compositor;
    XWaylandServer *m_server;
    XWaylandManager *m_manager;
};

}

#endif // XWAYLAND_H
