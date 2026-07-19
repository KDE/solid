/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "btrfsstoragesubvolumetree.h"

using namespace Solid::Backends::Btrfs;

BtrfsStorageSubvolumeTree::BtrfsStorageSubvolumeTree(BtrfsDevice *device)
    : m_device(device)
{
}

QString BtrfsStorageSubvolumeTree::filePath() const
{
    return m_device->prop("Path"_L1).toString();
}

qulonglong BtrfsStorageSubvolumeTree::id() const
{
    return m_device->prop("Id"_L1).toULongLong();
}

QUuid BtrfsStorageSubvolumeTree::uuid() const
{
    return m_device->prop("Uuid"_L1).toUuid();
}

QUuid BtrfsStorageSubvolumeTree::parentUuid() const
{
    return m_device->prop("ParentUuid"_L1).toUuid();
}

QDateTime BtrfsStorageSubvolumeTree::creationTime() const
{
    return m_device->prop("CreationTime"_L1).toDateTime();
}
