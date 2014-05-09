set(solid_LIB_SRCS
    ${solid_LIB_SRCS}
    power/job.cpp
)
if(WITH_NEW_POWER_ASYNC_API)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/power)
    set(solid_LIB_SRCS
        ${solid_LIB_SRCS}
        power/acpluggedjob.cpp
        power/powerbackendloader.cpp

        power/backends/abstractacpluggedjob.h
        power/backends/dummy/dummyacpluggedjob.cpp
    )
endif()