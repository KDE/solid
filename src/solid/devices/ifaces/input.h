/*
    SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_IFACES_INPUT_H
#define SOLID_IFACES_INPUT_H

#include <solid/devices/ifaces/deviceinterface.h>

#include <QStringList>

namespace Solid
{
namespace Ifaces
{
/**
 * This device interface is available on input devices devices.
 *
 * An input device will range across mice, touchpads, keyboards, game pads and
 * tv remote controls.
 *
 * @since 5.92
 */
class Input : virtual public DeviceInterface
{
public:
    /**
     * Destroys a Input object.
     */
    ~Input() override;

    /**
     * Retrieves the absolute path of the special file to interact
     * with the device.
     *
     * @return the absolute path of the special file to interact with
     * the device
     */
    virtual QString device() const = 0;
};
}
}

Q_DECLARE_INTERFACE(Solid::Ifaces::Input, "org.kde.Solid.Ifaces.Input/0.1")

#endif
