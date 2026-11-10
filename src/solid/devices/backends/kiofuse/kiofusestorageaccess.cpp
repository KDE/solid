/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kiofusestorageaccess.h"
#include "kiofusedevice.h"

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
    QMetaObject::invokeMethod(
        this,
        [this, udi] {
            Q_EMIT teardownDone(Solid::OperationFailed, tr("Remote locations mounted by kio-fuse cannot be unmounted."), udi);
        },
        Qt::QueuedConnection);

    return false;
}

#include "moc_kiofusestorageaccess.cpp"
