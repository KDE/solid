/*
    SPDX-FileCopyrightText: 2020 MBition GmbH
    SPDX-FileContributor: Kai Uwe Broulik <kai_uwe.broulik@mbition.io>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "imobilemanager.h"

#include "imobile_debug.h"

#include "../shared/rootdevice.h"
#include "imobile.h"
#include "imobiledevice.h"

using namespace Solid::Backends::IMobile;
using namespace Solid::Backends::Shared;

Manager::Manager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent)
{
    auto ret = idevice_event_subscribe(
        [](const idevice_event_t *event, void *user_data) {
            static_cast<Manager *>(user_data)->onDeviceEvent(event);
        },
        this);
    if (ret != IDEVICE_E_SUCCESS) {
        qCWarning(IMOBILE) << "Failed to subscribe to device events";
    }

    char **devices = nullptr;
    int count = 0;

    ret = idevice_get_device_list(&devices, &count);
    if (ret != IDEVICE_E_SUCCESS && ret != IDEVICE_E_NO_DEVICE) {
        qCWarning(IMOBILE) << "Failed to get list of iOS devices" << ret;
        return;
    }

    m_deviceUdis.reserve(count);

    for (int i = 0; i < count; ++i) {
        m_deviceUdis.append(udiPrefix() + QLatin1Char('/') + QString::fromLatin1(devices[i]));
    }

    if (devices) {
        idevice_device_list_free(devices);
    }
}

Manager::~Manager()
{
    idevice_event_unsubscribe();
}

QObject *Manager::createDevice(const QString &udi)
{
    if (udi == udiPrefix()) {
        RootDevice *root = new RootDevice(udi);
        root->setProduct(tr("iDevice"));
        root->setDescription(tr("iOS devices"));
        root->setIcon("phone-apple-iphone");
        return root;
    }

    if (m_deviceUdis.contains(udi)) {
        return new IMobileDevice(udi);
    }

    return nullptr;
}

QStringList Manager::devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type)
{
    QStringList devices;

    if (!parentUdi.isEmpty() || type != Solid::DeviceInterface::Unknown) {
        for (const QString &udi : m_deviceUdis) {
            IMobileDevice device(udi);
            if (!device.queryDeviceInterface(type)) {
                continue;
            }

            if (!parentUdi.isEmpty() && device.parentUdi() != parentUdi) {
                continue;
            }

            devices << udi;
        }
    }

    return devices;
}

QStringList Manager::allDevices()
{
    return m_deviceUdis;
}

QSet<Solid::DeviceInterface::Type> Manager::supportedInterfaces() const
{
    return {Solid::DeviceInterface::PortableMediaPlayer};
}

QString Manager::udiPrefix() const
{
    return Solid::Backends::IMobile::udiPrefix();
}

void Manager::onDeviceEvent(const idevice_event_t *event)
{
    const QString udi = udiPrefix() + QLatin1Char('/') + QString::fromLatin1(event->udid);

    switch (event->event) {
    case IDEVICE_DEVICE_ADD:
        if (!m_deviceUdis.contains(udi)) {
            m_deviceUdis.append(udi);
            Q_EMIT deviceAdded(udi);
        }
        return;
    case IDEVICE_DEVICE_REMOVE:
        if (m_deviceUdis.removeOne(udi)) {
            Q_EMIT deviceRemoved(udi);
        }
        return;
#if IMOBILEDEVICE_API >= QT_VERSION_CHECK(1, 3, 0)
    case IDEVICE_DEVICE_PAIRED:
        return;
#endif
    }

    qCDebug(IMOBILE) << "Unhandled device event" << event->event << "for" << event->udid;
}
