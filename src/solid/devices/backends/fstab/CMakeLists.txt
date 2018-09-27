set(solid_LIB_SRCS ${solid_LIB_SRCS}
    devices/backends/fstab/fstabmanager.cpp
    devices/backends/fstab/fstabdevice.cpp
    devices/backends/fstab/fstabnetworkshare.cpp
    devices/backends/fstab/fstabstorageaccess.cpp
    devices/backends/fstab/fstabhandling.cpp
    devices/backends/fstab/fstabwatcher.cpp
)

ecm_qt_declare_logging_category(solid_LIB_SRCS
                                HEADER fstab_debug.h
                                IDENTIFIER Solid::Backends::Fstab::FSTAB
                                DEFAULT_SEVERITY Warning
                                CATEGORY_NAME org.kde.solid.fstab)
