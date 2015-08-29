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

#ifndef GREENISLAND_LOGIND_H
#define GREENISLAND_LOGIND_H

#include <QtCore/QObject>
#include <QtDBus/QDBusConnection>

#include <greenislandplatform/greenisland_platform_export.h>

namespace GreenIsland {

namespace Platform {

class LogindPrivate;

class GREENISLANDPLATFORM_EXPORT Logind : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool hasSessionControl READ hasSessionControl NOTIFY hasSessionControlChanged)
    Q_PROPERTY(bool sessionActive READ isSessionActive NOTIFY sessionActiveChanged)
    Q_PROPERTY(bool inhibited READ isInhibited NOTIFY inhibitedChanged)
    Q_PROPERTY(int vtNumber READ vtNumber NOTIFY vtNumberChanged)
    Q_ENUMS(InhibitMode)
    Q_DECLARE_PRIVATE(Logind)
public:
    enum InhibitFlag {
        InhibitShutdown = 0x01,
        InhibitSleep = 0x02,
        InhibitIdle = 0x04,
        InhibitPowerKey = 0x08,
        InhibitSuspendKey = 0x10,
        InhibitHibernateKey = 0x20,
        InhibitLidSwitch = 0x40
    };
    Q_DECLARE_FLAGS(InhibitFlags, InhibitFlag)

    enum InhibitMode {
        Block = 0,
        Delay
    };

    static Logind *instance();

    bool isConnected() const;
    bool hasSessionControl() const;
    bool isSessionActive() const;
    bool isInhibited() const;
    int vtNumber() const;

public Q_SLOTS:
    void inhibit(const QString &who, const QString &why, InhibitFlags flags, InhibitMode mode);
    void uninhibit(int fd);

    void lockSession();
    void unlockSession();

    void takeControl();
    void releaseControl();

    int takeDevice(const QString &fileName);
    void releaseDevice(int fd);

Q_SIGNALS:
    void connectedChanged(bool);
    void hasSessionControlChanged(bool);
    void sessionActiveChanged(bool);
    void inhibitedChanged(bool);
    void vtNumberChanged(int);

    void prepareForSleep(bool before);
    void prepareForShutdown(bool before);

    void lockSessionRequested();
    void unlockSessionRequested();

    void inhibited(int fd);
    void uninhibited(int fd);

protected:
    explicit Logind(const QDBusConnection &connection, QObject *parent = 0);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_serviceRegistered())
    Q_PRIVATE_SLOT(d_func(), void _q_serviceUnregistered())
    Q_PRIVATE_SLOT(d_func(), void _q_sessionPropertiesChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_devicePaused(uint devMajor, uint devMinor, const QString &type))
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Logind::InhibitFlags)

} // namespace Platform

} // namespace GreenIsland

#endif // GREENISLAND_LOGIND_H