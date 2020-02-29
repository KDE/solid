/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halcamera.h"

using namespace Solid::Backends::Hal;

Camera::Camera(HalDevice *device)
    : DeviceInterface(device)
{

}

Camera::~Camera()
{

}

QStringList Camera::supportedProtocols() const
{
    QStringList protocols;

    QString method = m_device->prop("camera.access_method").toString();

    protocols << method;

    return protocols;
}

QStringList Camera::supportedDrivers(QString /*protocol*/) const
{
    QStringList res;

    if (m_device->prop("camera.libgphoto2.support").toBool()) {
        res << "gphoto";
    }

    return res;
}

QVariant Solid::Backends::Hal::Camera::driverHandle(const QString &driver) const
{
    if (driver == "gphoto"
            && m_device->prop("info.subsystem").toString() == "usb") {
        QVariantList list;

        list << "usb"
             << m_device->prop("usb.vendor_id")
             << m_device->prop("usb.product_id");

        return list;
    }

    return QVariant();
}

