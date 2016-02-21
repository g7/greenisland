/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWAYLANDQUICKEXTENSION_H
#define QWAYLANDQUICKEXTENSION_H

#include <GreenIsland/QtWaylandCompositor/QWaylandExtension>

QT_BEGIN_NAMESPACE

#define Q_COMPOSITOR_DECLARE_QUICK_DATA_CLASS(className) \
    class Q_COMPOSITOR_EXPORT className##QuickData : public className \
    { \
/* qmake ignore Q_OBJECT */ \
        Q_OBJECT \
        Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false) \
        Q_CLASSINFO("DefaultProperty", "data") \
    public: \
        QQmlListProperty<QObject> data() \
        { \
            return QQmlListProperty<QObject>(this, m_objects); \
        } \
    private: \
        QList<QObject *> m_objects; \
    };

#define Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(className) \
    class Q_COMPOSITOR_EXPORT className##QuickExtension : public className \
    { \
/* qmake ignore Q_OBJECT */ \
        Q_OBJECT \
        Q_PROPERTY(QQmlListProperty<QWaylandExtension> extensions READ extensions) \
        Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false) \
        Q_CLASSINFO("DefaultProperty", "data") \
    public: \
        QQmlListProperty<QObject> data() \
        { \
            return QQmlListProperty<QObject>(this, m_objects); \
        } \
        QQmlListProperty<QWaylandExtension> extensions() \
        { \
            return QQmlListProperty<QWaylandExtension>(this, this, \
                                                       &className##QuickExtension::append_extension, \
                                                       &className##QuickExtension::countFunction, \
                                                       &className##QuickExtension::atFunction, \
                                                       &className##QuickExtension::clearFunction); \
        } \
        static int countFunction(QQmlListProperty<QWaylandExtension> *list) \
        { \
            return static_cast<className##QuickExtension *>(list->data)->extension_vector.size(); \
        } \
        static QWaylandExtension *atFunction(QQmlListProperty<QWaylandExtension> *list, int index) \
        { \
            return static_cast<className##QuickExtension *>(list->data)->extension_vector.at(index); \
        } \
        static void append_extension(QQmlListProperty<QWaylandExtension> *list, QWaylandExtension *extension) \
        { \
            className##QuickExtension *quickExtObj = static_cast<className##QuickExtension *>(list->data); \
            extension->setExtensionContainer(quickExtObj); \
        } \
        static void clearFunction(QQmlListProperty<QWaylandExtension> *list) \
        { \
            static_cast<className##QuickExtension *>(list->data)->extension_vector.clear(); \
        } \
    private: \
        QList<QObject *> m_objects; \
    };

QT_END_NAMESPACE

#endif  /*QWAYLANDQUICKEXTENSION_H*/
