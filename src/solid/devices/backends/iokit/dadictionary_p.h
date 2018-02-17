/*
    Copyright 2018 René J.V. Bertin <rjvbertin@gmail.com>

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

#ifndef SOLID_BACKENDS_IOKIT_DADICTIONARY_H
#define SOLID_BACKENDS_IOKIT_DADICTIONARY_H

#include <QString>

#include "iokitdevice.h"

#include <CoreFoundation/CoreFoundation.h>
#include <DiskArbitration/DiskArbitration.h>

namespace Solid
{
namespace Backends
{
namespace IOKit
{
class DADictionary
{
public:
    DADictionary(const IOKitDevice *device);
    virtual ~DADictionary();

    bool getDict();
    void releaseDict();
    const QString stringForKey(const CFStringRef key);
    CFURLRef cfUrLRefForKey(const CFStringRef key);
    bool boolForKey(const CFStringRef key, bool &value);

    const IOKitDevice *device;
    DASessionRef daSession;
    DADiskRef daRef;
    CFDictionaryRef daDict;
};
}
}
}

#endif // SOLID_BACKENDS_IOKIT_DADICTIONARY_H
