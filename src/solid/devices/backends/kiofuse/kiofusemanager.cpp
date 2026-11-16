/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kiofusemanager.h"
#include "kiofuse_debug.h"
#include "kiofusedevice.h"
#include "kiofuseutils.h"

#include "../shared/rootdevice.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

using namespace Solid::Backends::KioFuse;
using namespace Solid::Backends::Shared;

Manager::Manager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent)
    , m_serviceWatcher(Utils::dbusService(), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange)
{
    qDBusRegisterMetaType<QMap<QString, QString>>();

    connect(&m_serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &Manager::onServiceOwnerChanged);

    if (!QDBusConnection::sessionBus().connect(Utils::dbusService(),
                                               Utils::dbusPath(),
                                               Utils::dbusInterface(),
                                               QStringLiteral("mountAdded"),
                                               this,
                                               SLOT(onMountAdded(QString, QString)))) {
        qCWarning(KIOFUSE_LOG) << "Could not subscribe to the mountAdded signal of kio-fuse";
    }

    reload();
}

Manager::~Manager() = default;

QString Manager::udiPrefix() const
{
    return Utils::udiPrefix();
}

QSet<Solid::DeviceInterface::Type> Manager::supportedInterfaces() const
{
    return {Solid::DeviceInterface::StorageAccess, Solid::DeviceInterface::NetworkShare};
}

QStringList Manager::allDevices()
{
    return QStringList{udiPrefix()} + m_mounts.keys();
}

QStringList Manager::devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type)
{
    if (parentUdi.isEmpty() || parentUdi == udiPrefix()) {
        return m_mounts.keys();
    }

    return QStringList();
}

std::unique_ptr<QObject> Manager::createDevice(const QString &udi)
{
    if (udi == udiPrefix()) {
        std::unique_ptr<RootDevice> root = std::make_unique<RootDevice>(udi);

        root->setProduct(tr("Remote Filesystems"));
        root->setDescription(tr("Remote locations mounted as files"));
        root->setIcon(QStringLiteral("folder-remote"));

        return root;
    }

    if (!m_mounts.contains(udi)) {
        return nullptr;
    }

    return std::make_unique<Device>(this, udi);
}

QString Manager::filePath(const QString &udi) const
{
    return m_mounts.value(udi);
}

void Manager::reload()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(Utils::dbusService(), Utils::dbusPath(), Utils::dbusInterface(), QStringLiteral("mounts"));
    msg.setAutoStartService(false);

    auto *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(msg), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        watcher->deleteLater();

        const QDBusPendingReply<QMap<QString, QString>> reply = *watcher;
        if (reply.isError()) {
            if (reply.error().name() != QLatin1String("org.freedesktop.DBus.Error.NameHasNoOwner")) {
                qCWarning(KIOFUSE_LOG) << "Could not list mounts:" << reply.error().message();
            }
            return;
        }

        const QMap<QString, QString> mounts = reply.value();
        for (auto it = mounts.cbegin(); it != mounts.cend(); ++it) {
            onMountAdded(it.key(), it.value());
        }
    });
}

void Manager::onMountAdded(const QString &remoteUrl, const QString &localPath)
{
    const QUrl url(remoteUrl);
    if (url.host().isEmpty()) {
        return;
    }

    const QString udi = Utils::udiForUrl(url);
    const bool isNew = !m_mounts.contains(udi);
    m_mounts.insert(udi, localPath);

    if (isNew) {
        qCDebug(KIOFUSE_LOG) << "Mount appeared at" << localPath;
        Q_EMIT deviceAdded(udi);
    }
}

void Manager::onServiceOwnerChanged(const QString &, const QString &, const QString &newOwner)
{
    const QStringList udis = m_mounts.keys();
    m_mounts.clear();

    for (const QString &udi : udis) {
        Q_EMIT deviceRemoved(udi);
    }

    if (!newOwner.isEmpty()) {
        reload();
    }
}

#include "moc_kiofusemanager.cpp"
