/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_DEVICEINTERFACE_H
#define SOLID_BACKENDS_HAL_DEVICEINTERFACE_H

#include <solid/devices/ifaces/deviceinterface.h>
#include "haldevice.h"

#include <QObject>
#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace Hal
{
class DeviceInterface : public QObject, virtual public Solid::Ifaces::DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::DeviceInterface)
public:
    DeviceInterface(HalDevice *device);
    virtual ~DeviceInterface();

protected:
    HalDevice *m_device;

public:
    inline static QStringList toStringList(Solid::DeviceInterface::Type type)
    {
        QStringList list;

        switch (type) {
        case Solid::DeviceInterface::GenericInterface:
            // Doesn't exist with HAL
            break;
        case Solid::DeviceInterface::Processor:
            list << "processor";
            break;
        case Solid::DeviceInterface::Block:
            list << "block";
            break;
        case Solid::DeviceInterface::StorageAccess:
            // Doesn't exist with HAL, but let's assume volume always cover this type
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
            list << "camera";
            break;
        case Solid::DeviceInterface::PortableMediaPlayer:
            list << "portable_audio_player";
            break;
        case Solid::DeviceInterface::Battery:
            list << "battery";
            break;
        case Solid::DeviceInterface::NetworkShare:
            list << "networkshare";
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
        if (capability == "processor") {
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

#endif // SOLID_BACKENDS_HAL_DEVICEINTERFACE_H
