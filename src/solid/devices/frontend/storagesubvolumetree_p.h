/*
    SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_STORAGESUBVOLUMETREE_P_H
#define SOLID_STORAGESUBVOLUMETREE_P_H

#include "deviceinterface_p.h"

namespace Solid
{
class StorageSubvolumeTreePrivate : public DeviceInterfacePrivate
{
public:
    StorageSubvolumeTreePrivate()
        : DeviceInterfacePrivate()
    {
    }
};
}

#endif
