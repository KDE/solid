/*
    Copyright 2014 Alejandro Fiestas Olivares <afiestas@kde.org>

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

#include "fdacpluggedjob.h"

#include <QDebug>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusInterface>

using namespace Solid;

FDAcPluggedJob::FDAcPluggedJob(QObject* parent)
    : AbstractAcPluggedJob(parent)
    , m_isPlugged(false)
{
}

void FDAcPluggedJob::doStart()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
            QStringLiteral("org.freedesktop.UPower"),
            QStringLiteral("/org/freedesktop/UPower"),
            QStringLiteral("org.freedesktop.DBus.Properties"),
            QStringLiteral("Get"));

    msg << QStringLiteral("org.freedesktop.UPower");
    msg << QStringLiteral("OnBattery");

    QDBusConnection::systemBus().callWithCallback(msg, this, SLOT(slotDBusReply(QDBusMessage)), SLOT(slotDBusError(QDBusError)));
}

void FDAcPluggedJob::slotDBusReply(const QDBusMessage & msg)
{
    Q_ASSERT(!msg.arguments().isEmpty());

    m_isPlugged = !msg.arguments().first().value<QDBusVariant>().variant().toBool();
    emitResult();
}

void FDAcPluggedJob::slotDBusError(const QDBusError& error)
{
    setError(error.type());
    setErrorText(error.message());
    emitResult();
}

bool FDAcPluggedJob::isPlugged() const
{
    return m_isPlugged;
}
