/*
    Copyright 2014 Alejandro Fiestas Olivares <afiestas@kde.org>

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
#include "fakeUpower.h"

#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <QDBusConnection>
#include <Solid/Power>
#include <Solid/AcPluggedJob>
#include <Solid/Inhibition>
#include <Solid/InhibitionJob>
#include <Solid/StatesJob>
#include <Solid/RequestStateJob>

using namespace Solid;
class solidFreedesktopTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testAcPluggedJob();
    void testAcPluggedChanged();
    void testAddInhibition();
    void testSupportedStates();
    void testRequestState();

private:
    FakeUpower *m_fakeUPower;
};

void solidFreedesktopTest::initTestCase()
{
    qputenv("SOLID_POWER_BACKEND", "FREE_DESKTOP");

    m_fakeUPower = new FakeUpower(this);
    QDBusConnection::systemBus().registerService(QStringLiteral("org.freedesktop.UPower"));
    QDBusConnection::systemBus().registerObject(QStringLiteral("/org/freedesktop/UPower"), m_fakeUPower, QDBusConnection::ExportAllContents);
}

void solidFreedesktopTest::testAcPluggedJob()
{
    m_fakeUPower->m_onBattery = true;
    auto job = new AcPluggedJob(this);
    QVERIFY(job->exec());
    QCOMPARE(job->isPlugged(), false);

    m_fakeUPower->m_onBattery = false;
    job = Solid::Power::isAcPlugged();
    QVERIFY(job->exec());
    QCOMPARE(job->isPlugged(), true);
}

void solidFreedesktopTest::testAcPluggedChanged()
{
    auto power = Solid::Power::self();
    QSignalSpy spy(power, SIGNAL(acPluggedChanged(bool)));

    m_fakeUPower->setOnBattery(true);
    m_fakeUPower->setOnBattery(false);
    spy.wait(10000);

    QCOMPARE(spy.count(), 2);

    QCOMPARE(spy.first().first().toBool(), false);
    QCOMPARE(spy.at(1).first().toBool(), true);
}

void solidFreedesktopTest::testAddInhibition()
{

}

void solidFreedesktopTest::testSupportedStates()
{

}

void solidFreedesktopTest::testRequestState()
{

}

QTEST_GUILESS_MAIN_SYSTEM_DBUS(solidFreedesktopTest)

#include "solidfreedesktoptest.moc"