set(solid_LIB_SRCS ${solid_LIB_SRCS}
    backends/udev/udevdevice.cpp
    backends/udev/udevmanager.cpp
    backends/udev/udevdeviceinterface.cpp
    backends/udev/udevgenericinterface.cpp
    backends/udev/cpuinfo.cpp
    backends/udev/udevprocessor.cpp
    backends/udev/udevcamera.cpp
    backends/udev/udevportablemediaplayer.cpp
    backends/udev/udevblock.cpp
    backends/shared/udevqtclient.cpp
    backends/shared/udevqtdevice.cpp
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