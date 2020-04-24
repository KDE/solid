/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halopticaldisc.h"

using namespace Solid::Backends::Hal;

OpticalDisc::OpticalDisc(HalDevice *device)
    : Volume(device)
{

}

OpticalDisc::~OpticalDisc()
{

}

Solid::OpticalDisc::ContentTypes OpticalDisc::availableContent() const
{
    Solid::OpticalDisc::ContentTypes content;

    const QMap<Solid::OpticalDisc::ContentType, QString> map = {
        {Solid::OpticalDisc::Audio, QStringLiteral("volume.disc.has_audio")},
        {Solid::OpticalDisc::Data, QStringLiteral("volume.disc.has_data")},
        {Solid::OpticalDisc::VideoCd, QStringLiteral("volume.disc.is_vcd")},
        {Solid::OpticalDisc::SuperVideoCd, QStringLiteral("volume.disc.is_svcd")},
        {Solid::OpticalDisc::VideoDvd, QStringLiteral("volume.disc.is_videodvd")},
        {Solid::OpticalDisc::VideoBluRay, QStringLiteral("volume.disc.is_blurayvideo")}};

    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        if (m_device->prop(it.value()).toBool()) {
            content |= it.key();
        }
    }

    return content;
}

Solid::OpticalDisc::DiscType OpticalDisc::discType() const
{
    QString type = m_device->prop("volume.disc.type").toString();

    if (type == "cd_rom") {
        return Solid::OpticalDisc::CdRom;
    } else if (type == "cd_r") {
        return Solid::OpticalDisc::CdRecordable;
    } else if (type == "cd_rw") {
        return Solid::OpticalDisc::CdRewritable;
    } else if (type == "dvd_rom") {
        return Solid::OpticalDisc::DvdRom;
    } else if (type == "dvd_ram") {
        return Solid::OpticalDisc::DvdRam;
    } else if (type == "dvd_r") {
        return Solid::OpticalDisc::DvdRecordable;
    } else if (type == "dvd_rw") {
        return Solid::OpticalDisc::DvdRewritable;
    } else if (type == "dvd_plus_r") {
        return Solid::OpticalDisc::DvdPlusRecordable;
    } else if (type == "dvd_plus_rw") {
        return Solid::OpticalDisc::DvdPlusRewritable;
    } else if (type == "dvd_plus_r_dl") {
        return Solid::OpticalDisc::DvdPlusRecordableDuallayer;
    } else if (type == "dvd_plus_rw_dl") {
        return Solid::OpticalDisc::DvdPlusRewritableDuallayer;
    } else if (type == "bd_rom") {
        return Solid::OpticalDisc::BluRayRom;
    } else if (type == "bd_r") {
        return Solid::OpticalDisc::BluRayRecordable;
    } else if (type == "bd_re") {
        return Solid::OpticalDisc::BluRayRewritable;
    } else if (type == "hddvd_rom") {
        return Solid::OpticalDisc::HdDvdRom;
    } else if (type == "hddvd_r") {
        return Solid::OpticalDisc::HdDvdRecordable;
    } else if (type == "hddvd_rw") {
        return Solid::OpticalDisc::HdDvdRewritable;
    } else {
        return Solid::OpticalDisc::UnknownDiscType;
    }
}

bool OpticalDisc::isAppendable() const
{
    return m_device->prop("volume.disc.is_appendable").toBool();
}

bool OpticalDisc::isBlank() const
{
    return m_device->prop("volume.disc.is_blank").toBool();
}

bool OpticalDisc::isRewritable() const
{
    return m_device->prop("volume.disc.is_rewritable").toBool();
}

qulonglong OpticalDisc::capacity() const
{
    return m_device->prop("volume.disc.capacity").toULongLong();
}

