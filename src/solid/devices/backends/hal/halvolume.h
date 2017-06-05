/*
    Copyright 2006 Kevin Ottens <ervin@kde.org>

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

    bool isIgnored() const Q_DECL_OVERRIDE;
    Solid::StorageVolume::UsageType usage() const Q_DECL_OVERRIDE;
    QString fsType() const Q_DECL_OVERRIDE;
    QString label() const Q_DECL_OVERRIDE;
    QString uuid() const Q_DECL_OVERRIDE;
    qulonglong size() const Q_DECL_OVERRIDE;
    QString encryptedContainerUdi() const Q_DECL_OVERRIDE;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_VOLUME_H
