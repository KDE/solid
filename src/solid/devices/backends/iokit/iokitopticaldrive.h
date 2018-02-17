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

#ifndef SOLID_BACKENDS_IOKIT_OPTICALDRIVE_H
#define SOLID_BACKENDS_IOKIT_OPTICALDRIVE_H

#include <solid/devices/ifaces/opticaldrive.h>
#include "iokitstorage.h"

namespace Solid
{
namespace Backends
{
namespace IOKit
{
class IOKitOpticalDrive : public IOKitStorage, virtual public Solid::Ifaces::OpticalDrive
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::OpticalDrive)

public:
    explicit IOKitOpticalDrive(IOKitDevice *device);
    virtual ~IOKitOpticalDrive();

public Q_SLOTS:
    Solid::OpticalDrive::MediumTypes supportedMedia() const Q_DECL_OVERRIDE;
    int readSpeed() const Q_DECL_OVERRIDE;
    int writeSpeed() const Q_DECL_OVERRIDE;
    QList<int> writeSpeeds() const Q_DECL_OVERRIDE;
    bool eject() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void ejectPressed(const QString &udi) Q_DECL_OVERRIDE;
    void ejectDone(Solid::ErrorType error, QVariant errorData, const QString &udi) Q_DECL_OVERRIDE;
    void ejectRequested(const QString &udi);

private:
    class Private;
    Private *d;
};
}
}
}

#endif // SOLID_BACKENDS_IOKIT_OPTICALDRIVE_H
