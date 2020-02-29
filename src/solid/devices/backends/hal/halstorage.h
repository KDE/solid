/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_STORAGE_H
#define SOLID_BACKENDS_HAL_STORAGE_H

#include <solid/devices/ifaces/storagedrive.h>
#include "halblock.h"

namespace Solid
{
namespace Backends
{
namespace Hal
{
class Storage : public Block, virtual public Solid::Ifaces::StorageDrive
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageDrive)

public:
    Storage(HalDevice *device);
    virtual ~Storage();

    Solid::StorageDrive::Bus bus() const override;
    Solid::StorageDrive::DriveType driveType() const override;

    bool isRemovable() const override;
    bool isHotpluggable() const override;
    qulonglong size() const override;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_STORAGE_H
