/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

#ifndef KEYBINDINGSFILTER_H
#define KEYBINDINGSFILTER_H

#include <QtCore/QPointer>
#include <QtQuick/QQuickItem>

#include "quickkeybindings.h"

class KeyBindingsFilter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QuickKeyBindings *keyBindings READ keyBindings WRITE setKeyBindings NOTIFY keyBindingsChanged)
public:
    KeyBindingsFilter(QQuickItem *parent = Q_NULLPTR);

    QuickKeyBindings *keyBindings() const;
    void setKeyBindings(QuickKeyBindings *keyBindings);

Q_SIGNALS:
    void keyBindingsChanged();

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    QPointer<QQuickWindow> m_window;
    QPointer<QuickKeyBindings> m_keyBindings;
};

#endif // KEYBINDINGSFILTER_H
