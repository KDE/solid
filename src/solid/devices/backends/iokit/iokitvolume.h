/*
    Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>

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

#ifndef SOLID_BACKENDS_IOKIT_VOLUME_H
#define SOLID_BACKENDS_IOKIT_VOLUME_H

#include <solid/devices/ifaces/storagevolume.h>
#include "iokitblock.h"
#include "dadictionary_p.h"

#include <DiskArbitration/DiskArbitration.h>

namespace Solid
{
namespace Backends
{
namespace IOKit
{
class IOKitVolume : public Block, virtual public Solid::Ifaces::StorageVolume
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageVolume)

public:
    IOKitVolume(IOKitDevice *device);
    IOKitVolume(const IOKitDevice *device);
    virtual ~IOKitVolume();

    bool isIgnored() const override;
    Solid::StorageVolume::UsageType usage() const override;
    QString fsType() const override;
    QString label() const override;
    QString uuid() const override;
    qulonglong size() const override;
    QString encryptedContainerUdi() const override;

    QString vendor() const;
    QString product() const;
    QString description() const;

    DADiskRef daRef() const;

private:
    DADictionary *daDict;
};
}
}
}

#endif // SOLID_BACKENDS_IOKIT_VOLUME_H
