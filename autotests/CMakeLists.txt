include(ECMMarkAsTest)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Concurrent Test)

########### fakehardwaretest ###############

set(fakehardwaretest_SRCS fakehardwaretest.cpp )


add_executable(fakehardwaretest ${fakehardwaretest_SRCS})
add_test(fakehardwaretest fakehardwaretest)
ecm_mark_as_test(fakehardwaretest)

target_link_libraries(fakehardwaretest KF5Solid_static Qt5::DBus Qt5::Xml Qt5::Test)
# QT5 TODO Port solid to QFINDTESTDATA
target_compile_definitions(fakehardwaretest PRIVATE SOLID_STATIC_DEFINE=1 TEST_DATA="${CMAKE_CURRENT_SOURCE_DIR}/../src/solid/devices/backends/fakehw/fakecomputer.xml")


########### halbasictest ###############

if(NOT WIN32 AND NOT APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES Linux)
   set(halbasictest_SRCS halbasictest.cpp )

   add_executable(halbasictest ${halbasictest_SRCS})

   target_link_libraries(halbasictest KF5Solid_static Qt5::DBus Qt5::Xml Qt5::Test)

   ecm_mark_as_test(halbasictest)
endif()

########### solidhwtest ###############

set(solidhwtest_SRCS
   solidhwtest.cpp )


add_executable(solidhwtest ${solidhwtest_SRCS})
add_test(solidhwtest solidhwtest)
ecm_mark_as_test(solidhwtest)
target_compile_definitions(solidhwtest PRIVATE SOLID_STATIC_DEFINE=1 FAKE_COMPUTER_XML="${CMAKE_CURRENT_SOURCE_DIR}/../src/solid/devices/backends/fakehw/fakecomputer.xml")

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../src/solid/devices/backends/fakehw)

target_link_libraries(solidhwtest Qt5::DBus Qt5::Xml Qt5::Test ${LIBS} KF5Solid_static)

########### solidmttest ###############

set(solidmttest_SRCS
   solidmttest.cpp )

find_package(Qt5Concurrent 5.2.0 REQUIRED NO_MODULE)

add_executable(solidmttest ${solidmttest_SRCS})
add_test(solidmttest solidmttest)
ecm_mark_as_test(solidmttest)
target_compile_definitions(solidmttest PRIVATE SOLID_STATIC_DEFINE=1)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../src/solid/backends/fakehw )

target_link_libraries(solidmttest Qt5::DBus Qt5::Xml Qt5::Test ${LIBS} KF5Solid_static Qt5::Concurrent)

