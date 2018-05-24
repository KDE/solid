/*
    Copyright 2010 Mario Bensi <mbensi@ipsquad.net>

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

#ifndef SOLID_BACKENDS_FSTAB_FSTAB_DEVICE_H
#define SOLID_BACKENDS_FSTAB_FSTAB_DEVICE_H

#include <solid/devices/ifaces/device.h>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include "fstabstorageaccess.h"

namespace Solid
{
namespace Backends
{
namespace Fstab
{

class FstabDevice : public Solid::Ifaces::Device
{
    Q_OBJECT

public:
    FstabDevice(QString uid);

    virtual ~FstabDevice();

    QString udi() const override;

    QString parentUdi() const override;

    QString vendor() const override;

    QString product() const override;

    QString icon() const override;

    QStringList emblems() const override;

    QString description() const override;

    bool queryDeviceInterface(const Solid::DeviceInterface::Type &type) const override;

    QObject *createDeviceInterface(const Solid::DeviceInterface::Type &type) override;

    QString device() const;

Q_SIGNALS:
    void mtabChanged(const QString &device);

private Q_SLOTS:
    void onMtabChanged(const QString &device);

private:
    QString m_uid;
    QString m_device;
    QString m_product;
    QString m_vendor;
    QString m_description;
    QString m_iconName;
    QPointer<FstabStorageAccess> m_storageAccess;
};

}
}
}
#endif // SOLID_BACKENDS_FSTAB_FSTAB_DEVICE_H
