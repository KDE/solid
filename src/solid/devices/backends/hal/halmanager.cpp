/*
    SPDX-FileCopyrightText: 2005, 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halmanager.h"
#include "haldevice.h"
#include "haldeviceinterface.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

using namespace Solid::Backends::Hal;

class Solid::Backends::Hal::HalManagerPrivate
{
public:
    HalManagerPrivate() : manager("org.freedesktop.Hal",
                                      "/org/freedesktop/Hal/Manager",
                                      "org.freedesktop.Hal.Manager",
                                      QDBusConnection::systemBus()),
        cacheSynced(false) { }

    QDBusInterface manager;
    QList<QString> devicesCache;
    bool cacheSynced;
    QSet<Solid::DeviceInterface::Type> supportedInterfaces;
};

HalManager::HalManager(QObject *parent)
    : DeviceManager(parent),  d(new HalManagerPrivate())
{
    d->manager.connection().connect("org.freedesktop.Hal",
                                    "/org/freedesktop/Hal/Manager",
                                    "org.freedesktop.Hal.Manager",
                                    "DeviceAdded",
                                    this, SLOT(slotDeviceAdded(QString)));

    d->manager.connection().connect("org.freedesktop.Hal",
                                    "/org/freedesktop/Hal/Manager",
                                    "org.freedesktop.Hal.Manager",
                                    "DeviceRemoved",
                                    this, SLOT(slotDeviceRemoved(QString)));

    d->supportedInterfaces << Solid::DeviceInterface::GenericInterface
                           << Solid::DeviceInterface::Processor
                           << Solid::DeviceInterface::Block
                           << Solid::DeviceInterface::StorageAccess
                           << Solid::DeviceInterface::StorageDrive
                           << Solid::DeviceInterface::OpticalDrive
                           << Solid::DeviceInterface::StorageVolume
                           << Solid::DeviceInterface::OpticalDisc
                           << Solid::DeviceInterface::Camera
                           << Solid::DeviceInterface::PortableMediaPlayer
                           << Solid::DeviceInterface::Battery;
}

HalManager::~HalManager()
{
    delete d;
}

QString HalManager::udiPrefix() const
{
    return "/org/freedesktop/Hal";
}

QSet<Solid::DeviceInterface::Type> HalManager::supportedInterfaces() const
{
    return d->supportedInterfaces;
}

QStringList HalManager::allDevices()
{
    if (d->cacheSynced) {
        return d->devicesCache;
    }

    QDBusReply<QStringList> reply = d->manager.call("GetAllDevices");

    if (!reply.isValid()) {
        qWarning() << Q_FUNC_INFO << " error: " << reply.error().name() << endl;
        return QStringList();
    }

    d->devicesCache = reply;
    d->cacheSynced = true;

    return reply;
}

bool HalManager::deviceExists(const QString &udi)
{
    if (d->devicesCache.contains(udi)) {
        return true;
    } else if (d->cacheSynced) {
        return false;
    }

    QDBusReply<bool> reply = d->manager.call("DeviceExists", udi);

    if (!reply.isValid()) {
        qWarning() << Q_FUNC_INFO << " error: " << reply.error().name() << endl;
        return false;
    }

    if (reply) {
        d->devicesCache.append(udi);
    }

    return reply;
}

QStringList HalManager::devicesFromQuery(const QString &parentUdi,
        Solid::DeviceInterface::Type type)
{
    if ((parentUdi.isEmpty()) && (type == Solid::DeviceInterface::Unknown)) {
        return allDevices();
    }

    QStringList result;

    const QStringList deviceList = allDevices();
    for (const QString &udi : deviceList) {
        HalDevice device(udi);

        if ((!parentUdi.isEmpty()) && (parentUdi != device.parentUdi())) {
            continue;
        }

        if ((type != Solid::DeviceInterface::Unknown) && (!device.queryDeviceInterface(type))) {
            continue;
        }

        result << udi;
    }

    return result;
}

QObject *HalManager::createDevice(const QString &udi)
{
    if (deviceExists(udi)) {
        return new HalDevice(udi);
    } else {
        return nullptr;
    }
}

void HalManager::slotDeviceAdded(const QString &udi)
{
    d->devicesCache.append(udi);
    Q_EMIT deviceAdded(udi);
}

void HalManager::slotDeviceRemoved(const QString &udi)
{
    d->devicesCache.removeAll(udi);
    Q_EMIT deviceRemoved(udi);
}

