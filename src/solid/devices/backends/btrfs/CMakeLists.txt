set(backend_libs LibMount::LibMount)

set(backend_sources
    btrfsmanager.cpp
    btrfsdevice.cpp
    btrfsdeviceinterface.cpp
    btrfsgenericinterface.cpp
    btrfshandling.cpp
    btrfsstoragesubvolumetree.cpp
)

ecm_qt_declare_logging_category(backend_sources
    HEADER btrfs_debug.h
    IDENTIFIER Solid::Backends::Btrfs::BTRFS_LOG
    CATEGORY_NAME kf.solid.backends.btrfs
    OLD_CATEGORY_NAMES org.kde.solid.btrfs
    DESCRIPTION "Btrfs (Solid)"
    EXPORT SOLID
)
