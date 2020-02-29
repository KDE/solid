/*
    SPDX-FileCopyrightText: 2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "fakestorageaccess.h"

using namespace Solid::Backends::Fake;

FakeStorageAccess::FakeStorageAccess(FakeDevice *device)
    : FakeDeviceInterface(device)
{
    connect(device, SIGNAL(propertyChanged(QMap<QString,int>)),
            this, SLOT(onPropertyChanged(QMap<QString,int>)));
}

FakeStorageAccess::~FakeStorageAccess()
{

}

bool FakeStorageAccess::isAccessible() const
{
    return fakeDevice()->property("isMounted").toBool();
}

QString FakeStorageAccess::filePath() const
{
    return fakeDevice()->property("mountPoint").toString();
}

bool FakeStorageAccess::isIgnored() const
{
    return fakeDevice()->property("isIgnored").toBool();
}

bool FakeStorageAccess::setup()
{
    if (fakeDevice()->isBroken() || isAccessible()) {
        return false;
    } else {
        fakeDevice()->setProperty("isMounted", true);
        return true;
    }
}

bool FakeStorageAccess::teardown()
{
    if (fakeDevice()->isBroken() || !isAccessible()) {
        return false;
    } else {
        fakeDevice()->setProperty("isMounted", false);
        return true;
    }
}

void Solid::Backends::Fake::FakeStorageAccess::onPropertyChanged(const QMap<QString, int> &changes)
{
    Q_FOREACH (const QString &property, changes.keys()) {
        if (property == "isMounted") {
            emit accessibilityChanged(fakeDevice()->property("isMounted").toBool(), fakeDevice()->udi());
        }
    }
}

