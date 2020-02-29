/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_VOLUME_H
#define SOLID_BACKENDS_HAL_VOLUME_H

#include <solid/devices/ifaces/storagevolume.h>
#include "halblock.h"

namespace Solid
{
namespace Backends
{
namespace Hal
{
class Volume : public Block, virtual public Solid::Ifaces::StorageVolume
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageVolume)

public:
    Volume(HalDevice *device);
    virtual ~Volume();

    bool isIgnored() const override;
    Solid::StorageVolume::UsageType usage() const override;
    QString fsType() const override;
    QString label() const override;
    QString uuid() const override;
    qulonglong size() const override;
    QString encryptedContainerUdi() const override;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_VOLUME_H
