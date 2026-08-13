/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_KIOFUSE_NETWORKSHARE_H
#define SOLID_BACKENDS_KIOFUSE_NETWORKSHARE_H

#include <solid/devices/ifaces/networkshare.h>

#include <QObject>

namespace Solid
{
namespace Backends
{
namespace KioFuse
{
class Device;

class NetworkShare : public QObject, public Solid::Ifaces::NetworkShare
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::NetworkShare)

public:
    explicit NetworkShare(Device *device);
    ~NetworkShare() override;

    Solid::NetworkShare::ShareType type() const override;
    QUrl url() const override;

private:
    Device *m_device;
};

}
}
}

#endif // SOLID_BACKENDS_KIOFUSE_NETWORKSHARE_H
