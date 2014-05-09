set(solid_LIB_SRCS
    ${solid_LIB_SRCS}
    power/job.cpp
)
if(WITH_NEW_POWER_ASYNC_API)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/power)
    set(solid_LIB_SRCS
        ${solid_LIB_SRCS}
        power/power.cpp
        power/acpluggedjob.cpp
        power/powerbackendloader.cpp

        power/backends/abstractacpluggedjob.h
        power/backends/powernotifier.h
        power/backends/dummy/dummyacpluggedjob.cpp
        power/backends/dummy/dummypowernotifier.cpp
    )
endif()