/*
    Copyright 2006 Kevin Ottens <ervin@kde.org>

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

#include "iokitopticaldisc.h"

#include <QDebug>

#include <CoreFoundation/CoreFoundation.h>

using namespace Solid::Backends::IOKit;

IOKitOpticalDisc::IOKitOpticalDisc(IOKitDevice *device)
    : IOKitVolume(device)
{
}

IOKitOpticalDisc::IOKitOpticalDisc(const IOKitDevice *device)
    : IOKitVolume(device)
{
}

IOKitOpticalDisc::~IOKitOpticalDisc()
{
}

QString IOKitOpticalDisc::device() const
{
    const QString devName = m_device->property(QLatin1String("BSD Name")).toString();
    if (devName.startsWith(QLatin1Char('r'))) {
        return QStringLiteral("/dev/") + devName;
    } else {
        return QStringLiteral("/dev/r") + devName;
    }
}

Solid::OpticalDisc::ContentTypes IOKitOpticalDisc::availableContent() const
{
    if (fsType() == QStringLiteral("cddafs")) {
        return Solid::OpticalDisc::Audio;
    }
    return Solid::OpticalDisc::Data;
}

Solid::OpticalDisc::DiscType IOKitOpticalDisc::discType() const
{
    QString type = m_device->property(QStringLiteral("Type")).toString();

    if (type == "CD-ROM") {
        return Solid::OpticalDisc::CdRom;
    } else if (type == "CD-R") {
        return Solid::OpticalDisc::CdRecordable;
    } else if (type == "CD-RW") {
        return Solid::OpticalDisc::CdRewritable;
    } else if (type == "DVD-ROM") {
        return Solid::OpticalDisc::DvdRom;
    } else if (type == "DVD-RAM") {
        return Solid::OpticalDisc::DvdRam;
    } else if (type == "DVD-R") {
        return Solid::OpticalDisc::DvdRecordable;
    } else if (type == "DVD-RW") {
        return Solid::OpticalDisc::DvdRewritable;
    } else if (type == "DVD+R") {
        return Solid::OpticalDisc::DvdPlusRecordable;
    } else if (type == "DVD+RW") {
        return Solid::OpticalDisc::DvdPlusRewritable;
    } else if (type == "BD-ROM") {
        return Solid::OpticalDisc::BluRayRom;
    } else if (type == "BD-R") {
        return Solid::OpticalDisc::BluRayRecordable;
    } else if (type == "BD-RE") {
        return Solid::OpticalDisc::BluRayRewritable;
    } else if (type == "HD DVD-ROM") {
        return Solid::OpticalDisc::HdDvdRom;
    } else if (type == "HD DVD-R") {
        return Solid::OpticalDisc::HdDvdRecordable;
    } else if (type == "HD DVD-RW") {
        return Solid::OpticalDisc::HdDvdRewritable;
    } else {
        return Solid::OpticalDisc::UnknownDiscType;
    }
}

bool IOKitOpticalDisc::isAppendable() const
{
    // TODO!
    return isRewritable();
}

bool IOKitOpticalDisc::isBlank() const
{
    // TODO!
    return isRewritable();
}

bool IOKitOpticalDisc::isRewritable() const
{
    return m_device->property(QStringLiteral("Writable")).toBool();
}

qulonglong IOKitOpticalDisc::capacity() const
{
    return size();
}
