/*
    Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>

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

#include "iokitstorageaccess.h"

#include <QDebug>

#include <CoreFoundation/CoreFoundation.h>
#include <DiskArbitration/DiskArbitration.h>

using namespace Solid::Backends::IOKit;

IOKitStorageAccess::IOKitStorageAccess(IOKitDevice *device)
    : DeviceInterface(device)
    , daDict(new DADictionary(device))
{
    connect(device, SIGNAL(propertyChanged(QMap<QString,int>)),
            this, SLOT(onPropertyChanged(QMap<QString,int>)));
}

IOKitStorageAccess::IOKitStorageAccess(const IOKitDevice *device)
    : DeviceInterface(device)
    , daDict(new DADictionary(device))
{
    connect(device, SIGNAL(propertyChanged(QMap<QString,int>)),
            this, SLOT(onPropertyChanged(QMap<QString,int>)));
}

IOKitStorageAccess::~IOKitStorageAccess()
{
    delete daDict;
}

bool IOKitStorageAccess::isAccessible() const
{
    filePath();
    const QVariant isMounted = m_device->property(QStringLiteral("isMounted"));
    return isMounted.isValid() && isMounted.toBool();
}

QString IOKitStorageAccess::filePath() const
{
    // mount points can change (but can they between invocations of filePath()?)
    QString mountPoint;
    if (const CFURLRef urlRef  = daDict->cfUrLRefForKey(kDADiskDescriptionVolumePathKey)) {
        const CFStringRef mpRef = CFURLCopyFileSystemPath(urlRef, kCFURLPOSIXPathStyle);
        mountPoint = QString::fromCFString(mpRef);
        CFRelease(mpRef);
        m_device->setProperty("mountPoint", QVariant(mountPoint));
        bool isMounted = !mountPoint.isEmpty();
        const QString isMountedKey = QStringLiteral("isMounted");
        const QVariant wasMounted = m_device->property(isMountedKey);
        if (wasMounted.isValid() && wasMounted.toBool() != isMounted) {
            IOKitStorageAccess(m_device).onPropertyChanged(QMap<QString,int>{{isMountedKey,isMounted}});
        }
        m_device->setProperty("isMounted", QVariant(isMounted));
    }
    return mountPoint;
}

bool IOKitStorageAccess::isIgnored() const
{
    if (m_device->iOKitPropertyExists(QStringLiteral("Open"))) {
        // ignore storage volumes that aren't mounted
        bool isIgnored = m_device->property(QStringLiteral("Open")).toBool() == false;
        m_device->setProperty("isIgnored", QVariant(isIgnored));
        return isIgnored;
    }
    const QVariant isIgnored = m_device->property(QStringLiteral("isIgnored"));
    return isIgnored.isValid() && isIgnored.toBool();
}

bool IOKitStorageAccess::setup()
{
    // TODO?
    return false;
}

bool IOKitStorageAccess::teardown()
{
    // TODO?
    return false;
}

void IOKitStorageAccess::onPropertyChanged(const QMap<QString, int> &changes)
{
    Q_FOREACH (const QString &property, changes.keys()) {
        if (property == QStringLiteral("isMounted")) {
            emit accessibilityChanged(m_device->property(QStringLiteral("isMounted")).toBool(), m_device->udi());
        }
    }
}

