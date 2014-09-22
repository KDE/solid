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

#include <QTest>
#include <QSignalSpy>
#include <Solid/Power>
#include <Solid/AcPluggedJob>
#include <Solid/Inhibition>
#include <Solid/InhibitionJob>
#include <Solid/StatesJob>
#include <Solid/RequestStateJob>

using namespace Solid;
class solidPowerTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testAcPluggedJob();
    void testAcPluggedChanged();
    void testAddInhibition();
    void testSupportedStates();
    void testRequestState();
};

void solidPowerTest::testAcPluggedJob()
{
    AcPluggedJob *job = new AcPluggedJob();
    QVERIFY(job->exec());
    QVERIFY(job->isPlugged());

    job = Power::isAcPlugged();
    QVERIFY(job->exec());
    QVERIFY(job->isPlugged());
}

void solidPowerTest::testAcPluggedChanged()
{
    Power *power = Power::self();
    QSignalSpy spy(power, SIGNAL(acPluggedChanged(bool)));

    QVERIFY(spy.wait());
    QVERIFY(spy.takeFirst().first().toBool());
}

void solidPowerTest::testAddInhibition()
{
    InhibitionJob *job = new InhibitionJob();
    QVERIFY(!job->exec());

    QCOMPARE(job->error(), (int) InhibitionJob::InvalidInhibitions);
    delete job;

    job = new InhibitionJob();
    job->setInhibitions(Power::Sleep);
    QVERIFY(!job->exec());
    QCOMPARE(job->error(), (int) InhibitionJob::EmptyDescription);
    delete job;

    job = Power::inhibit(Power::Sleep, QLatin1Literal("Running a test, we don't want to suspend now"));
    QVERIFY(job->exec());

    auto inhibition = job->inhibition();
    QSignalSpy spy(inhibition, SIGNAL(stateChanged(Inhibition::State)));
    inhibition->deleteLater();

    QVERIFY(spy.wait());
    QCOMPARE(spy.takeFirst().first().toInt(), (int) Inhibition::Stopped);
}

void solidPowerTest::testSupportedStates()
{
    auto job = new StatesJob();
    QVERIFY(job->exec());

    QCOMPARE(job->states(), Power::Brightness | Power::Sleep);

    job = Power::supportedStates();
    QVERIFY(job->exec());

    QCOMPARE(job->states(), Power::Brightness | Power::Sleep);
}

void solidPowerTest::testRequestState()
{
    auto job = new RequestStateJob();
    job->setState(Power::Sleep);
    QVERIFY(job->exec());

    job = Power::requestState(Power::Sleep);
    QVERIFY(job->exec());

    job = Power::requestState(Power::Brightness);
    QVERIFY(!job->exec());

    QCOMPARE(job->error(), (int) RequestStateJob::Unsupported);
    QCOMPARE(job->errorText(), QLatin1Literal(QLatin1Literal("State Brightness is unsupported")));
}

QTEST_MAIN(solidPowerTest)

#include "solidpowertest.moc"