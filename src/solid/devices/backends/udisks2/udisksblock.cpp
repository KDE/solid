/*
    SPDX-FileCopyrightText: 2012 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "udisksblock.h"

#if defined(Q_OS_LINUX)
#include <linux/kdev_t.h>
#else
// taken from linux/kdev_t.h
#define MINORBITS 20
#define MINORMASK ((1U << MINORBITS) - 1)
#define MAJOR(dev) ((unsigned int)((dev) >> MINORBITS))
#define MINOR(dev) ((unsigned int)((dev) & MINORMASK))
#endif

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDomDocument>
#include <QFile>

#include "udisks_debug.h"

using namespace Solid::Backends::UDisks2;

Block::Block(Device *dev)
    : DeviceInterface(dev)
    , m_devNum(0)
{
    QDBusMessage call = QDBusMessage::createMethodCall(QStringLiteral(UD2_DBUS_SERVICE),
                                                       QStringLiteral(UD2_DBUS_PATH),
                                                       QStringLiteral(DBUS_INTERFACE_MANAGER),
                                                       QStringLiteral("GetManagedObjects"));
    QDBusPendingReply<DBUSManagerStruct> reply = QDBusConnection::systemBus().asyncCall(call);
    reply.waitForFinished();

    if (!reply.isValid()) {
        qCWarning(UDISKS2) << "Failed enumerating UDisks2 objects:" << reply.error().name() << QStringLiteral("\n") << reply.error().message();
        return;
    }

    auto getProp = [](const VariantMapMap &ifaces, const QString &iface, const QString &prop) {
        return ifaces.value(iface).value(prop);
    };

    DBUSManagerStruct objects = reply.value();

    const QString udi = dev->udi();

    auto objectIt = objects.find(QDBusObjectPath(udi));
    if (objectIt == objects.end()) {
        return;
    }

    const VariantMapMap &ifaces = objectIt.value();

    // try direct block properties
    if (ifaces.contains(QStringLiteral(UD2_DBUS_INTERFACE_BLOCK))) {
        m_devNum = getProp(ifaces, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("DeviceNumber")).toULongLong();
        m_devFile = QFile::decodeName(getProp(ifaces, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("Device")).toByteArray());
        m_hintSystem = getProp(ifaces, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("HintSystem")).toBool();
    }

    // we have a drive (non-block device for udisks), so let's find the corresponding (real) block device
    if (m_devNum == 0 || m_devFile.isEmpty()) {
        for (auto objectIt = objects.begin(); objectIt != objects.end(); ++objectIt) {
            const VariantMapMap &interfaces = objectIt.value();

            if (!interfaces.contains(QStringLiteral(UD2_DBUS_INTERFACE_BLOCK))) {
                continue;
            }

            QDBusObjectPath drivePath = getProp(interfaces, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("Drive")).value<QDBusObjectPath>();

            if (drivePath.path() != udi) {
                continue;
            }

            // if partition -> get parent block (partition table)
            if (interfaces.contains(QStringLiteral(UD2_DBUS_INTERFACE_PARTITION))) {
                QDBusObjectPath tablePath = getProp(interfaces, QStringLiteral(UD2_DBUS_INTERFACE_PARTITION), QStringLiteral("Table")).value<QDBusObjectPath>();

                auto tableIt = objects.find(tablePath);
                if (tableIt != objects.end()) {
                    const VariantMapMap &tableIfs = tableIt.value();

                    m_devNum = getProp(tableIfs, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("DeviceNumber")).toULongLong();
                    m_devFile = QFile::decodeName(getProp(tableIfs, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("Device")).toByteArray());
                    m_hintSystem = getProp(tableIfs, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("HintSystem")).toBool();
                    break;
                }
            }

            // else use this block device directly
            m_devNum = getProp(interfaces, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("DeviceNumber")).toULongLong();
            m_devFile = QFile::decodeName(getProp(interfaces, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("Device")).toByteArray());
            m_hintSystem = getProp(interfaces, QStringLiteral(UD2_DBUS_INTERFACE_BLOCK), QStringLiteral("HintSystem")).toBool();
            break;
        }
    }

    // qDebug() << "devnum:" << m_devNum << "dev file:" << m_devFile;
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

bool Block::isSystem() const
{
    return m_hintSystem;
}

#include "moc_udisksblock.cpp"
