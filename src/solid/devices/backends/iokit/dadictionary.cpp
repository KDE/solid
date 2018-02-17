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

#include "dadictionary_p.h"

using namespace Solid::Backends::IOKit;

DADictionary::DADictionary(const IOKitDevice *device)
    : device(device)
    , daDict(nullptr)
{
    daSession = DASessionCreate(kCFAllocatorDefault);
    if (daSession) {
        const QString devName = device->property(QStringLiteral("BSD Name")).toString();
        daRef = DADiskCreateFromBSDName(kCFAllocatorDefault, daSession, devName.toStdString().c_str());
    } else {
        daRef = nullptr;
    }
}

DADictionary::~DADictionary()
{
    releaseDict();
    if (daRef) {
        CFRelease(daRef);
        daRef = nullptr;
    }
    if (daSession) {
        CFRelease(daSession);
        daSession = nullptr;
    }
}

bool DADictionary::getDict()
{
    if (daRef) {
        daDict = DADiskCopyDescription(daRef);
    }
    return daRef != nullptr;
}

void DADictionary::releaseDict()
{
    if (daDict) {
        CFRelease(daDict);
        daDict = nullptr;
    }
}

const QString DADictionary::stringForKey(const CFStringRef key)
{
    QString ret;
    if (getDict()) {
        ret = QString::fromCFString((const CFStringRef) CFDictionaryGetValue(daDict, key));
    }
    releaseDict();
    return ret;
}

CFURLRef DADictionary::cfUrLRefForKey(const CFStringRef key)
{
    CFURLRef ret = nullptr;
    if (getDict()) {
        ret = (const CFURLRef) CFDictionaryGetValue(daDict, key);
    }
    // we cannot release the dictionary here, or else we'd need to
    // copy the CFURLRef and oblige our caller to release the return value.
    return ret;
}

bool DADictionary::boolForKey(const CFStringRef key, bool &value)
{
    if (getDict()) {
        const CFBooleanRef boolRef = (const CFBooleanRef) CFDictionaryGetValue(daDict, key);
        if (boolRef) {
            value = CFBooleanGetValue(boolRef);
        }
        return boolRef != nullptr;
    }
    return false;
}
