/*
    SPDX-FileCopyrightText: 2013 Ivan Cukic <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "solidextensionplugin.h"

#include <QtQml>

#include "devices.h"
#include "solid/deviceinterface.h"

SolidExtensionPlugin::SolidExtensionPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
}

void SolidExtensionPlugin::registerTypes(const char *uri)
{
    using namespace Solid;

    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.solid"));

    qmlRegisterType<Devices>(uri, 1, 0, "Devices");
}
