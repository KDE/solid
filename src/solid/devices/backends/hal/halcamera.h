/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_CAMERA_H
#define SOLID_BACKENDS_HAL_CAMERA_H

#include <solid/devices/ifaces/camera.h>
#include "haldeviceinterface.h"

namespace Solid
{
namespace Backends
{
namespace Hal
{
class Camera : public DeviceInterface, virtual public Solid::Ifaces::Camera
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Camera)

public:
    Camera(HalDevice *device);
    virtual ~Camera();

    QStringList supportedProtocols() const override;
    QStringList supportedDrivers(QString protocol = QString()) const override;
    QVariant driverHandle(const QString &driver) const override;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_CAMERA_H
