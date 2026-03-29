/*
    SPDX-FileCopyrightText: 2005-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_DEVICENOTIFIER_H
#define SOLID_DEVICENOTIFIER_H

#include <QObject>

#include <solid/solid_export.h>

namespace Solid
{
/*!
 * \class Solid::DeviceNotifier
 * \inheaderfile Solid/DeviceNotifier
 * \inmodule Solid
 *
 * \brief This class allow to query the underlying system for information
 * about the hardware available.
 *
 * Applications should use this to be notified about hardware changes.
 *
 * Note that it's implemented as a singleton and encapsulates the backend logic.
 */
class SOLID_EXPORT DeviceNotifier : public QObject // krazy:exclude=dpointer (interface class)
{
    Q_OBJECT
public:
    /*!
     * Returns a pointer to the global DeviceNotifier instance
     * to allow for its use with QObject::connect();
     * \code
     * auto notifier = Solid::DeviceNotifier::instance();
     *
     * auto refresh = [this] (const QString &udi) {
     *     Solid::Device device(udi);
     *     if (device.is<Solid::StorageDrive>()) {
     *         qInfo() << "A new storage drive was added!";
     *     }
     * };
     * QObject::connect(notifier, &Solid::DeviceNotifier::deviceAdded, refresh);
     * \endcode
     */
    static DeviceNotifier *instance();

Q_SIGNALS:
    /*!
     * This signal is emitted when a new device
     * with the given \a udi appears in the underlying system.
     */
    void deviceAdded(const QString &udi);

    /*!
     * This signal is emitted when a device
     * with the given \a udi disappears from the underlying system.
     */
    void deviceRemoved(const QString &udi);
};
}

#endif
