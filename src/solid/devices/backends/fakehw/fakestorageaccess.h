/*
    Copyright 2007 Kevin Ottens <ervin@kde.org>

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

#ifndef SOLID_BACKENDS_FAKEHW_FAKESTORAGEACCESS_H
#define SOLID_BACKENDS_FAKEHW_FAKESTORAGEACCESS_H

#include <solid/devices/ifaces/storageaccess.h>
#include "fakedeviceinterface.h"

namespace Solid
{
namespace Backends
{
namespace Fake
{
class FakeStorageAccess : public FakeDeviceInterface, virtual public Solid::Ifaces::StorageAccess
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageAccess)

public:
    FakeStorageAccess(FakeDevice *device);
    virtual ~FakeStorageAccess();

    bool isAccessible() const override;
    QString filePath() const override;
    bool isIgnored() const override;
public Q_SLOTS:
    bool setup() override;
    bool teardown() override;

Q_SIGNALS:
    void accessibilityChanged(bool accessible, const QString &udi) override;
    void setupDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
    void teardownDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
    void setupRequested(const QString &udi) override;
    void teardownRequested(const QString &udi) override;

private Q_SLOTS:
    void onPropertyChanged(const QMap<QString, int> &changes);
};
}
}
}

#endif // SOLID_BACKENDS_FAKEHW_FAKESTORAGEACCESS_H
