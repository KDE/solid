/*
    Copyright 2010 Mario Bensi <mbensi@ipsquad.net>

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

#include "fstabdevice.h"
#include "fstabhandling.h"
#include "fstabnetworkshare.h"
#include "fstabstorageaccess.h"
#include "fstabservice.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

using namespace Solid::Backends::Fstab;

FstabDevice::FstabDevice(QString uid) :
    Solid::Ifaces::Device(),
    m_uid(uid)
{
    m_device = m_uid;
    m_device.remove(parentUdi() + "/");

    if (m_device.startsWith("//")) {
        m_vendor = m_device.mid(2, m_device.indexOf("/", 2) - 2);
        m_product = m_device.mid(m_device.indexOf("/", 2) + 1);
    } else {
        m_vendor = m_device.left(m_device.indexOf(":/"));
        m_product = m_device.mid(m_device.indexOf(":/") + 2);
    }

    const QStringList& gvfsOptions = FstabHandling::options(m_device);

    foreach (const QString& option, gvfsOptions) {
        if (option.startsWith(QLatin1String("x-gvfs-name="))) {
            QStringRef encoded = option.midRef(12);
            m_description = QUrl::fromPercentEncoding(encoded.toLatin1());
        } else if (option.startsWith(QLatin1String("x-gvfs-icon="))) {
            QStringRef encoded = option.midRef(12);
            m_iconName = QUrl::fromPercentEncoding(encoded.toLatin1());
        }
    }

    if (m_description.isEmpty()) {
        m_description = QCoreApplication::translate("", "%1 on %2",
            "%1 is sharename, %2 is servername").arg(m_product).arg(m_vendor);
    }

    if (m_iconName.isEmpty()) {
        m_iconName = QLatin1String("network-server");
    }
}

FstabDevice::~FstabDevice()
{
}

QString FstabDevice::udi() const
{
    return m_uid;
}

QString FstabDevice::parentUdi() const
{
    return QString::fromLatin1(FSTAB_UDI_PREFIX);
}

QString FstabDevice::vendor() const
{
    return m_vendor;
}

QString FstabDevice::product() const
{
    return m_product;
}

QString FstabDevice::icon() const
{
    return m_iconName;
}

QStringList FstabDevice::emblems() const
{
    QStringList res;
    if (!m_storageAccess) {
        FstabDevice *d = const_cast<FstabDevice *>(this);
        d->m_storageAccess = new FstabStorageAccess(d);
    }
    if (m_storageAccess->isAccessible()) {
        res << "emblem-mounted";
    } else {
        res << "emblem-unmounted";
    }

    return res;
}

QString FstabDevice::description() const
{
    return m_description;
}

bool FstabDevice::queryDeviceInterface(const Solid::DeviceInterface::Type &type) const
{
    if (type == Solid::DeviceInterface::StorageAccess
            || type == Solid::DeviceInterface::NetworkShare) {
        return true;
    }
    return false;
}

QObject *FstabDevice::createDeviceInterface(const Solid::DeviceInterface::Type &type)
{
    if (type == Solid::DeviceInterface::StorageAccess) {
        if (!m_storageAccess) {
            m_storageAccess = new FstabStorageAccess(this);
        }
        return m_storageAccess;
    } else if (type == Solid::DeviceInterface::NetworkShare) {
        return new FstabNetworkShare(this);
    }
    return nullptr;
}

QString FstabDevice::device() const
{
    return m_device;
}

void FstabDevice::onMtabChanged(const QString &device)
{
    if (m_device == device) {
        emit mtabChanged(device);
    }
}
