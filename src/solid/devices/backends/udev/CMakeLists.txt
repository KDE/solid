set(solid_LIB_SRCS ${solid_LIB_SRCS}
    devices/backends/udev/udevdevice.cpp
    devices/backends/udev/udevmanager.cpp
    devices/backends/udev/udevdeviceinterface.cpp
    devices/backends/udev/udevgenericinterface.cpp
    devices/backends/udev/cpuinfo.cpp
    devices/backends/udev/udevprocessor.cpp
    devices/backends/udev/udevcamera.cpp
    devices/backends/udev/udevportablemediaplayer.cpp
    devices/backends/udev/udevblock.cpp
    devices/backends/shared/udevqtclient.cpp
    devices/backends/shared/udevqtdevice.cpp
)

set(UDEV_DETAILED_OUTPUT OFF CACHE BOOL "provide extended output regarding udev events")
if(UDEV_DETAILED_OUTPUT)
    add_definitions(-DUDEV_DETAILED_OUTPUT)
endif()

# check for media-player-info (runtime-only optional dependency)
find_package(MediaPlayerInfo)
set_package_properties(MediaPlayerInfo PROPERTIES
    DESCRIPTION "Enables identification and querying of portable media players"
    PURPOSE "Runtime-only dependency of the udev solid backend. Support for m-p-i is included even if not found during build"
    URL "http://www.freedesktop.org/wiki/Software/media-player-info"
    TYPE RUNTIME
)
