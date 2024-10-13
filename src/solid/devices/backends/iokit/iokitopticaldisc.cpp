/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "iokitopticaldisc.h"

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

    if (type == QLatin1String("CD-ROM")) {
        return Solid::OpticalDisc::CdRom;
    } else if (type == QLatin1String("CD-R")) {
        return Solid::OpticalDisc::CdRecordable;
    } else if (type == QLatin1String("CD-RW")) {
        return Solid::OpticalDisc::CdRewritable;
    } else if (type == QLatin1String("DVD-ROM")) {
        return Solid::OpticalDisc::DvdRom;
    } else if (type == QLatin1String("DVD-RAM")) {
        return Solid::OpticalDisc::DvdRam;
    } else if (type == QLatin1String("DVD-R")) {
        return Solid::OpticalDisc::DvdRecordable;
    } else if (type == QLatin1String("DVD-RW")) {
        return Solid::OpticalDisc::DvdRewritable;
    } else if (type == QLatin1String("DVD+R")) {
        return Solid::OpticalDisc::DvdPlusRecordable;
    } else if (type == QLatin1String("DVD+RW")) {
        return Solid::OpticalDisc::DvdPlusRewritable;
    } else if (type == QLatin1String("BD-ROM")) {
        return Solid::OpticalDisc::BluRayRom;
    } else if (type == QLatin1String("BD-R")) {
        return Solid::OpticalDisc::BluRayRecordable;
    } else if (type == QLatin1String("BD-RE")) {
        return Solid::OpticalDisc::BluRayRewritable;
    } else if (type == QLatin1String("HD DVD-ROM")) {
        return Solid::OpticalDisc::HdDvdRom;
    } else if (type == QLatin1String("HD DVD-R")) {
        return Solid::OpticalDisc::HdDvdRecordable;
    } else if (type == QLatin1String("HD DVD-RW")) {
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

#include "moc_iokitopticaldisc.cpp"
