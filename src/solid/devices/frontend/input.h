/*
    SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_INPUT_H
#define SOLID_INPUT_H

#include <solid/solid_export.h>

#include <solid/deviceinterface.h>

#include <QStringList>
#include <QVariant>

namespace Solid
{
class InputPrivate;
class Device;

/**
 * This device interface is available on input devices devices.
 *
 * An input device will range across mice, touchpads, keyboards, game pads and
 * tv remote controls.
 *
 * @since 5.92
 */
class SOLID_EXPORT Input : public DeviceInterface
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Input)
    friend class Device;

private:
    /**
     * Creates a new Input object.
     * You generally won't need this. It's created when necessary using
     * Device::as().
     *
     * @param backendObject the device interface object provided by the backend
     * @see Solid::Device::as()
     */
    explicit Input(QObject *backendObject);

public:
    /**
     * Destroys a Input object.
     */
    ~Input() override;

    /**
     * Get the Solid::DeviceInterface::Type of the Input device interface.
     *
     * @return the Input device interface type
     * @see Solid::DeviceInterface::Type
     */
    static Type deviceInterfaceType()
    {
        return DeviceInterface::Input;
    }

    /**
     * Retrieves the absolute path of the special file to interact
     * with the device.
     *
     * @return the absolute path of the special file to interact with
     * the device
     */
    virtual QString device() const;
};
}

#endif
