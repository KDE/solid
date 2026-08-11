/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kiofusestorageaccess.h"
#include "kiofusedevice.h"
#include "kiofuseutils.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

using namespace Solid::Backends::KioFuse;

StorageAccess::StorageAccess(Device *device)
    : QObject(device)
    , m_device(device)
{
}

StorageAccess::~StorageAccess() = default;

bool StorageAccess::isAccessible() const
{
    return true;
}

QString StorageAccess::filePath() const
{
    return m_device->filePath();
}

bool StorageAccess::isIgnored() const
{
    return false;
}

bool StorageAccess::isEncrypted() const
{
    return false;
}

bool StorageAccess::setup()
{
    const QString udi = m_device->udi();
    QMetaObject::invokeMethod(
        this,
        [this, udi] {
            Q_EMIT setupDone(Solid::NoError, QVariant(), udi);
        },
        Qt::QueuedConnection);

    return true;
}

bool StorageAccess::teardown()
{
    const QString udi = m_device->udi();

    Q_EMIT teardownRequested(udi);

    QDBusMessage msg = QDBusMessage::createMethodCall(Utils::dbusService(), Utils::dbusPath(), Utils::dbusInterface(), QStringLiteral("unmountUrl"));
    msg.setArguments({m_device->url().toString()});

    auto *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(msg), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, udi, watcher] {
        watcher->deleteLater();

        const QDBusPendingReply<> reply = *watcher;
        if (reply.isError()) {
            const bool busy = reply.error().name() == QLatin1String("org.kde.KIOFuse.VFS.Error.MountBusy");
            Q_EMIT teardownDone(busy ? Solid::DeviceBusy : Solid::OperationFailed, reply.error().message(), udi);
            return;
        }

        Q_EMIT teardownDone(Solid::NoError, QVariant(), udi);
    });

    return true;
}

#include "moc_kiofusestorageaccess.cpp"
