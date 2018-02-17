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

#ifndef SOLID_BACKENDS_IOKIT_IOKITSTORAGE_H
#define SOLID_BACKENDS_IOKIT_IOKITSTORAGE_H

#include "iokitblock.h"
#include "dadictionary_p.h"

#include <solid/devices/ifaces/storagedrive.h>

namespace Solid
{
namespace Backends
{
namespace IOKit
{
class IOKitStorage : public Block, virtual public Solid::Ifaces::StorageDrive
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageDrive)

public:
    explicit IOKitStorage(IOKitDevice *device);
    explicit IOKitStorage(const IOKitDevice *device);
    ~IOKitStorage();

    QString vendor() const;
    QString product() const;
    QString description() const;

public Q_SLOTS:
    Solid::StorageDrive::Bus bus() const Q_DECL_OVERRIDE;
    Solid::StorageDrive::DriveType driveType() const Q_DECL_OVERRIDE;

    bool isRemovable() const Q_DECL_OVERRIDE;
    bool isHotpluggable() const Q_DECL_OVERRIDE;
    qulonglong size() const Q_DECL_OVERRIDE;
private:
    DADictionary *daDict;
};
}
}
}

#endif // SOLID_BACKENDS_IOKIT_IOKITSTORAGE_H
