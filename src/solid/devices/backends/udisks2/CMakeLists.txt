set(solid_LIB_SRCS ${solid_LIB_SRCS}
    devices/backends/udisks2/udisksmanager.cpp
    devices/backends/udisks2/udisksdevice.cpp
    devices/backends/udisks2/udisksdevicebackend.cpp
    devices/backends/udisks2/udisksblock.cpp
    devices/backends/udisks2/udisksstoragevolume.cpp
    devices/backends/udisks2/udisksdeviceinterface.cpp
    devices/backends/udisks2/udisksopticaldisc.cpp
    devices/backends/udisks2/udisksopticaldrive.cpp
    devices/backends/udisks2/udisksstoragedrive.cpp
    devices/backends/udisks2/udisksstorageaccess.cpp
    devices/backends/udisks2/udisksgenericinterface.cpp
    devices/backends/udisks2/dbus/manager.cpp
)

ecm_qt_declare_logging_category(solid_LIB_SRCS
                                HEADER udisks_debug.h
                                IDENTIFIER Solid::Backends::UDisks2::UDISKS2
                                DEFAULT_SEVERITY Warning
                                CATEGORY_NAME org.kde.solid.udisks2)
