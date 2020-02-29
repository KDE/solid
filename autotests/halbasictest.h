/*
    SPDX-FileCopyrightText: 2005 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef HALBASICTEST_H
#define HALBASICTEST_H

#include <QObject>
#include <QMap>
#include <QString>

#include "solid/devices/backends/hal/haldevice.h"

class HalBasicTest : public QObject
{
    Q_OBJECT
public:
    HalBasicTest(QObject *parent = nullptr);
private Q_SLOTS:
    void initTestCase();
    void testBasic();
    void testProcessorList();
    void testDeviceCreation();
    void testSignalHandling();

    void slotPropertyChanged(const QMap<QString, int> &changes);

private:
    Solid::Backends::Hal::HalDevice *m_device;
};

#endif
