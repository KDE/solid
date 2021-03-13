/*
    SPDX-FileCopyrightText: 2005 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLIDHWTEST_H
#define SOLIDHWTEST_H

#include <QMap>
#include <QObject>

namespace Solid
{
namespace Backends
{
namespace Fake
{
class FakeManager;
}
}
}

class SolidHwTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testAllDevices();
    void testDeviceBasicFeatures();
    void testManagerSignals();
    void testDeviceSignals();
    void testDeviceExistence();
    void testDeviceInterfaceIntrospection_data();
    void testDeviceInterfaceIntrospection();
    void testDeviceInterfaceIntrospectionCornerCases();
    void testDeviceInterfaces();
    void testInvalidPredicate();
    void testPredicate();
    void testQueryStorageVolumeOrProcessor();
    void testQueryStorageVolumeOrStorageAccess();
    void testQueryWithParentUdi();
    void testListFromTypeProcessor();
    void testListFromTypeInvalid();
    void testSetupTeardown();

    void slotPropertyChanged(const QMap<QString, int> &changes);

private:
    Solid::Backends::Fake::FakeManager *fakeManager;
    QList<QMap<QString, int>> m_changesList;
};

#endif
