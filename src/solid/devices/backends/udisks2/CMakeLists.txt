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

# We need the major(3) and minor(3) macros to decompose dev_t device ids
# properly in udisksblock. BSD systems normally have these in sys/types.h.
# Glibc on Linux has a version in that header, but it spits out a deprecation
# warning - on Linux, one should use sys/sysmacros.h. Search for these in
# the relevant headers.
include(CheckSymbolExists)
check_symbol_exists(major "sys/sysmacros.h" HAVE_SYSMACROS_MAJOR)
check_symbol_exists(minor "sys/sysmacros.h" HAVE_SYSMACROS_MINOR)
check_symbol_exists(major "sys/types.h" HAVE_SYSTYPES_MAJOR)
check_symbol_exists(minor "sys/types.h" HAVE_SYSTYPES_MINOR)
check_symbol_exists(major "sys/mkdev.h" HAVE_SYSMKDEV_MAJOR)
check_symbol_exists(minor "sys/mkdev.h" HAVE_SYSMKDEV_MINOR)

# Set each of these three properties to 0 or 1
add_definitions(
  -DHAVE_SYSMACROS_MAJOR_MINOR=$<AND:$<BOOL:${HAVE_SYSMACROS_MAJOR}>,$<BOOL:${HAVE_SYSMACROS_MINOR}>>
)
add_definitions(
  -DHAVE_SYSTYPES_MAJOR_MINOR=$<AND:$<BOOL:${HAVE_SYSTYPES_MAJOR}>,$<BOOL:${HAVE_SYSTYPES_MINOR}>>
)
add_definitions(
  -DHAVE_SYSMKDEV_MAJOR_MINOR=$<AND:$<BOOL:${HAVE_SYSMKDEV_MAJOR}>,$<BOOL:${HAVE_SYSMKDEV_MINOR}>>
)
