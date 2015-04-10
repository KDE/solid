
cmake_minimum_required(VERSION 2.8.12)

project(Solid)

find_package(ECM 5.9.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.2.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Xml DBus Widgets)

if(WIN32)
    find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Network)
endif()

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMMarkNonGuiExecutable)

if (CMAKE_SYSTEM_NAME MATCHES Linux)
    find_package( UDev )

    set_package_properties(UDev PROPERTIES  URL "http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html"
                                            DESCRIPTION "UDev support for Solid"
                                            TYPE REQUIRED
                                            PURPOSE "Allows Solid to use UDev to provide information about devices on Linux"
                            )
endif()


include(ECMPoQmTools)

set(KF5_VERSION "5.10.0") # handled by release scripts

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX SOLID
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/solid_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5SolidConfigVersion.cmake"
                        SOVERSION 5)

# TODO: Remove these
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

option(WITH_NEW_SOLID_JOB "WIP: base Job class" Off)
add_feature_info(Solid::Job WITH_NEW_SOLID_JOB "WIP: Base class for Solid Asynchronous apis")

option(WITH_NEW_POWER_ASYNC_API "WIP: Asynchronous API for power management" Off)
add_feature_info(Solid::PowerManagement WITH_NEW_POWER_ASYNC_API "WIP: Asynchronous API for power management")

option(WITH_NEW_POWER_ASYNC_FREEDESKTOP "WIP: Freedesktop backend for the asynchronous api" Off)
add_feature_info(Solid::PowerManagement WITH_NEW_POWER_ASYNC_FREEDESKTOP "WIP: Freedesktop backend for the asynchronous api")

add_subdirectory(src)
add_subdirectory(autotests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Solid")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5SolidConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5SolidConfig.cmake"
    PATH_VARS KDE_INSTALL_DBUSINTERFACEDIR
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5SolidConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5SolidConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5SolidTargets  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"  FILE KF5SolidTargets.cmake  NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/solid_version.h  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}  COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
