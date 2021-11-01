/*
    SPDX-FileCopyrightText: 2010 Michael Zanetti <mzanetti@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef UPOWERDEVICEINTERFACE_H
#define UPOWERDEVICEINTERFACE_H

#include "upowerdevice.h"
#include <ifaces/deviceinterface.h>

#include <QObject>
#include <QPointer>
#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace UPower
{
class DeviceInterface : public QObject, virtual public Solid::Ifaces::DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::DeviceInterface)
public:
    DeviceInterface(UPowerDevice *device);
    ~DeviceInterface() override;

protected:
    QPointer<UPowerDevice> m_device;

public:
    inline static QStringList toStringList(Solid::DeviceInterface::Type type)
    {
        QStringList list;

        switch (type) {
        case Solid::DeviceInterface::GenericInterface:
            list << "generic";
            break;
        case Solid::DeviceInterface::Processor:
            // Doesn't exist with UDisks
            break;
        case Solid::DeviceInterface::Block:
            list << "block";
            break;
        case Solid::DeviceInterface::StorageAccess:
            list << "volume";
            break;
        case Solid::DeviceInterface::StorageDrive:
            list << "storage";
            break;
        case Solid::DeviceInterface::OpticalDrive:
            list << "storage.cdrom";
            break;
        case Solid::DeviceInterface::StorageVolume:
            list << "volume";
            break;
        case Solid::DeviceInterface::OpticalDisc:
            list << "volume.disc";
            break;
        case Solid::DeviceInterface::Camera:
            // Doesn't exist with UDisks
            break;
        case Solid::DeviceInterface::PortableMediaPlayer:
            // Doesn't exist with UDisks
            break;
        case Solid::DeviceInterface::Battery:
            list << "battery";
            break;
        case Solid::DeviceInterface::NetworkShare:
            // Doesn't exist with UPower
            break;
        case Solid::DeviceInterface::Unknown:
            break;
        case Solid::DeviceInterface::Last:
            break;
        }

        return list;
    }

    inline static Solid::DeviceInterface::Type fromString(const QString &capability)
    {
        if (capability == "generic") {
            return Solid::DeviceInterface::GenericInterface;
        } else if (capability == "processor") {
            return Solid::DeviceInterface::Processor;
        } else if (capability == "block") {
            return Solid::DeviceInterface::Block;
        } else if (capability == "storage") {
            return Solid::DeviceInterface::StorageDrive;
        } else if (capability == "storage.cdrom") {
            return Solid::DeviceInterface::OpticalDrive;
        } else if (capability == "volume") {
            return Solid::DeviceInterface::StorageVolume;
        } else if (capability == "volume.disc") {
            return Solid::DeviceInterface::OpticalDisc;
        } else if (capability == "camera") {
            return Solid::DeviceInterface::Camera;
        } else if (capability == "portable_audio_player") {
            return Solid::DeviceInterface::PortableMediaPlayer;
        } else if (capability == "battery") {
            return Solid::DeviceInterface::Battery;
        } else if (capability == "networkshare") {
            return Solid::DeviceInterface::NetworkShare;
        } else {
            return Solid::DeviceInterface::Unknown;
        }
    }
};

}
}
}

#endif // UPOWERDEVICEINTERFACE_H
