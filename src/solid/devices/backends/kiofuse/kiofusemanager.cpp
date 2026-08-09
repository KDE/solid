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
#include <QDBusReply>

using namespace Solid::Backends::KioFuse;
using namespace Solid::Backends::Shared;

static constexpr int s_replyTimeoutMs = 500;

Manager::Manager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent)
    , m_serviceWatcher(Utils::dbusService(), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange)
{
    qDBusRegisterMetaType<QMap<QString, QString>>();

    m_supportedInterfaces << Solid::DeviceInterface::StorageAccess;

    connect(&m_serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &Manager::onServiceOwnerChanged);

    reload();
}

Manager::~Manager() = default;

QString Manager::udiPrefix() const
{
    return Utils::udiPrefix();
}

QSet<Solid::DeviceInterface::Type> Manager::supportedInterfaces() const
{
    return m_supportedInterfaces;
}

QStringList Manager::allDevices()
{
    QStringList result;
    result << udiPrefix();
    result << m_mounts.keys();
    return result;
}

QStringList Manager::devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type)
{
    if (type != Solid::DeviceInterface::Unknown && !m_supportedInterfaces.contains(type)) {
        return QStringList();
    }

    if (parentUdi.isEmpty() || parentUdi == udiPrefix()) {
        return m_mounts.keys();
    }

    if (m_mounts.contains(parentUdi)) {
        return QStringList{parentUdi};
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
    const QDBusMessage msg = QDBusMessage::createMethodCall(Utils::dbusService(), Utils::dbusPath(), Utils::dbusInterface(), QStringLiteral("mounts"));

    const QDBusReply<QMap<QString, QString>> reply = QDBusConnection::sessionBus().call(msg, QDBus::Block, s_replyTimeoutMs);
    if (!reply.isValid()) {
        qCWarning(KIOFUSE_LOG) << "Could not list mounts:" << reply.error().message();
        return;
    }

    const QMap<QString, QString> mounts = reply.value();

    QMap<QString, QString> fresh;
    for (auto it = mounts.cbegin(); it != mounts.cend(); ++it) {
        fresh.insert(Utils::udiForUrl(QUrl(it.key())), it.value());
    }

    const QStringList knownUdis = m_mounts.keys();
    for (const QString &udi : knownUdis) {
        if (!fresh.contains(udi)) {
            m_mounts.remove(udi);
            Q_EMIT deviceRemoved(udi);
        }
    }

    for (auto it = fresh.cbegin(); it != fresh.cend(); ++it) {
        remember(it.key(), it.value());
    }
}

void Manager::remember(const QString &udi, const QString &localPath)
{
    const bool isNew = !m_mounts.contains(udi);
    m_mounts.insert(udi, localPath);

    if (isNew) {
        qCDebug(KIOFUSE_LOG) << "Mount appeared at" << localPath;
        Q_EMIT deviceAdded(udi);
    }
}

void Manager::forgetAll()
{
    const QStringList udis = m_mounts.keys();
    m_mounts.clear();

    for (const QString &udi : udis) {
        Q_EMIT deviceRemoved(udi);
    }
}

void Manager::onServiceOwnerChanged(const QString &service, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(service);
    Q_UNUSED(oldOwner);

    forgetAll();

    if (!newOwner.isEmpty()) {
        reload();
    }
}

#include "moc_kiofusemanager.cpp"
