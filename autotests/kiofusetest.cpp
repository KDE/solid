/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <QTest>

#include "kiofuseutils.h"

using namespace Solid::Backends::KioFuse;

class KioFuseTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testUdiRoundTrip_data();
    void testUdiRoundTrip();
    void testUdiHasNoSlashes();
};

void KioFuseTest::testUdiRoundTrip_data()
{
    QTest::addColumn<QString>("url");

    QTest::newRow("host only") << QStringLiteral("smb://fileserver");
    QTest::newRow("host and share") << QStringLiteral("smb://fileserver/documents");
    QTest::newRow("nested path") << QStringLiteral("smb://fileserver/documents/reports");
    QTest::newRow("user and port") << QStringLiteral("sftp://someone@example.org:2222/srv/data");
    QTest::newRow("trailing slash") << QStringLiteral("ftp://ftp.example.org/");
    QTest::newRow("space in path") << QStringLiteral("smb://fileserver/my share");
}

void KioFuseTest::testUdiRoundTrip()
{
    QFETCH(QString, url);

    const QUrl original(url);
    const QString udi = Utils::udiForUrl(original);

    QVERIFY(udi.startsWith(Utils::udiPrefix() + QLatin1Char('/')));
    QCOMPARE(Utils::urlForUdi(udi), original);
}

void KioFuseTest::testUdiHasNoSlashes()
{
    const QString udi = Utils::udiForUrl(QUrl(QStringLiteral("smb://fileserver/documents/reports/2026")));
    const QString encoded = udi.mid(Utils::udiPrefix().length() + 1);

    QVERIFY(!encoded.isEmpty());
    QCOMPARE(encoded.count(QLatin1Char('/')), 0);
}

QTEST_MAIN(KioFuseTest)

#include "kiofusetest.moc"
