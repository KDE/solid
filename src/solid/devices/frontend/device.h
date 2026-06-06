/*
    SPDX-FileCopyrightText: 2005-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_DEVICE_H
#define SOLID_DEVICE_H

#include <QList>
#include <QSharedData>

#include <solid/solid_export.h>

#include <solid/deviceinterface.h>

namespace Solid
{
class DevicePrivate;

/*!
 * \class Solid::Device
 * \inheaderfile Solid/Device
 * \inmodule Solid
 *
 * \brief Allows applications to deal with devices available in the
 * underlying system.
 *
 * Device stores a reference to device data provided by the backend.
 *
 * Device objects are designed to be used by value. Copying these objects
 * is quite cheap, so using pointers is generally not needed.
 */
class SOLID_EXPORT Device
{
public:
    /*!
     * Returns all devices available in the underlying system.
     */
    static QList<Device> allDevices();

    /*!
     * Returns a list of devices matching the given \a type.
     *
     * The \a parentUdi should match the provider for the device. For example:
     *
     * \list
     * \li /org/kde/solid/udev
     * \li /org/freedesktop/UPower
     * \li /org/freedesktop/UDisks2
     * \endlist
     */
    static QList<Device> listFromType(const DeviceInterface::Type &type, const QString &parentUdi = QString());

    /*!
     * Returns a list of devices matching the given \a predicate query.
     *
     * The \a parentUdi should match the provider for the device. For example:
     *
     * \list
     * \li /org/kde/solid/udev
     * \li /org/freedesktop/UPower
     * \li /org/freedesktop/UDisks2
     * \endlist
     *
     * \sa Solid::Predicate
     */
    static QList<Device> listFromQuery(const Predicate &predicate, const QString &parentUdi = QString());

    /*!
     * \overload listFromQuery()
     */
    static QList<Device> listFromQuery(const QString &predicate, const QString &parentUdi = QString());

    /*!
     * Returns the Device containing the filesystem for the given \a path.
     *
     * For Devices implementing the
     * StorageVolume interface, only ones matching UsageType::FileSystem are
     * returned, i.e. no backing encrypted volumes.
     * \since 5.73
     * \sa QFileInfo::canonicalFilePath
     */
    static Device storageAccessFromPath(const QString &path);

    /*!
     * Constructs a device for a given Universal Device Identifier \a udi.
     */
    explicit Device(const QString &udi = QString());

    Device(const Device &device);

    ~Device();

    Device &operator=(const Device &device);

    /*!
     * Returns if this device is valid (available in the system).
     */
    bool isValid() const;

    /*!
     * Returns the Universal Device Identifier \a udi.
     *
     * \warning Don't use the UDI for anything except communication with Solid. Also don't store
     * UDIs as there's no guarantee that the UDI stays the same when the hardware setup changed.
     * The UDI is a unique identifier that is local to the computer in question and for the
     * current boot session. The UDIs may change after a reboot.
     * Similar hardware in other computers may have different values; different
     * hardware could have the same UDI.
     */
    QString udi() const;

    /*!
     * Returns the Universal Device Identifier (UDI) of the Device's parent.
     */
    QString parentUdi() const;

    /*!
     * Returns the parent of the Device.
     * \sa parentUdi()
     */
    Device parent() const;

    /*!
     * Returns the name of the device vendor.
     */
    QString vendor() const;

    /*!
     * Returns the name of the product corresponding to this device.
     */
    QString product() const;

    /*!
     * Returns the name of the icon representing this device.
     *
     * The naming follows the freedesktop.org specification.
     */
    QString icon() const;

    /*!
     * Returns the names of the emblems representing the state of this device.
     *
     * The naming follows the freedesktop.org specification.
     * \since 4.4
     */
    QStringList emblems() const;

    /*!
     * Returns the display name to use for this device.
     *
     * Same as description when not defined.
     * \since 5.71
     */
    QString displayName() const;

    /*!
     * Returns the description of this device.
     * \since 4.4
     */
    QString description() const;

    /*!
     * Tests if a device interface \a type is available from the device.
     * \code
     * if (device.isDeviceInterface(Solid::DeviceInterface::StorageAccess)) {
     *     // ...
     * }
     * \endcode
     */
    bool isDeviceInterface(const DeviceInterface::Type &type) const;

    /*!
     * Returns a specialized interface to interact with the device corresponding to
     * a particular device interface \a type, or nullptr if it doesn't exist.
     * \code
     * const Solid::Battery* battery =
     *     qobject_cast<const Solid::Battery>(device.asDeviceInterface(Solid::DeviceInterface::Battery));
     * \endcode
     */
    DeviceInterface *asDeviceInterface(const DeviceInterface::Type &type);

    /*!
     * Returns a specialized interface to interact with the device corresponding to
     * a particular device interface \a type, or nullptr if it doesn't exist.
     */
    const DeviceInterface *asDeviceInterface(const DeviceInterface::Type &type) const;

    /*!
     * Returns a specialized interface to interact with the device corresponding
     * to a given device interface, or nullptr if it doesn't exist.
     * \code
     * const Solid::Processor* processor = device.as<Solid::Processor>();
     * \endcode
     */
    template<class DevIface>
    DevIface *as()
    {
        DeviceInterface::Type type = DevIface::deviceInterfaceType();
        DeviceInterface *iface = asDeviceInterface(type);
        return qobject_cast<DevIface *>(iface);
    }

    /*!
     * Returns a specialized interface to interact with the device corresponding
     * to a given device interface, or nullptr if it doesn't exist.
     * \code
     * const Solid::Processor* processor = device.as<const Solid::Processor>();
     * \endcode
     */
    template<class DevIface>
    const DevIface *as() const
    {
        DeviceInterface::Type type = DevIface::deviceInterfaceType();
        const DeviceInterface *iface = asDeviceInterface(type);
        return qobject_cast<const DevIface *>(iface);
    }

    /*!
     * Tests if a device provides a given device interface.
     * \code
     * if (device.is<Solid::StorageDrive>()) {
     *     // ...
     * }
     * \endcode
     */
    template<class DevIface>
    bool is() const
    {
        return isDeviceInterface(DevIface::deviceInterfaceType());
    }

private:
    QExplicitlySharedDataPointer<DevicePrivate> d;
    friend class DeviceManagerPrivate;
};
}

Q_DECLARE_TYPEINFO(Solid::Device, Q_RELOCATABLE_TYPE);

#endif
