include_directories(${CMAKE_CURRENT_SOURCE_DIR}/power)

set(solid_LIB_SRCS
    ${solid_LIB_SRCS}
    power/backends/freedesktop/fdacpluggedjob.cpp
    power/backends/freedesktop/fdpowernotifier.cpp
)