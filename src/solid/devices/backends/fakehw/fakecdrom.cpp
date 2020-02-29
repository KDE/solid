/*
    SPDX-FileCopyrightText: 2006 Davide Bettio <davide.bettio@kdemail.net>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "fakecdrom.h"

#include <QStringList>

using namespace Solid::Backends::Fake;

FakeCdrom::FakeCdrom(FakeDevice *device)
    : FakeStorage(device)
{

}

FakeCdrom::~FakeCdrom()
{

}

Solid::OpticalDrive::MediumTypes FakeCdrom::supportedMedia() const
{
    Solid::OpticalDrive::MediumTypes supported;

    QMap<Solid::OpticalDrive::MediumType, QString> map;
    map[Solid::OpticalDrive::Cdr] = "cdr";
    map[Solid::OpticalDrive::Cdrw] = "cdrw";
    map[Solid::OpticalDrive::Dvd] = "dvd";
    map[Solid::OpticalDrive::Dvdr] = "dvdr";
    map[Solid::OpticalDrive::Dvdrw] = "dvdrw";
    map[Solid::OpticalDrive::Dvdram] = "dvdram";
    map[Solid::OpticalDrive::Dvdplusr] = "dvdplusr";
    map[Solid::OpticalDrive::Dvdplusrw] = "dvdplusrw";
    map[Solid::OpticalDrive::Dvdplusdl] = "dvdplusrdl";
    map[Solid::OpticalDrive::Dvdplusdlrw] = "dvdplusrwdl";
    map[Solid::OpticalDrive::Bd] = "bd";
    map[Solid::OpticalDrive::Bdr] = "bdr";
    map[Solid::OpticalDrive::Bdre] = "bdre";
    map[Solid::OpticalDrive::HdDvd] = "hddvd";
    map[Solid::OpticalDrive::HdDvdr] = "hddvdr";
    map[Solid::OpticalDrive::HdDvdrw] = "hddvdrw";

    QStringList supported_medialist = fakeDevice()->property("supportedMedia").toString().simplified().split(',');

    Q_FOREACH (const Solid::OpticalDrive::MediumType type, map.keys()) {
        if (supported_medialist.indexOf(map[type]) != -1) {
            supported |= type;
        }
    }

    return supported;
}

int FakeCdrom::readSpeed() const
{
    return fakeDevice()->property("readSpeed").toInt();
}

int FakeCdrom::writeSpeed() const
{
    return fakeDevice()->property("writeSpeed").toInt();
}

QList<int> FakeCdrom::writeSpeeds() const
{
    QList<int> speeds;
    QStringList speed_strlist = fakeDevice()->property("writeSpeeds").toString().simplified().split(',');

    Q_FOREACH (const QString &speed_str, speed_strlist) {
        speeds << speed_str.toInt();
    }

    return speeds;
}

bool FakeCdrom::eject()
{
    return false;
}

