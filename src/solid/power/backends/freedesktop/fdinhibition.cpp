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

#include "fdinhibition.h"
#include "logindinhibitionargument.h"

#include <unistd.h>

#include <QVariant>
#include <QDebug>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusUnixFileDescriptor>

using namespace Solid;

FdInhibition::FdInhibition(Power::InhibitionTypes inhibitions, const QString &description, QObject* parent)
    : AbstractInhibition(parent)
    , m_state(Inhibition::Stopped)
    , m_description(description)
    , m_inhibitions(inhibitions)
{

}

FdInhibition::~FdInhibition()
{
    close(m_fd);
}

void FdInhibition::start()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
            QStringLiteral("org.freedesktop.login1"),
            QStringLiteral("/org/freedesktop/login1"),
            QStringLiteral("org.freedesktop.login1.Manager"),
            QStringLiteral("Inhibit"));

    QList<QVariant> args;
    args << LogindInhibitionArgument::fromPowerState(m_inhibitions)
         << qApp->applicationName()
         << m_description
         << QStringLiteral("block");
    msg.setArguments(args);

    QDBusConnection::systemBus().callWithCallback(msg, this, SLOT(slotDBusReply(QDBusMessage)), SLOT(slotDBusError(QDBusError)));
}

void FdInhibition::stop()
{
    close(m_fd);
    setState(Inhibition::Stopped);
}

void FdInhibition::setDescription(const QString& description)
{
    m_description = description;
}

void FdInhibition::slotDBusReply(const QDBusMessage& msg)
{
    m_fd = msg.arguments().first().value<QDBusUnixFileDescriptor>().fileDescriptor();
    setState(Inhibition::Started);
}

void FdInhibition::slotDBusError(const QDBusError& error)
{
    qDebug() << error.message();
}

void FdInhibition::setState(const Inhibition::State& state)
{
    if (m_state == state) {
        return;
    }

    m_state = state;
    Q_EMIT stateChanged(m_state);
}

Inhibition::State FdInhibition::state() const
{
    return m_state;
}

