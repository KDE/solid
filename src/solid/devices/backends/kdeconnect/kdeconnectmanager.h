/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_KDECONNECT_MANAGER_H
#define SOLID_BACKENDS_KDECONNECT_MANAGER_H

#include "solid/devices/ifaces/devicemanager.h"

#include "dbus/kdeconnect_daemon_interface.h"

#include <QDBusServiceWatcher>
#include <QSet>

namespace Solid
{
namespace Backends
{
namespace KdeConnect
{
class Manager : public Solid::Ifaces::DeviceManager
{
    Q_OBJECT

public:
    explicit Manager(QObject *parent);
    ~Manager() override;

    std::unique_ptr<QObject> createDevice(const QString &udi) override;
    QStringList devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type) override;
    QStringList allDevices() override;
    QSet<Solid::DeviceInterface::Type> supportedInterfaces() const override;
    QString udiPrefix() const override;

    QVariant deviceProperty(const QString &udi, const QString &property) const;

private:
    QString udiFromDeviceId(const QString &deviceId) const;
    void reloadDevices();
    void fetchDeviceProperties(const QString &deviceId);
    void fetchLoadedModules(const QString &deviceId);
    void addOrRemoveDevice(const QString &udi, bool loadedPluginsChanged = false);

    void onServiceOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner);
    void onDeviceAdded(const QString &deviceId);
    void onDeviceVisibilityChanged(const QString &deviceId, bool visible);
    void onDeviceRemoved(const QString &deviceId);
    void onPairingRequestsChanged();

    org::kde::kdeconnect::daemon m_daemonIface;
    QDBusServiceWatcher m_serviceWatcher;

    QStringList m_interestingDevices; // paired and reachable.
    QMap<QString /*udi*/, QVariantMap> m_cache;
};

}
}
}

#endif // SOLID_BACKENDS_KDECONNECT_MANAGER_H
