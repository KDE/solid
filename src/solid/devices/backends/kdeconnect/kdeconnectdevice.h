/*
    Copyright 2013 Albert Vaca <albertvaka@gmail.com>
    Copyright 2015 Kai Uwe Broulik <kde@privat.broulik.de>

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

#ifndef KDECONNECTDEVICE_H
#define KDECONNECTDEVICE_H

#include <ifaces/device.h>
#include <solid/deviceinterface.h>
#include <battery.h>

#include <QtDBus/QDBusInterface>
#include <QtCore/QSet>

namespace Solid {
namespace Backends {
namespace KdeConnect {

class KdeConnectDevice : public Solid::Ifaces::Device
{
    Q_OBJECT

public:
    KdeConnectDevice(const QString &udi, const QString &id);
    virtual ~KdeConnectDevice();

    virtual QObject *createDeviceInterface(const Solid::DeviceInterface::Type &type);
    virtual bool queryDeviceInterface(const Solid::DeviceInterface::Type &type) const;
    virtual QString description() const;
    virtual QStringList emblems() const;
    virtual QString icon() const;
    virtual QString product() const;
    virtual QString vendor() const;
    virtual QString udi() const;
    virtual QString parentUdi() const;

    // ugly, perhaps use udi and slice the actual id out of it
    QString deviceId() const;

Q_SIGNALS:
    void changed();

private Q_SLOTS:
    void onChanged();

private:
    mutable QDBusInterface m_device;
    QString m_udi;
    QString m_id;

};

} // namespace KdeConnect
} // namespace Backends
} // namespace Solid

#endif // PHONEPOWERDEVICE_H
