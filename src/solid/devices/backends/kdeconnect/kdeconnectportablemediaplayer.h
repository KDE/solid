/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_KDECONNECT_PORTABLEMEDIAPLAYER_H
#define SOLID_BACKENDS_KDECONNECT_PORTABLEMEDIAPLAYER_H

#include "kdeconnectdeviceinterface.h"
#include <solid/devices/ifaces/portablemediaplayer.h>

#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace KdeConnect
{
class Device;

class PortableMediaPlayer : public DeviceInterface, virtual public Solid::Ifaces::PortableMediaPlayer
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::PortableMediaPlayer)

public:
    PortableMediaPlayer(Device *device);
    ~PortableMediaPlayer() override;

    QStringList supportedProtocols() const override;
    QStringList supportedDrivers(QString protocol = QString()) const override;
    QVariant driverHandle(const QString &driver) const override;
};

}
}
}

#endif // SOLID_BACKENDS_KDECONNECT_PORTABLEMEDIAPLAYER_H
