/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "storagevolume.h"
#include "storagevolume_p.h"

#include "soliddefs_p.h"
#include <solid/device.h>
#include <solid/devices/ifaces/storagevolume.h>

Solid::StorageVolume::StorageVolume(QObject *backendObject)
    : DeviceInterface(*new StorageVolumePrivate(), backendObject)
{
}

Solid::StorageVolume::StorageVolume(StorageVolumePrivate &dd, QObject *backendObject)
    : DeviceInterface(dd, backendObject)
{
}

Solid::StorageVolume::~StorageVolume()
{
}

bool Solid::StorageVolume::isIgnored() const
{
    Q_D(const StorageVolume);
    auto *p = iface_cast<Ifaces::StorageVolume>(d->backendObject());
    return p ? p->isIgnored() : true;
}

Solid::StorageVolume::UsageType Solid::StorageVolume::usage() const
{
    Q_D(const StorageVolume);
    auto *p = iface_cast<Ifaces::StorageVolume>(d->backendObject());
    return p ? p->usage() : Unused;
}

QString Solid::StorageVolume::fsType() const
{
    Q_D(const StorageVolume);
    auto *p = iface_cast<Ifaces::StorageVolume>(d->backendObject());
    return p ? p->fsType() : QString();
}

QString Solid::StorageVolume::label() const
{
    Q_D(const StorageVolume);
    auto *p = iface_cast<Ifaces::StorageVolume>(d->backendObject());
    return p ? p->label() : QString();
}

QString Solid::StorageVolume::uuid() const
{
    Q_D(const StorageVolume);
    auto *p = iface_cast<Ifaces::StorageVolume>(d->backendObject());
    return p ? p->uuid().toLower() : QString();
}

qulonglong Solid::StorageVolume::size() const
{
    Q_D(const StorageVolume);
    auto *p = iface_cast<Ifaces::StorageVolume>(d->backendObject());
    return p ? p->size() : 0;
}

Solid::Device Solid::StorageVolume::encryptedContainer() const
{
    Q_D(const StorageVolume);

    auto *iface = iface_cast<Ifaces::StorageVolume>(d->backendObject());

    if (iface != nullptr) {
        return Device(iface->encryptedContainerUdi());
    } else {
        return Device();
    }
}
