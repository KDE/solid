/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "btrfsdevice.h"

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QUuid>

#include <solid/devices/ifaces/storagesubvolumetree.h>

#ifndef SOLID_BACKENDS_BTRFS_BTRFS_STORAGESUBVOLUMETREE_H
#define SOLID_BACKENDS_BTRFS_BTRFS_STORAGESUBVOLUMETREE_H

using namespace Qt::StringLiterals;

namespace Solid
{
namespace Backends
{
namespace Btrfs
{

class BtrfsStorageSubvolumeTree : public QObject, public Solid::Ifaces::StorageSubvolumeTree
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageSubvolumeTree)

public:
    explicit BtrfsStorageSubvolumeTree(BtrfsDevice *device);

    QString filePath() const override;

    qulonglong id() const override;

    QUuid uuid() const override;

    QUuid parentUuid() const override;

    QDateTime creationTime() const override;

private:
    BtrfsDevice *m_device;
};

}
}
}

#endif
