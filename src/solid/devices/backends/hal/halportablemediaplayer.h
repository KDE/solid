/*
    SPDX-FileCopyrightText: 2006 Davide Bettio <davide.bettio@kdemail.net>
    SPDX-FileCopyrightText: 2007 Jeff Mitchell <kde-dev@emailgoeshere.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_PORTABLEMEDIAPLAYER_H
#define SOLID_BACKENDS_HAL_PORTABLEMEDIAPLAYER_H

#include <solid/devices/ifaces/portablemediaplayer.h>
#include "haldeviceinterface.h"

#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace Hal
{
class HalDevice;

class PortableMediaPlayer : public DeviceInterface, virtual public Solid::Ifaces::PortableMediaPlayer
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::PortableMediaPlayer)

public:
    PortableMediaPlayer(HalDevice *device);
    virtual ~PortableMediaPlayer();

    QStringList supportedProtocols() const override;
    QStringList supportedDrivers(QString protocol = QString()) const override;
    QVariant driverHandle(const QString &driver) const override;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_PORTABLEMEDIAPLAYER_H
