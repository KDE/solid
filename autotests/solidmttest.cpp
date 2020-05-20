/*
    SPDX-FileCopyrightText: 2008 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <QObject>
#include <QThread>
#include <QThreadPool>
#include <qtconcurrentrun.h>

#include <QTest>

#include <solid/device.h>
#include <solid/predicate.h>
#include <solid/storagevolume.h>
#include <solid/storagedrive.h>
#include <solid/genericinterface.h>

class SolidMtTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testWorkerThread();
    void testThreadedPredicate();
};

class WorkerThread : public QThread
{
    Q_OBJECT
protected:
    void run() override
    {
        Solid::Device dev("/org/freedesktop/Hal/devices/computer");

        const QList<Solid::Device> driveList = Solid::Device::listFromType(Solid::DeviceInterface::StorageDrive);
        for (const Solid::Device &solidDevice : driveList) {
            const Solid::StorageDrive *solidDrive = solidDevice.as<Solid::StorageDrive>();
            Q_ASSERT(solidDrive);
            Q_UNUSED(solidDrive);
        }
    }
};

static void doPredicates()
{
    Solid::Predicate p5 = Solid::Predicate::fromString("[[Processor.maxSpeed == 3201 AND Processor.canChangeFrequency == false] OR StorageVolume.mountPoint == '/media/blup']");

    Solid::Predicate p6 = Solid::Predicate::fromString("StorageVolume.usage == 'Other'");
    Solid::Predicate p7 = Solid::Predicate::fromString(QString("StorageVolume.usage == %1").arg((int)Solid::StorageVolume::Other));
}

QTEST_MAIN(SolidMtTest)

void SolidMtTest::testWorkerThread()
{
    Solid::Device dev("/org/freedesktop/Hal/devices/acpi_ADP1");

    WorkerThread *wt = new WorkerThread;
    wt->start();
    wt->wait();

    const QList<Solid::Device> driveList = Solid::Device::listFromType(Solid::DeviceInterface::StorageDrive);
    for (const Solid::Device &solidDevice : driveList) {
        const Solid::GenericInterface *solidDrive = solidDevice.as<Solid::GenericInterface>();
        Q_ASSERT(solidDrive);
        Q_UNUSED(solidDrive);
    }

    delete wt;
}

void SolidMtTest::testThreadedPredicate()
{
    QThreadPool::globalInstance()->setMaxThreadCount(10);
    const QList<QFuture<void> > futures = { QtConcurrent::run(&doPredicates), QtConcurrent::run(&doPredicates),
                                            QtConcurrent::run(&doPredicates), QtConcurrent::run(&doPredicates),
                                            QtConcurrent::run(&doPredicates), QtConcurrent::run(&doPredicates),
                                            QtConcurrent::run(&doPredicates), QtConcurrent::run(&doPredicates) };
    for (QFuture<void> f : futures) {
        f.waitForFinished();
    }
    QThreadPool::globalInstance()->setMaxThreadCount(1); // delete those threads
}

#include "solidmttest.moc"

