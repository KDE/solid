/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kiofusenetworkshare.h"
#include "kiofusedevice.h"

using namespace Solid::Backends::KioFuse;

NetworkShare::NetworkShare(Device *device)
    : QObject(device)
    , m_device(device)
{
}

NetworkShare::~NetworkShare() = default;

Solid::NetworkShare::ShareType NetworkShare::type() const
{
    const QString scheme = m_device->url().scheme();

    if (scheme == QLatin1String("smb")) {
        return Solid::NetworkShare::Cifs;
    }
    if (scheme == QLatin1String("nfs")) {
        return Solid::NetworkShare::Nfs;
    }

    return Solid::NetworkShare::Unknown;
}

QUrl NetworkShare::url() const
{
    return m_device->url();
}

#include "moc_kiofusenetworkshare.cpp"
