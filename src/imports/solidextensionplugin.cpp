/*
 *   Copyright (C) 2013 Ivan Cukic <ivan.cukic(at)kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "solidextensionplugin.h"

#include <QQmlExtensionPlugin>

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

    qmlRegisterType<Devices> (uri, 1, 0, "Devices");
}

