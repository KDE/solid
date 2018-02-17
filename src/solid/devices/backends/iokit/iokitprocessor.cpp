/*
    Copyright 2009 Harald Fernengel <harry@kdevelop.org>
    Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "iokitprocessor.h"
#include "iokitdevice.h"

#include <QtCore/qdebug.h>

#include <sys/types.h>
#include <sys/sysctl.h>
#include <errno.h>

#include "../shared/cpufeatures.h"

// from cfhelper.cpp
extern bool q_sysctlbyname(const char *name, QString &result);

using namespace Solid::Backends::IOKit;

Processor::Processor(IOKitDevice *device)
    : DeviceInterface(device)
{
    //IOKitDevice parent(device->parentUdi());
}

Processor::~Processor()
{

}

int Processor::number() const
{
    return m_device->property(QLatin1String("IOCPUNumber")).toInt();
}

int Processor::maxSpeed() const
{
    uint64_t freq = 0;
    size_t size = sizeof(freq);

    if (sysctlbyname("hw.cpufrequency", &freq, &size, nullptr, 0) < 0) {
        qWarning() << "sysctl error reading hw.cpufrequency:" << strerror(errno);
        return 0;
    } else {
        return int(freq / 1000000);
    }
}

bool Processor::canChangeFrequency() const
{
    uint64_t minFreq = 0, maxFreq = 0;
    size_t size = sizeof(uint64_t);

    if (sysctlbyname("hw.cpufrequency_min", &minFreq, &size, nullptr, 0) == 0
        && sysctlbyname("hw.cpufrequency_max", &maxFreq, &size, nullptr, 0) == 0) {
        return maxFreq > minFreq;
    }
    return false;
}

Solid::Processor::InstructionSets Processor::instructionSets() const
{
    // use sysctlbyname() and "machdep.cpu.features" + "machdep.cpu.extfeatures"
    static Solid::Processor::InstructionSets cpuextensions = Solid::Backends::Shared::cpuFeatures();

    return cpuextensions;
}

QString Processor::vendor()
{
    QString qVendor;
    q_sysctlbyname("machdep.cpu.vendor", qVendor);
    return qVendor;
}

QString Processor::product()
{
    QString product;
    q_sysctlbyname("machdep.cpu.brand_string", product);
    return product;
}
