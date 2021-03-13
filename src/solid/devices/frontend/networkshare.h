/*
    SPDX-FileCopyrightText: 2011 Mario Bensi <mbensi@ipsquad.net>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_NETWORKSHARE_H
#define SOLID_NETWORKSHARE_H

#include <solid/solid_export.h>

#include <solid/deviceinterface.h>

#include <QUrl>

namespace Solid
{
class Device;
class NetworkSharePrivate;

/**
 * NetworkShare interface.
 *
 * a NetworkShare interface is used to determine the type of
 * network access.
 * @since 4.7
 */
class SOLID_EXPORT NetworkShare : public DeviceInterface
{
    Q_OBJECT
    Q_PROPERTY(ShareType type READ type)
    Q_PROPERTY(QUrl url READ url)
    Q_DECLARE_PRIVATE(NetworkShare)
    friend class Device;

private:
    /**
     * Creates a new NetworkShare object.
     * You generally won't need this. It's created when necessary using
     * Device::as().
     *
     * @param backendObject the device interface object provided by the backend
     * @see Solid::Device::as()
     */
    explicit NetworkShare(QObject *backendObject);

public:
    /**
     * Destroys a NetworkShare object.
     */
    virtual ~NetworkShare();

    /**
     * This enum type defines the type of networkShare device can be.
     *
     * - Unknown : a unsupported network protocol
     * - Nfs : nfs protocol
     * - Cifs : samba protocol
     */

    enum ShareType { Unknown, Nfs, Cifs };
    Q_ENUM(ShareType)

    /**
     * Get the Solid::DeviceInterface::Type of the NetworkShare device interface.
     *
     * @return the NetworkShare device interface type
     * @see Solid::DeviceInterface::Type
     */
    static Type deviceInterfaceType()
    {
        return DeviceInterface::NetworkShare;
    }

    /**
     * Retrieves the type of network share
     *
     * @return the type of network share
     */
    ShareType type() const;

    /**
     * Retrieves the url of network share
     *
     * @return the url of network share
     */
    QUrl url() const;
};
}

#endif
