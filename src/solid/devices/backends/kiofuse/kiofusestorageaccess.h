/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_KIOFUSE_STORAGEACCESS_H
#define SOLID_BACKENDS_KIOFUSE_STORAGEACCESS_H

#include <solid/devices/ifaces/storageaccess.h>

#include <QObject>

namespace Solid
{
namespace Backends
{
namespace KioFuse
{
class Device;

class StorageAccess : public QObject, public Solid::Ifaces::StorageAccess
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageAccess)

public:
    explicit StorageAccess(Device *device);
    ~StorageAccess() override;

    bool isAccessible() const override;
    QString filePath() const override;
    bool isIgnored() const override;
    bool isEncrypted() const override;
    bool setup() override;
    bool teardown() override;

Q_SIGNALS:
    void accessibilityChanged(bool accessible, const QString &udi) override;
    void setupDone(Solid::ErrorType error, QVariant data, const QString &udi) override;
    void teardownDone(Solid::ErrorType error, QVariant data, const QString &udi) override;
    void setupRequested(const QString &udi) override;
    void teardownRequested(const QString &udi) override;
    void checkRequested(const QString &udi) override;
    void checkDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
    void repairRequested(const QString &udi) override;
    void repairDone(Solid::ErrorType error, QVariant resultData, const QString &udi) override;

private:
    Device *m_device;
};

}
}
}

#endif // SOLID_BACKENDS_KIOFUSE_STORAGEACCESS_H
