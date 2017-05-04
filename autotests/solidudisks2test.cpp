/*
    Copyright 2017 Konstantinos Tsanaktsidis <kjtsanaktsidis@gmail.com>

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

#include "qtest_dbus.h"
#include "fakeUdisks2.h"

#include <QTest>
#include <QDBusConnection>
#include <Solid/Device>
#include <Solid/Block>
#include <Solid/Predicate>
#include <Solid/DeviceNotifier>

#if HAVE_SYSMACROS_MAJOR_MINOR
#include <sys/sysmacros.h>
#elif HAVE_SYSTYPES_MAJOR_MINOR
#include <sys/types.h>
#elif HAVE_SYSMKDEV_MAJOR_MINOR
#include <sys/mkdev.h>
#else
#error "Could not find a major/minor/makedev implementation"
#endif

static Solid::Predicate majorMinorPredicate(quint32 majorNum, quint32 minorNum)
{
    Solid::Predicate hasMajorMinor(
        Solid::DeviceInterface::Type::Block,
        QStringLiteral("major"),
        QVariant(majorNum),
        Solid::Predicate::ComparisonOperator::Equals
    );
    hasMajorMinor &= Solid::Predicate(
        Solid::DeviceInterface::Type::Block,
        QStringLiteral("minor"),
        QVariant(minorNum),
        Solid::Predicate::ComparisonOperator::Equals
    );
    return hasMajorMinor;
}


class SolidUdisks2Test : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testDeviceNumbersAreDecomposed();
    void testLargeDeviceNumbersAreDecomposed();

private:
    FakeUdisks2BlockDevice *m_deviceSda1;
    FakeUdisks2BlockDevice *m_deviceNvme0n1p2;
    QObject *m_emptyDbus;
};

void SolidUdisks2Test::initTestCase()
{
    m_deviceSda1 = new FakeUdisks2BlockDevice(this, "sda1", makedev(8, 1));
    m_deviceNvme0n1p2 = new FakeUdisks2BlockDevice(this, "nvme0n1p2", makedev(259, 2));
    m_emptyDbus = new QObject(this);
    QDBusConnection::systemBus().registerService(QStringLiteral("org.freedesktop.UDisks2"));
    QDBusConnection::systemBus().registerObject(QStringLiteral("/org/freedesktop/UDisks2/block_devices/sda1"), m_deviceSda1, QDBusConnection::ExportAllContents);
    QDBusConnection::systemBus().registerObject(QStringLiteral("/org/freedesktop/UDisks2/block_devices/nvme0n1p2"), m_deviceNvme0n1p2, QDBusConnection::ExportAllContents);
    QDBusConnection::systemBus().registerObject(QStringLiteral("/org/freedesktop/UDisks2/drives"), m_emptyDbus, QDBusConnection::ExportAllContents);
}

void SolidUdisks2Test::testDeviceNumbersAreDecomposed()
{
    auto hasMajorMinor = majorMinorPredicate(8, 1);
    QList<Solid::Device> solidSda1BlockDeviceList = Solid::Device::listFromQuery(hasMajorMinor);
    QCOMPARE(solidSda1BlockDeviceList.count(), 1);

    auto solidSda1BlockDevice = dynamic_cast<Solid::Block*>(
            solidSda1BlockDeviceList.first().asDeviceInterface(Solid::DeviceInterface::Type::Block)
    );
    QVERIFY(solidSda1BlockDevice);

    QCOMPARE(solidSda1BlockDevice->deviceMajor(), 8);
    QCOMPARE(solidSda1BlockDevice->deviceMinor(), 1);
}

void SolidUdisks2Test::testLargeDeviceNumbersAreDecomposed()
{
    // On Linux, nvme devices have a major number > 255, so take up more than 8 bits of dev_t
    // This test checks that the correct glibc routines are being used to handle this
    auto hasMajorMinor = majorMinorPredicate(259, 2);
    QList<Solid::Device> solidNvme0n1p2BlockDeviceList = Solid::Device::listFromQuery(hasMajorMinor);
    QCOMPARE(solidNvme0n1p2BlockDeviceList.count(), 1);

    auto solidNvme0n1p2BlockDevice = dynamic_cast<Solid::Block*>(
            solidNvme0n1p2BlockDeviceList.first().asDeviceInterface(Solid::DeviceInterface::Type::Block)
    );
    QVERIFY(solidNvme0n1p2BlockDevice);

    QCOMPARE(solidNvme0n1p2BlockDevice->deviceMajor(), 259);
    QCOMPARE(solidNvme0n1p2BlockDevice->deviceMinor(), 2);
}

QTEST_GUILESS_MAIN_SYSTEM_DBUS(SolidUdisks2Test)

#include "solidudisks2test.moc"
