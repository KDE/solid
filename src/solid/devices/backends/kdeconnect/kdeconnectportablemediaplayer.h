/*
    Copyright 2015 Kai Uwe Broulik <kde@privat.broulik.de>

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

#ifndef KDECONNECTPORTABLEMEDIAPLAYER_H
#define KDECONNECTPORTABLEMEDIAPLAYER_H

#include <solid/devices/ifaces/portablemediaplayer.h>
#include "kdeconnectdeviceinterface.h"

#include <QtCore/QStringList>

namespace Solid {
namespace Backends {
namespace KdeConnect {

class KdeConnectDevice;

class PortableMediaPlayer : public DeviceInterface, virtual public Solid::Ifaces::PortableMediaPlayer
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::PortableMediaPlayer)

public:
    PortableMediaPlayer(KdeConnectDevice *device);
    virtual ~PortableMediaPlayer();

    virtual QStringList supportedProtocols() const Q_DECL_OVERRIDE;
    virtual QStringList supportedDrivers(QString protocol = QString()) const Q_DECL_OVERRIDE;
    virtual QVariant driverHandle(const QString &driver) const Q_DECL_OVERRIDE;

private:
    QString m_id;

};

} // namespace KdeConnect
} // namespace Backends
} // namespace Solid

#endif // KDECONNECTPORTABLEMEDIAPLAYER_H
