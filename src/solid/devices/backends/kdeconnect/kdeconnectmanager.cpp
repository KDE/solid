/*
    Copyright 2013 Albert Vaca <albertvaka@gmail.com>

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

#include "kdeconnectmanager.h"
#include "kdeconnectdevice.h"

#include <QtDBus/QDBusReply>
#include <QtCore/QDebug>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusConnectionInterface>
#include <QDBusServiceWatcher>

#include "../shared/rootdevice.h"

static const char KDECONNECT_SERVICE[] = "org.kde.kdeconnect";
static const char KDECONNECT_PATH[] = "/org.kde.kdeconnect";

using namespace Solid::Backends::KdeConnect;
using namespace Solid::Backends::Shared;

KdeConnectManager::KdeConnectManager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent)
    , m_manager(KDECONNECT_SERVICE,
                QStringLiteral("/modules/kdeconnect"),
                QStringLiteral("org.kde.kdeconnect.daemon"),
                QDBusConnection::sessionBus())
    , m_ready(false)
{
    m_supportedInterfaces << Solid::DeviceInterface::Battery
                          << Solid::DeviceInterface::PortableMediaPlayer;

    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(KDECONNECT_SERVICE,
                                                           QDBusConnection::sessionBus(),
                                                           QDBusServiceWatcher::WatchForRegistration |
                                                           QDBusServiceWatcher::WatchForUnregistration,
                                                           this);
    connect(watcher, &QDBusServiceWatcher::serviceRegistered, this, &KdeConnectManager::onServiceRegistered);
    connect(watcher, &QDBusServiceWatcher::serviceUnregistered, this, &KdeConnectManager::onServiceUnregistered);

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(KDECONNECT_SERVICE)) {
        onServiceRegistered(QString());
    }
}

KdeConnectManager::~KdeConnectManager()
{

}

QObject *KdeConnectManager::createDevice(const QString &udi)
{
    if (!m_ready) {
        return Q_NULLPTR;
    }

    qDebug() << "createDevice" << udi;

    if (udi == udiPrefix()) {
        RootDevice *root = new RootDevice(udiPrefix());

        root->setProduct(tr("KDE Connect"));
        root->setDescription(tr("Information about devices available through KDE Connect"));
        root->setIcon("kdeconnect");

        return root;
    } else if (allDevices().contains(udi)) {
        const QString id = udi.mid(QString(udiPrefix() + QLatin1Char('/')).length());
        return new KdeConnectDevice(udi, id);
    }

    return Q_NULLPTR;
}

QStringList KdeConnectManager::devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type)
{
    qDebug() << "DEVICES FROM QUERY";
    if (!m_ready) {
        qDebug() << "NOT READY";
        return QStringList();
    }

    if (type != Solid::DeviceInterface::Battery
        && type != Solid::DeviceInterface::PortableMediaPlayer) {
        qDebug() << "neither battery nor portable media player";
        return QStringList();
    }

    if (!parentUdi.isEmpty() && parentUdi != udiPrefix()) {
        qDebug() << "parent udi" << parentUdi << "is" << udiPrefix();
        return QStringList();
    }

    return allDevices();
}

QStringList KdeConnectManager::allDevices()
{
    qDebug() << "all devices";
    if (!m_ready) {
        return QStringList();
    }

    QVariantList args; // TODO initializer_list
    args << true /*onlyReachable*/ << true /*onlyVisible*/;

    QDBusPendingReply<QStringList> pendingDeviceList = m_manager.asyncCallWithArgumentList("devices", args);
    pendingDeviceList.waitForFinished(); // urgh...
    const QStringList &deviceList = pendingDeviceList.value();

    m_devices.clear(); // also update our internal device "cache"

    QStringList udis;
    Q_FOREACH(const QString &device, deviceList) {
        udis.append(prefixedId(device));
        m_devices.append(device);
    }

    qDebug() << "telling it" << udis;

    return udis;
}

QSet<Solid::DeviceInterface::Type> KdeConnectManager::supportedInterfaces() const
{
    return m_supportedInterfaces;
}

QString KdeConnectManager::udiPrefix() const
{
    return QStringLiteral("/org/kde/kdeconnect");
}

void KdeConnectManager::onServiceRegistered(const QString &service)
{
    Q_UNUSED(service);

    m_ready = true;
    connect(&m_manager, SIGNAL(deviceAdded(QString)), this, SLOT(onDeviceAdded(QString)));
    connect(&m_manager, SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceRemoved(QString)));
    connect(&m_manager, SIGNAL(deviceVisibilityChanged(QString,bool)), this, SLOT(onDeviceVisibilityChanged(QString,bool)));
}

void KdeConnectManager::onServiceUnregistered(const QString &service)
{
    Q_UNUSED(service);

    m_ready = false;
    disconnect(&m_manager, SIGNAL(deviceAdded(QString)), this, SLOT(onDeviceAdded(QString)));
    disconnect(&m_manager, SIGNAL(deviceRemoved(QString)), this, SLOT(onDeviceRemoved(QString)));
    disconnect(&m_manager, SIGNAL(deviceVisibilityChanged(QString,bool)), this, SLOT(onDeviceVisibilityChanged(QString,bool)));
}

void KdeConnectManager::onDeviceAdded(const QString &id)
{
    qDebug() << "Device added" << id;
    if (m_devices.contains(id)) {
        qDebug() << "But we already know that one";
        return;
    }

    m_devices.append(id);
    emit deviceAdded(prefixedId(id));
}

void KdeConnectManager::onDeviceRemoved(const QString &id)
{
    qDebug() << "device removed" << id;
    if (!m_devices.contains(id)) {
        qDebug() << "we don't know that one";
        return;
    }

    m_devices.removeAll(id);
    emit deviceRemoved(prefixedId(id));
}

void KdeConnectManager::onDeviceVisibilityChanged(const QString &id, bool visible)
{
    qDebug() << "device" << id << "visiblity changed to" << visible;
    if (visible) {
        onDeviceAdded(id);
    } else {
        onDeviceRemoved(id);
    }
}

QString KdeConnectManager::prefixedId(const QString &id) const
{
    return udiPrefix() + QLatin1Char('/') + id;
}

#include "backends/kdeconnectmanager/kdeconnectmanager.moc"
