/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "camera.h"
#include "camera_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/camera.h>

Solid::Camera::Camera(QObject *backendObject)
    : DeviceInterface(*new CameraPrivate(), backendObject)
{
}

Solid::Camera::~Camera()
{
}

QStringList Solid::Camera::supportedProtocols() const
{
    Q_D(const Camera);
    auto *p = iface_cast<Ifaces::Camera>(d->backendObject());
    return p ? p->supportedProtocols() : QStringList();
}

QStringList Solid::Camera::supportedDrivers(QString protocol) const
{
    Q_D(const Camera);
    auto *p = iface_cast<Ifaces::Camera>(d->backendObject());
    return p ? p->supportedDrivers(protocol) : QStringList();
}

QVariant Solid::Camera::driverHandle(const QString &driver) const
{
    Q_D(const Camera);
    auto *p = iface_cast<Ifaces::Camera>(d->backendObject());
    return p ? p->driverHandle(driver) : QVariant();
}
