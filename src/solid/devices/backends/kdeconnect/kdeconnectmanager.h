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

#ifndef KDECONNECTMANAGER_H
#define KDECONNECTMANAGER_H

#include "solid/devices/ifaces/devicemanager.h"

#include <QtDBus/QDBusInterface>
#include <QtCore/QSet>

#include "kdeconnectbattery.h"

namespace Solid {
namespace Backends {
namespace KdeConnect {

class KdeConnectDevice;

class KdeConnectManager : public Solid::Ifaces::DeviceManager
{
    Q_OBJECT

public:
    KdeConnectManager(QObject *parent);
    virtual ~KdeConnectManager();

    virtual QObject *createDevice(const QString &udi);
    virtual QStringList devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type);
    virtual QStringList allDevices();
    virtual QSet<Solid::DeviceInterface::Type> supportedInterfaces() const;
    virtual QString udiPrefix() const;

private Q_SLOTS:
    void onDeviceAdded(const QString &id);
    void onDeviceRemoved(const QString &id);
    void onDeviceVisibilityChanged(const QString &id, bool visible);

    void onServiceRegistered(const QString &service);
    void onServiceUnregistered(const QString &service);

private:
    QString prefixedId(const QString &id) const;

    QSet<Solid::DeviceInterface::Type> m_supportedInterfaces;

    QStringList m_devices;
    //QHash<QString, KdeConnectDevice *> m_devices;
    QDBusInterface m_manager;
    bool m_ready;

};

} // namespace KdeConnect
} // namespace Backends
} // namespace Solid

#endif // KDECONNECTMANAGER_H
