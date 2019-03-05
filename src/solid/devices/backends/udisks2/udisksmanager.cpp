/*
    Copyright 2012 Lukáš Tinkl <ltinkl@redhat.com>

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

#include "udisksmanager.h"
#include "udisksdevicebackend.h"
#include "udisks_debug.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusMetaType>
#include <QDBusConnectionInterface>

#include "../shared/rootdevice.h"
#include "solid/genericinterface.h"

using namespace Solid::Backends::UDisks2;
using namespace Solid::Backends::Shared;

Manager::Manager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent),
      m_manager(UD2_DBUS_SERVICE,
                UD2_DBUS_PATH,
                QDBusConnection::systemBus())
{
    m_supportedInterfaces
            << Solid::DeviceInterface::GenericInterface
            << Solid::DeviceInterface::Block
            << Solid::DeviceInterface::StorageAccess
            << Solid::DeviceInterface::StorageDrive
            << Solid::DeviceInterface::OpticalDrive
            << Solid::DeviceInterface::OpticalDisc
            << Solid::DeviceInterface::StorageVolume;

    qDBusRegisterMetaType<QList<QDBusObjectPath> >();
    qDBusRegisterMetaType<QVariantMap>();
    qDBusRegisterMetaType<VariantMapMap>();
    qDBusRegisterMetaType<DBUSManagerStruct>();

    bool serviceFound = m_manager.isValid();
    if (!serviceFound) {
        // find out whether it will be activated automatically
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.DBus",
                               "/org/freedesktop/DBus",
                               "org.freedesktop.DBus",
                               "ListActivatableNames");

        QDBusReply<QStringList> reply = QDBusConnection::systemBus().call(message);
        if (reply.isValid() && reply.value().contains(UD2_DBUS_SERVICE)) {
            QDBusConnection::systemBus().interface()->startService(UD2_DBUS_SERVICE);
            serviceFound = true;
        }
    }

    if (serviceFound) {
        connect(&m_manager, SIGNAL(InterfacesAdded(QDBusObjectPath,VariantMapMap)),
                this, SLOT(slotInterfacesAdded(QDBusObjectPath,VariantMapMap)));
        connect(&m_manager, SIGNAL(InterfacesRemoved(QDBusObjectPath,QStringList)),
                this, SLOT(slotInterfacesRemoved(QDBusObjectPath,QStringList)));

        QDBusConnection::systemBus().connect(UD2_DBUS_SERVICE, QString(), DBUS_INTERFACE_PROPS, "PropertiesChanged", this,
                                             SLOT(slotPropertiesChanged(QDBusMessage)));
    }

    DeviceBackend::setManager(this);
}

Manager::~Manager()
{
    for (auto it = m_cache.constBegin(), end = m_cache.constEnd(); it != end; ++it) {
        DeviceBackend::destroyBackend(it.key());
    }
    m_cache.clear();
}

QObject *Manager::createDevice(const QString &udi)
{
    if (udi == udiPrefix()) {
        RootDevice *root = new RootDevice(udi);

        root->setProduct(tr("Storage"));
        root->setDescription(tr("Storage devices"));
        root->setIcon("server-database"); // Obviously wasn't meant for that, but maps nicely in oxygen icon set :-p

        return root;
    } else if (deviceCache().contains(udi)) {
        return new Device(udi);
    } else {
        return nullptr;
    }
}

QStringList Manager::devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type)
{
    QStringList result;

    const auto devices = deviceCache();
    if (!parentUdi.isEmpty()) {
        for (auto it = devices.keyBegin(), end = devices.keyEnd(); it != end; ++it) {
            Device device(*it);
            if (device.queryDeviceInterface(type) && device.parentUdi() == parentUdi) {
                result << *it;
            }
        }

        return result;
    } else if (type != Solid::DeviceInterface::Unknown) {
        for (auto it = devices.keyBegin(), end = devices.keyEnd(); it != end; ++it) {
            Device device(*it);
            if (device.queryDeviceInterface(type)) {
                result << *it;
            }
        }

        return result;
    }

    return devices.keys();
}

QStringList Manager::allDevices()
{
    m_cache.clear();

    QDBusPendingReply<DBUSManagerStruct> reply = m_manager.GetManagedObjects();
    reply.waitForFinished();
    if (!reply.isError()) {
        const auto items = reply.value();
        for (auto it = items.begin(), end = items.end(); it != end; ++it) {
            const QString udi = it.key().path();

            if (!udi.startsWith(UD2_DBUS_PATH_BLOCK_DEVICES) && !udi.startsWith(UD2_DBUS_PATH_DRIVES)) {
                continue;
            }

            PropertyMap deviceProps = it.value();

            // Check if it might be an optical disc so we detect when media arrives
            if (udi.startsWith(UD2_DBUS_PATH_BLOCK_DEVICES)) {
                // FIXME check if this is working, as we explicitly don't check /drives but /block_devices, but I don't have a CD drive :)
                const QString drivePath = deviceProps.value(UD2_DBUS_INTERFACE_BLOCK).value(QStringLiteral("Drive")).value<QDBusObjectPath>().path();
                const bool isDrive = deviceProps.contains(QLatin1String(UD2_DBUS_INTERFACE_DRIVE));
                const QStringList mediaCompatibility = deviceProps.value(UD2_DBUS_INTERFACE_DRIVE).value(QStringLiteral("MediaCompatibility")).toStringList();

                // this is basically device.mightBeOpticalDisc() but without first creating an entire Device object
                if (isDrive
                        && !drivePath.isEmpty() && drivePath != QLatin1String("/")
                        && !mediaCompatibility.filter(QStringLiteral("optical_")).isEmpty()) {

                    QDBusConnection::systemBus().connect(UD2_DBUS_SERVICE, udi, DBUS_INTERFACE_PROPS,
                                                         "PropertiesChanged", this,
                                                         SLOT(slotMediaChanged(QDBusMessage)));

                    // not interesting for us right now without optical media, skip
                    if (!deviceProps.value(UD2_DBUS_INTERFACE_DRIVE).value(QStringLiteral("Optical")).toBool()) {
                        continue;
                    }
                }
            }

            m_cache.insert(udi, deviceProps);
        }
    }

    return m_cache.keys();
}

QSet< Solid::DeviceInterface::Type > Manager::supportedInterfaces() const
{
    return m_supportedInterfaces;
}

QString Manager::udiPrefix() const
{
    return UD2_UDI_DISKS_PREFIX;
}

void Manager::slotInterfacesAdded(const QDBusObjectPath &object_path, const VariantMapMap &interfaces_and_properties)
{
    const QString udi = object_path.path();

    /* Ignore jobs */
    if (udi.startsWith(UD2_DBUS_PATH_JOBS)) {
        return;
    }

    qCDebug(UDISKS2) << udi << "has new interfaces:" << interfaces_and_properties.keys();

    auto deviceIt = m_cache.find(udi);

    // new device, we don't know it yet
    if (deviceIt == m_cache.end()) {
        qCDebug(UDISKS2) << "\tIt's a new device, emitting added";
        // FIXME filter out generic DBus interfaces
        m_cache.insert(udi, interfaces_and_properties);
        emit deviceAdded(udi);
        return;
    }

    qCDebug(UDISKS2) << "\tUpdating existing device";

    // TODO "merge" / "unite"?
    for (auto it = interfaces_and_properties.begin(), end = interfaces_and_properties.end(); it != end; ++it) {
        /* Don't store generic DBus interfaces */
        if (it.key().startsWith(UD2_DBUS_SERVICE)) {
            continue;
        }
        deviceIt->insert(it.key(), it.value());
    }

    updateBackend(udi);

    // re-emit in case of 2-stage devices like N9 or some Android phones
    if (interfaces_and_properties.contains(UD2_DBUS_INTERFACE_FILESYSTEM)) {
        emit deviceAdded(udi);
    }
}

void Manager::slotInterfacesRemoved(const QDBusObjectPath &object_path, const QStringList &interfaces)
{
    const QString udi = object_path.path();
    if (udi.isEmpty()) {
        return;
    }

    /* Ignore jobs */
    if (udi.startsWith(UD2_DBUS_PATH_JOBS)) {
        return;
    }

    auto it = m_cache.find(udi);
    if (it == m_cache.end()) {
        return;
    }

    qCDebug(UDISKS2) << udi << "lost interfaces:" << interfaces;

    for (const QString &iface : interfaces) {
        it->remove(iface);
    }

    if (it->isEmpty()) {
        qCDebug(UDISKS2) << "\tThere are no more interface, emitting device removal";
        emit deviceRemoved(udi);
        m_cache.remove(udi);
        DeviceBackend::destroyBackend(udi);
    } else {
        if (DeviceBackend *backend = DeviceBackend::backendForUDI(udi, false /*create*/)) { // can this even be null here?
            backend->invalidateProperties();
        }
        /*
         * Changes in the interface composition may change if a device
         * matches a Predicate. We have to do a remove-and-readd cycle
         * as there is no dedicated signal for Predicate reevaluation.
         */
        emit deviceRemoved(udi);
        emit deviceAdded(udi);
    }
}

void Manager::slotPropertiesChanged(const QDBusMessage &msg)
{
    const QString udi = msg.path();

    if (udi.isEmpty() || !udi.startsWith(UD2_UDI_DISKS_PREFIX) || udi.startsWith(UD2_DBUS_PATH_JOBS)) {
        return;
    }

    auto cachedIt = m_cache.find(udi);
    if (cachedIt == m_cache.end()) {
        return;
    }

    QMap<QString, int> changeMap;

    const QString iface = qdbus_cast<QString>(msg.arguments().at(0));
    const QVariantMap changed = qdbus_cast<QVariantMap>(msg.arguments().at(1));
    const QStringList invalidated = qdbus_cast<QStringList>(msg.arguments().at(2));

    for (const QString &prop : invalidated) {
        (*cachedIt)[iface].remove(prop);
        // TODO old code did this but isn't this a removal?
        changeMap.insert(prop, Solid::GenericInterface::PropertyModified);
    }

    for (auto it = changed.begin(), end = changed.end(); it != end; ++it) {
        (*cachedIt)[iface].insert(it.key(), it.value());
        changeMap.insert(it.key(), Solid::GenericInterface::PropertyModified);
    }

    if (!changeMap.isEmpty()) {
        if (DeviceBackend *backend = DeviceBackend::backendForUDI(udi, false /*create*/)) {
            // FIXME just update the backend cache instead of invalidating it
            backend->invalidateProperties();
            emit backend->propertyChanged(changeMap);
        }
    }
}

void Manager::slotMediaChanged(const QDBusMessage &msg)
{
    const QVariantMap properties = qdbus_cast<QVariantMap>(msg.arguments().at(1));

    if (!properties.contains("Size")) { // react only on Size changes
        return;
    }

    const QString udi = msg.path();
    updateBackend(udi);
    qulonglong size = properties.value("Size").toULongLong();
    qCDebug(UDISKS2) << "MEDIA CHANGED in" << udi << "; size is:" << size;

    if (!m_cache.contains(udi) && size > 0) { // we don't know the optdisc, got inserted
        // FIXME FIXME FIXME get the properties
        //m_cache.insert(udi, properties);
        emit deviceAdded(udi);
    }

    if (m_cache.contains(udi) && size == 0) {  // we know the optdisc, got removed
        emit deviceRemoved(udi);
        m_cache.remove(udi);
        DeviceBackend::destroyBackend(udi);
    }
}

QMap<QString, PropertyMap> Manager::deviceCache()
{
    if (m_cache.isEmpty()) {
        allDevices();
    }

    return m_cache;
}

QStringList Manager::interfaces(const QString &udi)
{
    return deviceCache().value(udi).keys();
}

QMap<QString, PropertyMap> Manager::allProperties()
{
    return deviceCache();

}

PropertyMap Manager::properties(const QString &udi)
{
    return deviceCache().value(udi);
}

void Manager::updateBackend(const QString &udi)
{
    DeviceBackend *backend = DeviceBackend::backendForUDI(udi, false /*create*/);
    if (!backend) {
        return;
    }

    backend->invalidateProperties();

    // also update the drive
    const QString drivePath = m_cache.value(udi).value(UD2_DBUS_INTERFACE_BLOCK).value(QStringLiteral("Drive")).value<QDBusObjectPath>().path();
    if (drivePath.isEmpty() || drivePath == QLatin1String("/")) {
        return;
    }

    DeviceBackend *driveBackend = DeviceBackend::backendForUDI(drivePath, false /*create*/);
    if (!driveBackend) {
        return;
    }

    qCDebug(UDISKS2) << "\tUpdating drive" << drivePath;

    driveBackend->invalidateProperties();
}
