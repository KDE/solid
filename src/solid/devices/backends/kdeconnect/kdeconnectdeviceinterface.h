/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_KDECONNECT_DEVICEINTERFACE_H
#define SOLID_BACKENDS_KDECONNECT_DEVICEINTERFACE_H

#include "kdeconnectdevice.h"
#include <ifaces/deviceinterface.h>

#include <QObject>
#include <QPointer>
#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace KdeConnect
{

class Device;

class DeviceInterface : public QObject, virtual public Solid::Ifaces::DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::DeviceInterface)
public:
    DeviceInterface(Device *device);
    ~DeviceInterface() override;

protected:
    QPointer<Device> m_device;
};
}
}
}

#endif // SOLID_BACKENDS_KDECONNECT_DEVICEINTERFACE_H
