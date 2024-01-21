/*
    SPDX-FileCopyrightText: 2012 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "udisksmanager.h"
#include "dbus/manager.h"
#include "udisks_debug.h"
#include "udisksutils.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDomDocument>

#include <algorithm>

#include "../shared/rootdevice.h"
#include "solid/genericinterface.h"

using namespace Solid::Backends::UDisks2;
using namespace Solid::Backends::Shared;

Manager::Manager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent)
    , m_manager(org::freedesktop::DBus::ObjectManager(UD2_DBUS_SERVICE, UD2_DBUS_PATH, QDBusConnection::systemBus()))
{
    m_supportedInterfaces = {
        Solid::DeviceInterface::GenericInterface,
        Solid::DeviceInterface::Block,
        Solid::DeviceInterface::StorageAccess,
        Solid::DeviceInterface::StorageDrive,
        Solid::DeviceInterface::OpticalDrive,
        Solid::DeviceInterface::OpticalDisc,
        Solid::DeviceInterface::StorageVolume,
    };

    qDBusRegisterMetaType<QList<QDBusObjectPath>>();
    qDBusRegisterMetaType<QVariantMap>();
    qDBusRegisterMetaType<VariantMapMap>();
    qDBusRegisterMetaType<DBUSManagerStruct>();

    connect(&m_manager, &org::freedesktop::DBus::ObjectManager::InterfacesAdded, this, &Manager::slotInterfacesAdded);
    connect(&m_manager, &org::freedesktop::DBus::ObjectManager::InterfacesRemoved, this, &Manager::slotInterfacesRemoved);

    QDBusConnection::systemBus()
        .connect(UD2_DBUS_SERVICE, QString() /*any path*/, DBUS_INTERFACE_PROPS, "PropertiesChanged", this, SLOT(slotPropertiesChanged(QDBusMessage)));
}

Manager::~Manager()
{
    m_cache.clear();
}

bool Manager::hasInterface(const QString &udi, const QString &interface)
{
    return deviceCache().value(udi).contains(interface);
}

QMap<QString, PropertyMap> Manager::allProperties()
{
    return deviceCache();
}

PropertyMap Manager::deviceProperties(const QString &udi)
{
    return deviceCache().value(udi);
}

QVariant Manager::deviceProperty(const QString &udi, const QString &name, Manager::FetchMode fetchMode)
{
    const auto &props = m_cache.value(udi);

    // Loop through all interfaces looking for a property.
    for (auto it = props.begin(), end = props.end(); it != end; ++it) {
        const QString iface = it.key();
        const auto valueIt = it->constFind(name);
        if (valueIt != it->constEnd()) {
            if (!valueIt->isValid() && fetchMode == FetchIfNeeded) {
                QDBusMessage call = QDBusMessage::createMethodCall(UD2_DBUS_SERVICE, udi, DBUS_INTERFACE_PROPS, "Get");
                call.setArguments({iface, name});
                QDBusReply<QVariant> reply = QDBusConnection::systemBus().call(call);

                /* We don't check for error here and store the item in the cache anyway so next time we don't have to
                 * do the DBus call to find out it does not exist but just check whether
                 * prop(key).isValid() */
                const QVariant value = Utils::sanitizeValue(reply.value());
                m_cache[udi][iface][name] = value;
                return value;
            }

            return *valueIt;
        }
    }

    return QVariant();
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
        return new Device(this, udi);
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
            Device device(this, *it);
            if (device.queryDeviceInterface(type) && device.parentUdi() == parentUdi) {
                result << *it;
            }
        }

        return result;
    } else if (type != Solid::DeviceInterface::Unknown) {
        for (auto it = devices.keyBegin(), end = devices.keyEnd(); it != end; ++it) {
            Device device(this, *it);
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

    org::freedesktop::DBus::ObjectManager manager(UD2_DBUS_SERVICE, UD2_DBUS_PATH, QDBusConnection::systemBus());
    QDBusPendingReply<DBUSManagerStruct> reply = manager.GetManagedObjects();
    reply.waitForFinished();
    if (!reply.isError()) {
        const auto items = reply.value();
        for (auto it = items.begin(), end = items.end(); it != end; ++it) {
            const QString udi = it.key().path();

            if (!udi.startsWith(UD2_DBUS_PATH_BLOCK_DEVICES) && !udi.startsWith(UD2_DBUS_PATH_DRIVES)) {
                continue;
            }

            VariantMapMap mapMap = it.value();
            for (QVariantMap &map : mapMap) {
                map = Utils::sanitizeValue(map);
            }
            m_cache.insert(udi, mapMap);
        }
    }

    return m_cache.keys();
}

QSet<Solid::DeviceInterface::Type> Manager::supportedInterfaces() const
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

    auto cachedIt = m_cache.find(udi);
    bool isNewDevice = (cachedIt == m_cache.end());

    if (isNewDevice) {
        qCDebug(UDISKS2) << "\tIt's a new device, emitting added";
        cachedIt = m_cache.insert(udi, VariantMapMap{});
    }

    // We need to re-fetch all existing interfaces to ensure by the time we emit "add" for FileSystem
    // the rest is up to date (e.g. if Loop gets updated after we gained FileSystem) some propertes aren't updated yet.
    // We'll skip Block as every device we are interested in will be a Block device.
    QStringList oldInterfaces = cachedIt->keys();
    oldInterfaces.removeOne(UD2_DBUS_INTERFACE_BLOCK);

    // Filters generic DBus interfaces.
    for (auto it = interfaces_and_properties.begin(), end = interfaces_and_properties.end(); it != end; ++it) {
        if (!it.key().startsWith(UD2_DBUS_SERVICE)) {
            continue;
        }
        cachedIt->insert(it.key(), Utils::sanitizeValue(it.value()));
    }

    for (const QString &interface : oldInterfaces) {
        QDBusMessage call = QDBusMessage::createMethodCall(UD2_DBUS_SERVICE, udi, DBUS_INTERFACE_PROPS, "GetAll");
        call.setArguments({interface});
        QDBusReply<QVariantMap> reply = QDBusConnection::systemBus().call(call);
        if (reply.isValid()) {
            cachedIt->insert(interface, Utils::sanitizeValue(reply.value()));
        }
    }

    // re-emit in case of 2-stage devices like N9 or some Android phones
    if (isNewDevice || interfaces_and_properties.contains(UD2_DBUS_INTERFACE_FILESYSTEM)) {
        Q_EMIT deviceAdded(udi);
    }

    // TODO invalidate drive? updateBackend did that
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

    auto cachedIt = m_cache.find(udi);
    if (cachedIt == m_cache.end()) {
        return;
    }

    qCDebug(UDISKS2) << udi << "lost interfaces:" << interfaces;

    for (const QString &iface : interfaces) {
        cachedIt->remove(iface);
    }

    /*
     * Determine left interfaces. The device backend may have processed the
     * InterfacesRemoved signal already, but the result set is the same
     * independent if the backend or the manager processes the signal first.
     */
    if (cachedIt->isEmpty()) {
        qCDebug(UDISKS2) << "\tThere are no more interface, emitting device removal";
        Q_EMIT deviceRemoved(udi);
        m_cache.remove(udi);
    } else {
        /*
         * Changes in the interface composition may change if a device
         * matches a Predicate. We have to do a remove-and-readd cycle
         * as there is no dedicated signal for Predicate reevaluation.
         */
        Q_EMIT deviceRemoved(udi);
        Q_EMIT deviceAdded(udi);
    }
}

void Manager::slotPropertiesChanged(const QDBusMessage &msg)
{
    const QString udi = msg.path();

    if (udi.isEmpty() || !udi.startsWith(UD2_UDI_DISKS_PREFIX) || udi.startsWith(UD2_DBUS_PATH_JOBS)) {
        return;
    }

    const auto args = msg.arguments();
    if (Q_UNLIKELY(args.size() != 3)) {
        return;
    }

    const QString iface = qdbus_cast<QString>(args.at(0));
    const QVariantMap changed = qdbus_cast<QVariantMap>(args.at(1));
    const QStringList invalidated = qdbus_cast<QStringList>(args.at(2));

    auto cachedIt = m_cache.find(udi);
    const bool knownDevice = (cachedIt != m_cache.end());

    if (knownDevice) {
        QMap<QString, int> changeMap;

        for (const QString &prop : invalidated) {
            // Invalid QVariant() marks property that exists but needs to be fetched first.
            (*cachedIt)[iface].insert(prop, QVariant());
            changeMap.insert(prop, Solid::GenericInterface::PropertyModified);
        }

        for (auto it = changed.begin(), end = changed.end(); it != end; ++it) {
            (*cachedIt)[iface].insert(it.key(), Utils::sanitizeValue(it.value()));
            changeMap.insert(it.key(), Solid::GenericInterface::PropertyModified);
        }

        if (!changeMap.isEmpty()) {
            Q_EMIT propertyChanged(udi, changeMap);
        }
    }

    // Special handling for Media insertion/removal.
    if (iface == UD2_DBUS_INTERFACE_BLOCK && changed.contains("Size")) {
        qulonglong size = changed.value("Size").toULongLong();

        const bool mediaInserted = !knownDevice && size > 0;
        const bool mediaRemoved = knownDevice && size == 0;

        // Short-circuit, don't query Device if the conditions cheap to check aren't fulfilled.
        if (mediaInserted || mediaRemoved) {
            Device device(this, udi);
            if (device.mightBeOpticalDisc()) {
                if (!knownDevice && size > 0) { // we don't know the optdisc, got inserted
                    // Populate at least the stuff we just got, rest has to be fetched on demand.
                    PropertyMap map;
                    map.insert(udi, changed);
                    m_cache.insert(udi, map);
                    Q_EMIT deviceAdded(udi);
                } else if (knownDevice && size == 0) { // we know the optdisc, got removed
                    Q_EMIT deviceRemoved(udi);
                    m_cache.remove(udi);
                }
            }
        }
    }

    // TODO invalidate drive? updateBackend did that
}

QMap<QString, PropertyMap> Manager::deviceCache()
{
    if (m_cache.isEmpty()) {
        allDevices();
    }

    return m_cache;
}

#include "moc_udisksmanager.cpp"
