include_directories(
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)

set(solid_LIB_SRCS ${solid_LIB_SRCS}
    devices/backends/iokit/iokitmanager.cpp
    devices/backends/iokit/iokitdevice.cpp
    devices/backends/iokit/cfhelper.cpp
    devices/backends/iokit/dadictionary.cpp
    devices/backends/iokit/iokitdeviceinterface.cpp
    devices/backends/iokit/iokitgenericinterface.cpp
    devices/backends/iokit/iokitprocessor.cpp
    devices/backends/iokit/iokitbattery.cpp
    devices/backends/iokit/iokitblock.cpp
    devices/backends/iokit/iokitstorage.cpp
    devices/backends/iokit/iokitvolume.cpp
    devices/backends/iokit/iokitstorageaccess.cpp
    devices/backends/iokit/iokitopticaldrive.cpp
    devices/backends/iokit/iokitopticaldisc.cpp
)
