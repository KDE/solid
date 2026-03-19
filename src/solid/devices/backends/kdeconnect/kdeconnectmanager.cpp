/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "kdeconnect_debug.h"

#include "kdeconnectdevice.h"
#include "kdeconnectmanager.h"
#include "kdeconnectutils.h"

#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>

#include <algorithm>

#include "../shared/rootdevice.h"

#include "dbus/kdeconnect_device_interface.h"

using namespace Solid::Backends::KdeConnect;
using namespace Solid::Backends::Shared;

Manager::Manager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent)
    , m_daemonIface(Utils::dbusService(), QStringLiteral("/modules/kdeconnect"), QDBusConnection::sessionBus())
    , m_serviceWatcher(Utils::dbusService(), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange)
{
    qDBusRegisterMetaType<QVariantMap>();

    connect(&m_serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &Manager::onServiceOwnerChanged);

    connect(&m_daemonIface, &org::kde::kdeconnect::daemon::deviceAdded, this, &Manager::onDeviceAdded);
    connect(&m_daemonIface, &org::kde::kdeconnect::daemon::deviceVisibilityChanged, this, &Manager::onDeviceVisibilityChanged);
    connect(&m_daemonIface, &org::kde::kdeconnect::daemon::deviceRemoved, this, &Manager::onDeviceRemoved);
    connect(&m_daemonIface, &org::kde::kdeconnect::daemon::pairingRequestsChanged, this, &Manager::onPairingRequestsChanged);

    reloadDevices();
}

Manager::~Manager() = default;

std::unique_ptr<QObject> Manager::createDevice(const QString &udi)
{
    if (udi == udiPrefix()) {
        std::unique_ptr<RootDevice> root = std::make_unique<RootDevice>(udiPrefix());

        root->setProduct(tr("KDE Connect"));
        root->setDescription(tr("Battery and storage from connected devices"));
        root->setIcon(QStringLiteral("kdeconnect"));

        return root;

    } else if (m_interestingDevices.contains(udi)) {
        return std::make_unique<Device>(this, udi);

    } else {
        return nullptr;
    }
}

QStringList Manager::devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type)
{
    const QStringList allDev = allDevices();

    QStringList result;

    if (!parentUdi.isEmpty()) {
        for (const QString &udi : allDev) {
            if (udi == udiPrefix()) {
                continue;
            }

            Device device(this, udi);
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

            Device device(this, udi);
            if (device.queryDeviceInterface(type)) {
                result << udi;
            }
        }

        return result;
    } else {
        return allDev;
    }
}

void Manager::reloadDevices()
{
    qCDebug(KDECONNECT) << "Loading devices...";

    auto call = m_daemonIface.devices(true /*reachable*/, true /*paired*/);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        watcher->deleteLater();

        QDBusPendingReply<QStringList> reply = *watcher;
        if (reply.isError()) {
            qCWarning(KDECONNECT) << "Failed to get list of devices:" << reply.error().message();
            return;
        }

        const QStringList newDeviceIds = reply.value();

        const QStringList oldUdis = m_interestingDevices;
        for (const QString &oldUdi : oldUdis) {
            if (std::find_if(newDeviceIds.begin(),
                             newDeviceIds.end(),
                             [this, &oldUdi](const QString &deviceId) {
                                 return udiFromDeviceId(deviceId) == oldUdi;
                             })
                == newDeviceIds.end()) {
                qCDebug(KDECONNECT) << "Device gone" << oldUdi;
                m_interestingDevices.removeOne(oldUdi);
                m_cache.remove(oldUdi);
                Q_EMIT deviceRemoved(oldUdi);
            }
        }

        for (const QString &deviceId : newDeviceIds) {
            fetchDeviceProperties(deviceId);
        }
    });
}

void Manager::fetchDeviceProperties(const QString &deviceId)
{
    const QString udi = udiFromDeviceId(deviceId);

    qCDebug(KDECONNECT) << "Fetch device properties" << udi;

    OrgKdeKdeconnectDeviceInterface deviceIface(m_daemonIface.service(), Utils::deviceDBusPath(deviceId), m_daemonIface.connection());
    auto message = Utils::getAllPropertiesCall(deviceIface);

    auto call = m_daemonIface.connection().asyncCall(message);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher, udi, deviceId] {
        watcher->deleteLater();

        QDBusPendingReply<QVariantMap> reply = *watcher;
        if (!reply.isValid()) {
            qCWarning(KDECONNECT) << "Failed to get all device properties for" << udi << reply.error().message();
            return;
        }

        const QVariantMap properties = reply.value();
        m_cache.insert(udi, properties);

        fetchLoadedModules(deviceId);
    });
}

void Manager::fetchLoadedModules(const QString &deviceId)
{
    const QString udi = udiFromDeviceId(deviceId);

    if (!m_cache.value(udi).value(QStringLiteral("isReachable")).toBool()) {
        // Cannot know what modules are loaded when it's unreachable.
        return;
    }

    qCDebug(KDECONNECT) << "Fetching loaded modules" << deviceId;
    OrgKdeKdeconnectDeviceInterface deviceIface(m_daemonIface.service(), Utils::deviceDBusPath(deviceId), m_daemonIface.connection());
    auto pluginsCall = deviceIface.loadedPlugins();
    auto *pluginsWatcher = new QDBusPendingCallWatcher(pluginsCall);
    connect(pluginsWatcher, &QDBusPendingCallWatcher::finished, this, [this, pluginsWatcher, udi] {
        pluginsWatcher->deleteLater();

        QDBusPendingReply<QStringList> pluginsReply = *pluginsWatcher;
        if (!pluginsReply.isValid()) {
            qCWarning(KDECONNECT) << "Failed to get loaded plugins for" << udi << pluginsReply.error().message();
            return;
        }

        QVariantMap &deviceCache = m_cache[udi];
        const QStringList newLoadedPlugins = pluginsReply.value();

        bool loadedPluginsChanged = false;
        auto pluginsIt = deviceCache.find(QStringLiteral("loadedPlugins"));
        if (pluginsIt != deviceCache.end()) {
            loadedPluginsChanged = (newLoadedPlugins != *pluginsIt);
        }

        deviceCache.insert(QStringLiteral("loadedPlugins"), newLoadedPlugins);

        addOrRemoveDevice(udi, loadedPluginsChanged);
    });
}

QStringList Manager::allDevices()
{
    return m_interestingDevices;
}

QSet<Solid::DeviceInterface::Type> Manager::supportedInterfaces() const
{
    return {Solid::DeviceInterface::Battery, Solid::DeviceInterface::PortableMediaPlayer};
}

QString Manager::udiPrefix() const
{
    return Utils::udiPrefix();
}

QString Manager::udiFromDeviceId(const QString &deviceId) const
{
    return udiPrefix() + QLatin1Char('/') + deviceId;
}

QVariant Manager::deviceProperty(const QString &udi, const QString &property) const
{
    return m_cache.value(udi).value(property);
}

void Manager::onServiceOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(service);

    qCDebug(KDECONNECT) << "service owner changed from" << oldOwner << "to" << newOwner;
    for (const QString &udi : std::as_const(m_interestingDevices)) {
        Q_EMIT deviceRemoved(udi);
    }
    m_interestingDevices.clear();
    m_cache.clear();

    if (!newOwner.isEmpty()) {
        reloadDevices();
    }
}

void Manager::onDeviceAdded(const QString &deviceId)
{
    const QString udi = udiFromDeviceId(deviceId);

    if (m_interestingDevices.contains(udi)) {
        return;
    }

    qCDebug(KDECONNECT) << "Device added" << deviceId;

    // will emit deviceAdded
    fetchDeviceProperties(deviceId);
}

void Manager::onDeviceVisibilityChanged(const QString &deviceId, bool isReachable)
{
    const QString udi = udiFromDeviceId(deviceId);
    qCDebug(KDECONNECT) << "Device reachability for" << udi << "changed to" << isReachable;

    auto cacheIt = m_cache.find(udi);
    if (cacheIt == m_cache.end()) {
        // Device wasn't cached before because it wasn't visible+reachable, fetch it now.
        if (isReachable) {
            fetchDeviceProperties(deviceId);
        }
        return;
    }

    // When pairing a device, its visibility is emitted even when it hasn't really changed.
    if (isReachable == cacheIt->value(QStringLiteral("isReachable")).toBool()) {
        return;
    }

    cacheIt->insert(QStringLiteral("isReachable"), isReachable);

    if (isReachable) {
        // Will call addOrRemoveDevice once finished.
        fetchLoadedModules(deviceId);
    } else {
        addOrRemoveDevice(udi);
    }
}

void Manager::onDeviceRemoved(const QString &deviceId)
{
    const QString udi = udiFromDeviceId(deviceId);

    const int deviceIndex = m_interestingDevices.indexOf(udi);
    if (deviceIndex > -1) {
        qCDebug(KDECONNECT) << "Device removed" << udi;
        m_interestingDevices.remove(deviceIndex);
        Q_EMIT deviceRemoved(udi);
    }
    m_cache.remove(udi);
}

void Manager::onPairingRequestsChanged()
{
    // Check if any devices are now or no longer paired.
    // This could probably be optimized...
    qCDebug(KDECONNECT) << "Pairing requests changed, reloading devices";
    reloadDevices();
}

void Manager::addOrRemoveDevice(const QString &udi, bool loadedPluginsChanged)
{
    const QVariantMap deviceCache = m_cache.value(udi);
    if (deviceCache.isEmpty()) {
        return;
    }

    const bool isReachable = deviceCache.value(QStringLiteral("isReachable")).toBool();
    const bool isPaired = deviceCache.value(QStringLiteral("isPaired")).toBool();
    const QStringList loadedPlugins = deviceCache.value(QStringLiteral("loadedPlugins")).toStringList();

    const bool isInteresting = isReachable && isPaired && !loadedPlugins.isEmpty();

    if (isInteresting) {
        if (!m_interestingDevices.contains(udi)) {
            qCDebug(KDECONNECT) << "Device is now interesting, adding" << udi;
            m_interestingDevices.append(udi);
            Q_EMIT deviceAdded(udi);
        }
    } else if (m_interestingDevices.contains(udi)) {
        qCDebug(KDECONNECT) << "Device is no longer interesting, removing" << udi;
        m_interestingDevices.removeOne(udi);
        Q_EMIT deviceRemoved(udi);
    } else if (loadedPluginsChanged) {
        qCDebug(KDECONNECT) << "Loaded plugins changed, removing and re-adding" << udi;
        Q_EMIT deviceRemoved(udi);
        Q_EMIT deviceAdded(udi);
    }
}

#include "moc_kdeconnectmanager.cpp"
