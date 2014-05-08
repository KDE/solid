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

#include <QObject>
#include <QTest>
#include <QSignalSpy>

#include <Solid/Job>

using namespace Solid;

class MockSolidJob : public Solid::Job
{
    Q_OBJECT
public:
    enum Error {
        Foo = Solid::Job::UserDefinedError,
        Bar
    };
    bool execWithError();
    void startWithError();
public Q_SLOTS:
    virtual void doStart();
    void emitQueued() {
        emitResult();
    }
private:
    Q_DECLARE_PRIVATE(Job)
};

void MockSolidJob::doStart()
{
    QMetaObject::invokeMethod(this, "emitQueued", Qt::QueuedConnection);
}

bool MockSolidJob::execWithError()
{
    setError(Bar);
    setErrorText(QStringLiteral("Error Bar happened"));
    return exec();
}

void MockSolidJob::startWithError()
{
    setError(Foo);
    setErrorText(QStringLiteral("Error Foo happened"));
    start();
}

class testSolidJob : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAsyncAndResult();
    void testAsyncWithError();
    void testAutoDelete();
    void testSync();
    void testError();
};

void testSolidJob::testAsyncAndResult()
{
    MockSolidJob *job = new MockSolidJob();
    QSignalSpy spy(job, SIGNAL(result(Solid::Job*)));

    job->start();

    QVERIFY(spy.wait());
    //Result is emitted
    QCOMPARE(spy.count(), 1);
    //Result argument is the job that emitted it
    QCOMPARE(spy.takeFirst().first().value<MockSolidJob*>(), job);
}

void testSolidJob::testAsyncWithError()
{
    MockSolidJob *job = new MockSolidJob();
    QSignalSpy spy(job, SIGNAL(result(Solid::Job*)));

    job->startWithError();
    QVERIFY(spy.wait()); //Even on error, we get a result

    MockSolidJob *rJob = spy.takeFirst().first().value<MockSolidJob*>();
    QCOMPARE(rJob->error(), (int) MockSolidJob::Foo);
    QCOMPARE(rJob->errorText(), QStringLiteral("Error Foo happened"));
}

void testSolidJob::testAutoDelete()
{
    MockSolidJob *job = new MockSolidJob();
    QSignalSpy spy(job, SIGNAL(destroyed(QObject*)));
    QSignalSpy spyResult(job, SIGNAL(destroyed(QObject*)));

    job->start();
    QVERIFY(spy.wait());
    QVERIFY(!spyResult.isEmpty()); // Make sure result was emitted as well
}

void testSolidJob::testSync()
{
    MockSolidJob *job = new MockSolidJob();
    QSignalSpy spy(job, SIGNAL(destroyed(QObject*)));

    QVERIFY(job->exec());
    QVERIFY(spy.wait()); //Jobs started with exec should also autodelete
}

void testSolidJob::testError()
{
    MockSolidJob *job = new MockSolidJob();
    QVERIFY(!job->execWithError());

    QCOMPARE(job->error(), (int)MockSolidJob::Bar);
    QCOMPARE(job->errorText(), QStringLiteral("Error Bar happened"));
}

QTEST_MAIN(testSolidJob)

#include "solidjobtest.moc"