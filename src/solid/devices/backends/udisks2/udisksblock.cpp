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

#include "udisksblock.h"
#include "udisksdevicebackend.h"
#include "udisksmanager.h"

#if defined(Q_OS_LINUX)
#include <linux/kdev_t.h>
#else
// taken from linux/kdev_t.h
#define MINORBITS       20
#define MINORMASK       ((1U << MINORBITS) - 1)
#define MAJOR(dev)      ((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)      ((unsigned int) ((dev) & MINORMASK))
#endif

#include <QFile>
#include <QDBusConnection>
#include <QDBusPendingReply>

#include "udisks_debug.h"

using namespace Solid::Backends::UDisks2;

Block::Block(Device *dev)
    : DeviceInterface(dev)
{
    m_devNum = m_device->prop("DeviceNumber").toULongLong();
    m_devFile = QFile::decodeName(m_device->prop("Device").toByteArray());

    // we have a drive (non-block device for udisks), so let's find the corresponding (real) block device
    if (m_devNum == 0 || m_devFile.isEmpty()) {
        // kinda breaks the encapsulation if this thing ends up calling all the way into the Manager...
        const auto allProperties = DeviceBackend::manager()->allProperties();
        for (auto it = allProperties.begin(), end = allProperties.end(); it != end; ++it) {
            const QVariantMap blockProps = it->value(UD2_DBUS_INTERFACE_BLOCK);

            // FIXME the old code introspecting DBus found "/dev/nvme0n1p3" for my SSD
            // whereas the new code finds "/dev/nvme0n1" which looks more sensible
            // but this could have some unwanted implications.
            // Not really sure what this code is trying to achieve anyway, but I think
            // returning the drive device instead of a (random?) partition on it is better

            const QString drivePath = blockProps.value(QStringLiteral("Drive")).value<QDBusObjectPath>().path();
            if (drivePath != dev->udi()) {
                continue;
            }

            m_devNum = blockProps.value(QStringLiteral("DeviceNumber")).toULongLong();
            m_devFile = QFile::decodeName(blockProps.value(QStringLiteral("Device")).toByteArray());
            break;
        }
    }

    //qDebug() << "devnum:" << m_devNum << "dev file:" << m_devFile;
}

Block::~Block()
{
}

QString Block::device() const
{
    return m_devFile;
}

int Block::deviceMinor() const
{
    return MINOR(m_devNum);
}

int Block::deviceMajor() const
{
    return MAJOR(m_devNum);
}
