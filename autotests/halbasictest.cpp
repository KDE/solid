/*
    SPDX-FileCopyrightText: 2005, 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halbasictest.h"

#include <QTest>

#include "solid/devices/backends/hal/halmanager.h"

#include <solid/device.h>
#include <solid/devices/ifaces/device.h>
#include <solid/devices/ifaces/deviceinterface.h>
#include <solid/devices/ifaces/processor.h>
#include "solid/devices/backends/hal/halprocessor.h"

#include <QDBusConnection>

QTEST_MAIN(HalBasicTest)

HalBasicTest::HalBasicTest(QObject *parent)
    : QObject(parent)
{
    qputenv("SOLID_HAL_LEGACY", "1");
}

void HalBasicTest::initTestCase()
{
    if (!QDBusConnection::systemBus().isConnected())
        QSKIP("D-Bus not running");
}

void HalBasicTest::testBasic()
{
    Solid::Backends::Hal::HalManager *manager = new Solid::Backends::Hal::HalManager(nullptr);

    QVERIFY(manager->deviceExists("/org/freedesktop/Hal/devices/computer"));
    QVERIFY(!manager->allDevices().isEmpty());

    QVERIFY(!manager->devicesFromQuery(QString(), Solid::DeviceInterface::Processor).isEmpty());

    QString proc_udi = manager->devicesFromQuery(QString(), Solid::DeviceInterface::Processor).at(0);

    Solid::Backends::Hal::HalDevice *processor = qobject_cast<Solid::Backends::Hal::HalDevice *>(manager->createDevice(proc_udi));

    QCOMPARE(processor->udi(), proc_udi);
    QCOMPARE(processor->parentUdi(), QString("/org/freedesktop/Hal/devices/computer"));
    QVERIFY(!processor->allProperties().isEmpty());
    QVERIFY(processor->propertyExists("info.product"));
    QVERIFY(!processor->propertyExists("the.meaning.of.life"));
    QVERIFY(processor->queryDeviceInterface(Solid::DeviceInterface::Processor));
    QVERIFY(!processor->queryDeviceInterface(Solid::DeviceInterface::OpticalDisc));

    QObject *interface = processor->createDeviceInterface(Solid::DeviceInterface::Processor);
    Solid::Ifaces::Processor *proc_iface = qobject_cast<Solid::Ifaces::Processor *>(interface);

    QVERIFY(proc_iface != nullptr);

#if 0
    // HAL locking support being broken anyway...
    QVERIFY(!processor->isLocked());
    QVERIFY(processor->lock("No reason..."));
    QVERIFY(processor->isLocked());
    QCOMPARE(processor->lockReason(), QString("No reason..."));
    QVERIFY(!processor->lock("Need a reason?"));
    QVERIFY(processor->unlock());
#endif

    QObject *object = processor;
    QCOMPARE(interface->parent(), object);

    delete processor;
    delete manager;
}

void HalBasicTest::testProcessorList()
{
    QList<Solid::Device> list = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString());
    qDebug() << "Number of processors:" << list.size();
    if (!list.isEmpty()) {
        Solid::Processor *p = list[0].as<Solid::Processor>();
        QVERIFY(p);
        Solid::Processor::InstructionSets features = p->instructionSets();
        qDebug() << "features:" << features;
    }
}

void HalBasicTest::testDeviceCreation()
{
    // Uncomment to check if the "still reachable" number grows in
    // valgrind, which probably indicates a memory leak.
    //for (int i=0; i<1000; i++)
    {
        Solid::Device dev("/org/freedesktop/Hal/devices/computer");
        QVERIFY(dev.isValid());
        dev = Solid::Device("ddd/ff");
        QVERIFY(!dev.isValid());
    }
}

void HalBasicTest::testSignalHandling()
{
    Solid::Backends::Hal::HalManager *manager = new Solid::Backends::Hal::HalManager(nullptr);
    m_device = qobject_cast<Solid::Backends::Hal::HalDevice *>(manager->createDevice("/org/freedesktop/Hal/devices/computer"));

#if 0
    connect(m_device, SIGNAL(propertyChanged(QMap<QString,int>)),
            this, SLOT(slotPropertyChanged(QMap<QString,int>)));

    // HAL locking support being broken anyway...
    QVERIFY(!m_device->isLocked());
    m_signalProcessed = false;
    m_device->lock("Still no reason... really");
    QVERIFY(m_device->isLocked());
    QVERIFY(m_signalProcessed);
#endif

    delete m_device;
    delete manager;
}

void HalBasicTest::slotPropertyChanged(const QMap<QString, int> &changes)
{
    Q_UNUSED(changes)
#if 0
    QVERIFY(m_device->isLocked());
    m_signalProcessed = true;
#endif
}

#include "moc_halbasictest.cpp"

