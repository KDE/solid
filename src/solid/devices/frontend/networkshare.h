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

/*!
 * \class Solid::NetworkShare
 * \inheaderfile Solid/NetworkShare
 * \inmodule Solid
 *
 * \brief This device interface is available for network share volumes.
 *
 * Used to acquire a list of network device types.
 *
 * \code
 * const auto devices = Solid::Device::listFromType(Solid::DeviceInterface::NetworkShare);
 * for (const auto &device : devices) {
 *     switch (device.as<Solid::NetworkShare>()->type()) {
 *     case Solid::NetworkShare::Cifs:
 *         hasCifsShare = true;
 *         continue;
 *     case Solid::NetworkShare::Smb3:
 *         hasSmb3Share = true;
 *         continue;
 *     case Solid::NetworkShare::Nfs:
 *         hasNfsShare = true;
 *         continue;
 *     default:
 *         continue;
 *     }
 * }
 * \endcode
 * \since 4.7
 */
class SOLID_EXPORT NetworkShare : public DeviceInterface
{
    Q_OBJECT

    /*!
     * \property Solid::NetworkShare::type
     */
    Q_PROPERTY(ShareType type READ type)

    /*!
     * \property Solid::NetworkShare::url
     */
    Q_PROPERTY(QUrl url READ url)

    Q_DECLARE_PRIVATE(NetworkShare)
    friend class Device;

private:
    /*!
     * \internal
     *
     * Creates a new NetworkShare object.
     * You generally won't need this. It's created when necessary using
     * Device::as().
     *
     * \a backendObject the device interface object provided by the backend
     * \sa Solid::Device::as()
     */
    SOLID_NO_EXPORT explicit NetworkShare(QObject *backendObject);

public:
    ~NetworkShare() override;

    /*!
     * This enum type defines the type of networkShare device can be.
     *
     * \value Unknown An unsupported network protocol
     * \value Nfs NFS protocol
     * \value Cifs Samba protocol
     * \value Smb3 Samba protocol (version 3)
     */
    enum ShareType { Unknown, Nfs, Cifs, Smb3 };
    Q_ENUM(ShareType)

    /*!
     * Returns the Solid::DeviceInterface::Type of the NetworkShare device interface.
     * \sa Solid::DeviceInterface::Type
     */
    static Type deviceInterfaceType()
    {
        return DeviceInterface::NetworkShare;
    }

    /*!
     * Returns the type of network share.
     */
    ShareType type() const;

    /*!
     * Returns the url of network share.
     */
    QUrl url() const;
};
}

#endif
