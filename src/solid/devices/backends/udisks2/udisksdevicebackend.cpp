/*
    Copyright 2010 Michael Zanetti <mzanetti@kde.org>
    Copyright 2010-2012 Lukáš Tinkl <ltinkl@redhat.com>
    Copyright 2012 Dan Vrátil <dvratil@redhat.com>

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

#include "udisksdevicebackend.h"
#include "udisks_debug.h"

#include <QDBusConnection>
#include <QDBusInterface>

#include "solid/deviceinterface.h"
#include "solid/genericinterface.h"

#include "udisksmanager.h"

using namespace Solid::Backends::UDisks2;

/* Static cache for DeviceBackends for all UDIs */
QMap<QString /* UDI */, DeviceBackend *> DeviceBackend::s_backends;

Manager *DeviceBackend::s_manager = nullptr;

DeviceBackend *DeviceBackend::backendForUDI(const QString &udi, bool create)
{
    DeviceBackend *backend = nullptr;
    if (udi.isEmpty()) {
        return backend;
    }

    backend = s_backends.value(udi);
    if (!backend && create) {
        backend = new DeviceBackend(udi);
        s_backends.insert(udi, backend);
    }

    return backend;
}

void DeviceBackend::destroyBackend(const QString &udi)
{
    delete s_backends.take(udi);
}

Manager *DeviceBackend::manager()
{
    return s_manager;
}

void DeviceBackend::setManager(Manager *manager)
{
    s_manager = manager;
}

DeviceBackend::DeviceBackend(const QString &udi)
    : m_udi(udi)
{
    Q_ASSERT(s_manager);
    connect(this, &DeviceBackend::propertyChanged, this, &DeviceBackend::changed);
}

DeviceBackend::~DeviceBackend()
{

}

QStringList DeviceBackend::interfaces() const
{
    if (m_interfaces.isEmpty()) {
        m_interfaces = s_manager->interfaces(m_udi);
    }
    return m_interfaces;
}

const QString &DeviceBackend::udi() const
{
    return m_udi;
}

QVariant DeviceBackend::prop(const QString &key) const
{
    checkCache(key);
    return m_propertyCache.value(key);
}

bool DeviceBackend::propertyExists(const QString &key) const
{
    checkCache(key);
    return m_propertyCache.value(key).isValid();
}

QVariantMap DeviceBackend::allProperties() const
{
    m_propertyCache.clear();

    const PropertyMap properties = s_manager->properties(m_udi);

    // flatten the properties into a single map as that is was the code was originally built upon
    // TODO I bet there is a neat std::transform or something to do this?
    for (auto it = properties.begin(), end = properties.end(); it != end; ++it) {
        const auto props = it.value();

        for (auto propsIt = props.begin(), propsEnd = props.end(); propsIt != propsEnd; ++propsIt) {
            m_propertyCache.insert(propsIt.key(), propsIt.value());
        }
    }

    return m_propertyCache;
}

void DeviceBackend::invalidateInterfaces()
{
    m_interfaces.clear();
}

void DeviceBackend::invalidateProperties()
{
    invalidateInterfaces();
    m_propertyCache.clear();
}

void DeviceBackend::checkCache(const QString &key) const
{
    if (m_propertyCache.isEmpty()) { // recreate the cache
        allProperties();
    }

    if (m_propertyCache.contains(key)) {
        return;
    }

    // old code fetched the property if neccessary but we have the manager take care of this
}
