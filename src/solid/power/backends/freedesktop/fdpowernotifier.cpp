/*
    Copyright 2014 Alejandro Fiestas Olivares <afiestas@kde.org>
    Copyright 2014 Lukáš Tinkl <ltinkl@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "fdpowernotifier.h"

#include <QDebug>
#include <QDBusConnection>

Solid::FDPowerNotifier::FDPowerNotifier(QObject* parent): PowerNotifier(parent)
{
    auto conn = QDBusConnection::systemBus();
    conn.connect(QStringLiteral("org.freedesktop.UPower"),
                 QStringLiteral("/org/freedesktop/UPower"),
                 QStringLiteral("org.freedesktop.DBus.Properties"),
                 QStringLiteral("PropertiesChanged"),
                 this,
                 SLOT(upowerPropertiesChanged(QString, QVariantMap, QStringList))
                 );

    conn.connect(QStringLiteral("org.freedesktop.login1"),
                 QStringLiteral("/org/freedesktop/login1"),
                 QStringLiteral("org.freedesktop.login1.Manager"),
                 QStringLiteral("PrepareForSleep"),
                 this,
                 SLOT(login1Resuming(bool))
                 );
}

void Solid::FDPowerNotifier::upowerPropertiesChanged(const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidated)
{
    if (interface != QStringLiteral("org.freedesktop.UPower")) {
        return;
    }

    if (changedProperties.contains(QStringLiteral("OnBattery"))) {
        Q_EMIT acPluggedChanged(!changedProperties.value(QStringLiteral("OnBattery")).toBool());
    }

    //Just for debug purposes
    if (!invalidated.isEmpty()) {
        qDebug() << "Invalidated" << invalidated;
    }
}

void Solid::FDPowerNotifier::login1Resuming(bool active)
{
    if (active) {
        Q_EMIT aboutToSuspend();
    } else {
        Q_EMIT resumeFromSuspend();
    }
}
