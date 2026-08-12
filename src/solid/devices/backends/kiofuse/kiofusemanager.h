/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_KIOFUSE_MANAGER_H
#define SOLID_BACKENDS_KIOFUSE_MANAGER_H

#include <solid/devices/ifaces/devicemanager.h>

#include <QDBusServiceWatcher>
#include <QMap>
#include <QSet>
#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace KioFuse
{

/*!
 * Exposes the locations kio-fuse has mounted as Solid devices.
 *
 * kio-fuse keeps every mount inside one FUSE filesystem, so the mount table does
 * not list them and the daemon has to be asked directly.
 */
class Manager : public Solid::Ifaces::DeviceManager
{
    Q_OBJECT

public:
    explicit Manager(QObject *parent);
    ~Manager() override;

    QString udiPrefix() const override;
    QSet<Solid::DeviceInterface::Type> supportedInterfaces() const override;
    QStringList allDevices() override;
    QStringList devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type) override;
    std::unique_ptr<QObject> createDevice(const QString &udi) override;

    QString filePath(const QString &udi) const;

private Q_SLOTS:
    void onMountAdded(const QString &remoteUrl, const QString &localPath);
    void onMountRemoved(const QString &remoteUrl);

private:
    void onServiceOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner);
    void reload();
    void forgetAll();
    void remember(const QString &udi, const QString &localPath);

    QDBusServiceWatcher m_serviceWatcher;
    QSet<Solid::DeviceInterface::Type> m_supportedInterfaces;
    QMap<QString /* udi */, QString /* local path */> m_mounts;
};

}
}
}

#endif // SOLID_BACKENDS_KIOFUSE_MANAGER_H
