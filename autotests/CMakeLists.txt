include(ECMAddTests)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Concurrent Test)

########### fakehardwaretest ###############

if (BUILD_DEVICE_BACKEND_fakehw)
    ecm_add_test(fakehardwaretest.cpp LINK_LIBRARIES KF5Solid_static Qt${QT_MAJOR_VERSION}::Xml Qt${QT_MAJOR_VERSION}::Test)
    # QT5 TODO Port solid to QFINDTESTDATA
    target_compile_definitions(fakehardwaretest PRIVATE SOLID_STATIC_DEFINE=1 TEST_DATA="${CMAKE_CURRENT_SOURCE_DIR}/../src/solid/devices/backends/fakehw/fakecomputer.xml")
endif()

########### solidhwtest ###############

if (BUILD_DEVICE_BACKEND_fakehw)
    ecm_add_test(solidhwtest.cpp LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Xml Qt${QT_MAJOR_VERSION}::Test ${LIBS} KF5Solid_static)
    target_compile_definitions(solidhwtest PRIVATE SOLID_STATIC_DEFINE=1 FAKE_COMPUTER_XML="${CMAKE_CURRENT_SOURCE_DIR}/../src/solid/devices/backends/fakehw/fakecomputer.xml")
    target_include_directories(solidhwtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/solid/devices/backends/fakehw)
endif()

########### solidmttest ###############

ecm_add_test(solidmttest.cpp LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Xml Qt${QT_MAJOR_VERSION}::Test ${LIBS} KF5Solid_static Qt${QT_MAJOR_VERSION}::Concurrent)
target_compile_definitions(solidmttest PRIVATE SOLID_STATIC_DEFINE=1)

########### solidmttest ###############
if (WITH_NEW_SOLID_JOB)
    ecm_add_test(solidjobtest.cpp LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test ${LIBS} KF5Solid_static)
endif()

########### solidpowertest ###############
if (WITH_NEW_SOLID_JOB AND WITH_NEW_POWER_ASYNC_API)
    ecm_add_test(solidpowertest.cpp LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test ${LIBS} KF5Solid_static)
endif()

########### solidpowertest ###############
if(WITH_NEW_POWER_ASYNC_API AND WITH_NEW_POWER_ASYNC_FREEDESKTOP)
    set(solidFreedesktopTest_SRCS solidfreedesktoptest.cpp fakeUpower.cpp fakelogind.cpp)
    ecm_add_test(${solidFreedesktopTest_SRCS} TEST_NAME "solidfreedesktopbackend" LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::DBus ${LIBS} KF5Solid_static)
endif()

########### logindinhibitionargument ###########
if(WITH_NEW_POWER_ASYNC_API AND WITH_NEW_POWER_ASYNC_FREEDESKTOP)
set(solidLogindInhibitionArgument_SRCS
    logindinhibitionargumenttest.cpp
    ../src/solid/power/backends/freedesktop/logindinhibitionargument.cpp
)
ecm_add_test(${solidLogindInhibitionArgument_SRCS} TEST_NAME "logindinhibitionargument" LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5Solid_static)
endif()
