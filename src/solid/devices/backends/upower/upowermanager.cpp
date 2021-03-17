/*
    SPDX-FileCopyrightText: 2010 Michael Zanetti <mzanetti@kde.org>
    SPDX-FileCopyrightText: 2010 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "upowermanager.h"
#include "upower.h"
#include "upowerdevice.h"

#include <QDBusConnectionInterface>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QDebug>

#include "../shared/rootdevice.h"

using namespace Solid::Backends::UPower;
using namespace Solid::Backends::Shared;

UPowerManager::UPowerManager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent)
    , m_manager(UP_DBUS_SERVICE, UP_DBUS_PATH, UP_DBUS_INTERFACE, QDBusConnection::systemBus())
{
    m_supportedInterfaces << Solid::DeviceInterface::GenericInterface << Solid::DeviceInterface::Battery;

    qDBusRegisterMetaType<QList<QDBusObjectPath>>();
    qDBusRegisterMetaType<QVariantMap>();

    bool serviceFound = m_manager.isValid();
    if (!serviceFound) {
        // find out whether it will be activated automatically
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.DBus", //
                                                              "/org/freedesktop/DBus",
                                                              "org.freedesktop.DBus",
                                                              "ListActivatableNames");

        QDBusReply<QStringList> reply = QDBusConnection::systemBus().call(message);
        if (reply.isValid() && reply.value().contains(UP_DBUS_SERVICE)) {
            QDBusConnection::systemBus().interface()->startService(UP_DBUS_SERVICE);
            serviceFound = true;
        }
    }

    if (serviceFound) {
        if (m_manager.metaObject()->indexOfSignal("DeviceAdded(QDBusObjectPath)") != -1) {
            // for UPower >= 0.99.0, changed signature :o/
            connect(&m_manager, SIGNAL(DeviceAdded(QDBusObjectPath)), this, SLOT(onDeviceAdded(QDBusObjectPath)));
            connect(&m_manager, SIGNAL(DeviceRemoved(QDBusObjectPath)), this, SLOT(onDeviceRemoved(QDBusObjectPath)));
        } else {
            connect(&m_manager, SIGNAL(DeviceAdded(QString)), this, SIGNAL(deviceAdded(QString)));
            connect(&m_manager, SIGNAL(DeviceRemoved(QString)), this, SIGNAL(deviceRemoved(QString)));
        }
    }
}

UPowerManager::~UPowerManager()
{
}

QObject *UPowerManager::createDevice(const QString &udi)
{
    if (udi == udiPrefix()) {
        RootDevice *root = new RootDevice(udiPrefix());

        root->setProduct(tr("Power Management"));
        root->setDescription(tr("Batteries and other sources of power"));
        root->setIcon("preferences-system-power-management");

        return root;

    } else if (allDevices().contains(udi)) {
        return new UPowerDevice(udi);

    } else {
        return nullptr;
    }
}

QStringList UPowerManager::devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type)
{
    const QStringList allDev = allDevices();
    QStringList result;

    if (!parentUdi.isEmpty()) {
        for (const QString &udi : allDev) {
            if (udi == udiPrefix()) {
                continue;
            }

            UPowerDevice device(udi);
            if (device.queryDeviceInterface(type) && device.parentUdi() == parentUdi) {
                result << udi;
            }
        }

        return result;
    } else if (type != Solid::DeviceInterface::Unknown) {
        for (const QString &udi : allDev) {
            if (udi == udiPrefix()) {
                continue;
            }

            UPowerDevice device(udi);
            if (device.queryDeviceInterface(type)) {
                result << udi;
            }
        }

        return result;
    } else {
        return allDev;
    }
}

QStringList UPowerManager::allDevices()
{
    QDBusReply<QList<QDBusObjectPath>> reply = m_manager.call("EnumerateDevices");

    if (!reply.isValid()) {
        qWarning() << Q_FUNC_INFO << " error: " << reply.error().name();
        return QStringList();
    }

    QStringList retList;
    retList << udiPrefix();

    const auto pathList = reply.value();
    for (const QDBusObjectPath &path : pathList) {
        retList << path.path();
    }

    return retList;
}

QSet<Solid::DeviceInterface::Type> UPowerManager::supportedInterfaces() const
{
    return m_supportedInterfaces;
}

QString UPowerManager::udiPrefix() const
{
    return UP_UDI_PREFIX;
}

void UPowerManager::onDeviceAdded(const QDBusObjectPath &path)
{
    Q_EMIT deviceAdded(path.path());
}

void UPowerManager::onDeviceRemoved(const QDBusObjectPath &path)
{
    Q_EMIT deviceRemoved(path.path());
}
