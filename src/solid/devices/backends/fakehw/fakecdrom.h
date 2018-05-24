/*
    Copyright 2006 Davide Bettio <davide.bettio@kdemail.net>

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

#ifndef SOLID_BACKENDS_FAKEHW_FAKECDROM_H
#define SOLID_BACKENDS_FAKEHW_FAKECDROM_H

#include <solid/devices/ifaces/opticaldrive.h>
#include "fakestorage.h"

namespace Solid
{
namespace Backends
{
namespace Fake
{
class FakeCdrom : public FakeStorage, virtual public Solid::Ifaces::OpticalDrive
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::OpticalDrive)

public:
    explicit FakeCdrom(FakeDevice *device);
    virtual ~FakeCdrom();

public Q_SLOTS:
    Solid::OpticalDrive::MediumTypes supportedMedia() const override;
    int readSpeed() const override;
    int writeSpeed() const override;
    QList<int> writeSpeeds() const override;
    bool eject() override;

Q_SIGNALS:
    void ejectPressed(const QString &udi) override;
    void ejectDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
};
}
}
}

#endif // SOLID_BACKENDS_FAKEHW_FAKECDROM_H
