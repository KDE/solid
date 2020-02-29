/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_OPTICALDISC_H
#define SOLID_BACKENDS_HAL_OPTICALDISC_H

#include <solid/devices/ifaces/opticaldisc.h>
#include "halvolume.h"

namespace Solid
{
namespace Backends
{
namespace Hal
{
class OpticalDisc : public Volume, virtual public Solid::Ifaces::OpticalDisc
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::OpticalDisc)

public:
    OpticalDisc(HalDevice *device);
    virtual ~OpticalDisc();

    Solid::OpticalDisc::ContentTypes availableContent() const override;
    Solid::OpticalDisc::DiscType discType() const override;
    bool isAppendable() const override;
    bool isBlank() const override;
    bool isRewritable() const override;
    qulonglong capacity() const override;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_OPTICALDISC_H
