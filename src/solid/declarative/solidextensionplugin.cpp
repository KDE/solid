/*
    SPDX-FileCopyrightText: 2013 Ivan Cukic <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <QQmlEngine>
#include <QQmlExtensionPlugin>

#include "devices.h"
#include "solid/deviceinterface.h"

class SolidExtensionPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override
    {
        qmlRegisterType<Solid::Devices>(uri, 1, 0, "Devices");
    }
};

#include "solidextensionplugin.moc"
