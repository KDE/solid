/*
    SPDX-FileCopyrightText: 2013 Ivan Cukic <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef SOLID_DEVICES_P_H
#define SOLID_DEVICES_P_H

#include "devices.h"

#include <QSharedPointer>
#include <QWeakPointer>

#include <solid/device.h>
#include <solid/devicenotifier.h>

namespace Solid
{
/**
 * Instances of this class are used as backends for
 * Devices and DevicesPrivate classes.
 *
 * The purpose of it is to create only one filter
 * (Solid::Predicate) and the corresponding devices list
 * per query, because there can be multiple clients
 * interested in the same device types.
 *
 * The user of the class needs not to worry about object
 * allocation and deallocation - just use DevicesQueryPrivate::forQuery
 * and store the retrieved smart pointer.
 */
class DevicesQueryPrivate : public QObject
{
    Q_OBJECT

public:
    /**
     * Returns a shared pointer to a handler for the specified query
     */
    static QSharedPointer<DevicesQueryPrivate> forQuery(const QString &query);

    ~DevicesQueryPrivate() override;

    /**
     * Returns a list of devices that match the query
     */
    const QStringList &devices() const;

    /**
     * A query which is used to create the predicate.
     * It can be public since it is immutable.
     */
    const QString query;

    /**
     * A predicate used for checking whether a device
     * satisfies the specified query.
     * It can be public since it is immutable.
     */
    const Solid::Predicate predicate;

Q_SIGNALS:
    void deviceAdded(const QString &udi);
    void deviceRemoved(const QString &udi);

public Q_SLOTS:
    void addDevice(const QString &udi);
    void removeDevice(const QString &udi);

private:
    DevicesQueryPrivate(const QString &query);

    // TODO: This could be static or something
    Solid::DeviceNotifier *const notifier;

    QStringList matchingDevices;

    // Maps queries to the handler objects
    static QHash<QString, QWeakPointer<DevicesQueryPrivate>> handlers;
};

} // namespace Solid

#endif
