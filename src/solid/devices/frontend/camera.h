/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_CAMERA_H
#define SOLID_CAMERA_H

#include <solid/solid_export.h>

#include <solid/deviceinterface.h>

#include <QStringList>
#include <QVariant>

namespace Solid
{
class CameraPrivate;
class Device;

/*!
 * \class Solid::Camera
 * \inheaderfile Solid/Camera
 * \inmodule Solid
 *
 * \brief This device interface is available on digital camera devices.
 *
 * A digital camera is a device used to transform images into
 * data. Nowadays most digital cameras are multifunctional and
 * able to take photographs, video or sound. On the system side
 * they are a particular type of device holding data, the access
 * method can be different from the typical storage device, hence
 * why it's a separate device interface.
 */
class SOLID_EXPORT Camera : public DeviceInterface
{
    Q_OBJECT

    /*!
     * \property Solid::Camera::supportedProtocols
     */
    Q_PROPERTY(QStringList supportedProtocols READ supportedProtocols)

    /*!
     * \property Solid::Camera::supportedDrivers
     */
    Q_PROPERTY(QStringList supportedDrivers READ supportedDrivers)

    Q_DECLARE_PRIVATE(Camera)
    friend class Device;

private:
    /*!
     * \internal
     * Creates a new Camera object.
     * You generally won't need this. It's created when necessary using
     * Device::as().
     *
     * \a backendObject the device interface object provided by the backend
     * \sa Solid::Device::as()
     */
    SOLID_NO_EXPORT explicit Camera(QObject *backendObject);

public:
    ~Camera() override;

    /*!
     * Returns the Solid::DeviceInterface::Type of the Camera device interface.
     * \sa Solid::DeviceInterface::Type
     */
    static Type deviceInterfaceType()
    {
        return DeviceInterface::Camera;
    }

    /*!
     * Returns known protocols this device supports.
     *
     * This list may be dependent
     * on installed device driver libraries.
     */
    QStringList supportedProtocols() const;

    /*!
     * Returns known installed device drivers that claim to handle this device
     * using the requested \a protocol.
     *
     * If \a protocol is omitted, returns a list of
     * all drivers supporting the device.
     */
    QStringList supportedDrivers(QString protocol = QString()) const;

    /*!
     * Returns a \a driver specific string allowing to access the device.
     *
     * For example for the "gphoto" driver it will return a list of the
     * form '("usb", vendor_id, product_id)'.
     */
    QVariant driverHandle(const QString &driver) const;
};
}

#endif
