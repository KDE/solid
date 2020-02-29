/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halgenericinterface.h"

#include "haldevice.h"

using namespace Solid::Backends::Hal;

GenericInterface::GenericInterface(HalDevice *device)
    : DeviceInterface(device)
{
    connect(device, SIGNAL(propertyChanged(QMap<QString,int>)),
            this, SIGNAL(propertyChanged(QMap<QString,int>)));
    connect(device, SIGNAL(conditionRaised(QString,QString)),
            this, SIGNAL(conditionRaised(QString,QString)));
}

GenericInterface::~GenericInterface()
{

}

QVariant GenericInterface::property(const QString &key) const
{
    return m_device->prop(key);
}

QMap<QString, QVariant> GenericInterface::allProperties() const
{
    return m_device->allProperties();
}

bool GenericInterface::propertyExists(const QString &key) const
{
    return m_device->propertyExists(key);
}

