/*
    SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "storagesubvolumetree.h"
#include "storagesubvolumetree_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/storagesubvolumetree.h>

Solid::StorageSubvolumeTree::StorageSubvolumeTree(QObject *backendObject)
    : DeviceInterface(*new StorageSubvolumeTreePrivate(), backendObject)
{
}

Solid::StorageSubvolumeTree::~StorageSubvolumeTree()
{
}

QString Solid::StorageSubvolumeTree::filePath() const
{
    Q_D(const StorageSubvolumeTree);
    return_SOLID_CALL(Ifaces::StorageSubvolumeTree *, d->backendObject(), QString(), filePath());
}

qulonglong Solid::StorageSubvolumeTree::id() const
{
    Q_D(const StorageSubvolumeTree);
    return_SOLID_CALL(Ifaces::StorageSubvolumeTree *, d->backendObject(), qulonglong(), id());
}

QUuid Solid::StorageSubvolumeTree::uuid() const
{
    Q_D(const StorageSubvolumeTree);
    return_SOLID_CALL(Ifaces::StorageSubvolumeTree *, d->backendObject(), QUuid(), uuid());
}

QUuid Solid::StorageSubvolumeTree::parentUuid() const
{
    Q_D(const StorageSubvolumeTree);
    return_SOLID_CALL(Ifaces::StorageSubvolumeTree *, d->backendObject(), QUuid(), parentUuid());
}

QDateTime Solid::StorageSubvolumeTree::creationTime() const
{
    Q_D(const StorageSubvolumeTree);
    return_SOLID_CALL(Ifaces::StorageSubvolumeTree *, d->backendObject(), QDateTime(), creationTime());
}
