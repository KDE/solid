set(backend_sources
    kdeconnectmanager.cpp
    kdeconnectmanager.h
    kdeconnectdevice.cpp
    kdeconnectdevice.h
    kdeconnectbattery.cpp
    kdeconnectbattery.h
    kdeconnectdeviceinterface.cpp
    kdeconnectdeviceinterface.h
    kdeconnectportablemediaplayer.cpp
    kdeconnectportablemediaplayer.h
    kdeconnectutils.cpp
    kdeconnectutils.h

    dbus/kdeconnect_daemon_interface.cpp
    dbus/kdeconnect_daemon_interface.h
    dbus/kdeconnect_device_interface.cpp
    dbus/kdeconnect_device_interface.h
    dbus/kdeconnect_device_battery_interface.cpp
    dbus/kdeconnect_device_battery_interface.h
)

ecm_qt_declare_logging_category(backend_sources
    HEADER kdeconnect_debug.h
    IDENTIFIER Solid::Backends::KdeConnect::KDECONNECT
    DEFAULT_SEVERITY Warning
    CATEGORY_NAME kf.solid.backends.kdeconnect
    DESCRIPTION "KDE Connect (Solid)"
    EXPORT SOLID
)

