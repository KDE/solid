/*
    Copyright 2009 Pino Toscano <pino@kde.org>

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

#ifndef SOLID_BACKENDS_UPOWER_H
#define SOLID_BACKENDS_UPOWER_H

/* UPower */
#define UP_DBUS_SERVICE           "org.freedesktop.UPower"
#define UP_DBUS_PATH              "/org/freedesktop/UPower"
#define UP_DBUS_INTERFACE         "org.freedesktop.UPower"
#define UP_DBUS_INTERFACE_DEVICE  UP_DBUS_INTERFACE ".Device"
#define UP_UDI_PREFIX             "/org/freedesktop/UPower"

typedef enum {
	UP_DEVICE_KIND_UNKNOWN,
	UP_DEVICE_KIND_LINE_POWER,
	UP_DEVICE_KIND_BATTERY,
	UP_DEVICE_KIND_UPS,
	UP_DEVICE_KIND_MONITOR,
	UP_DEVICE_KIND_MOUSE,
	UP_DEVICE_KIND_KEYBOARD,
	UP_DEVICE_KIND_PDA,
	UP_DEVICE_KIND_PHONE,
	UP_DEVICE_KIND_MEDIA_PLAYER,
	UP_DEVICE_KIND_TABLET,
	UP_DEVICE_KIND_COMPUTER,
	UP_DEVICE_KIND_GAMING_INPUT,
	UP_DEVICE_KIND_LAST
} UpDeviceKind;

#endif // SOLID_BACKENDS_UPOWER_H
