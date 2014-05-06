
cmake_minimum_required(VERSION 2.8.12)

project(Solid)

find_package(ECM 0.0.13 REQUIRED NO_MODULE)

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

if(NOT WIN32 AND NOT APPLE)
  find_package( UDev )
  if (NOT UDEV_HAVE_GET_SYSATTR_LIST_ENTRY) # udev_device_get_sysattr_list_entry() is used in udevqtdevice.cpp
    message(STATUS "Not building Solid UDev backend, version 167 or newer is needed." )
    set(UDEV_FOUND FALSE)
  endif()

  set_package_properties(UDev PROPERTIES  URL "http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html"
                                          DESCRIPTION "UDev support for Solid"
                                          TYPE OPTIONAL
                                          PURPOSE "Allows Solid to use UDev to provide information about devices on Linux"
                         )
endif()


set(WBEM_FOUND FALSE)
if(WIN32)
  include(CheckIncludeFileCXX)
  check_include_file_cxx(wbemidl.h HAVE_WBEM)
  find_library(WBEM_LIBRARIES NAMES wbemuuid wbemuuidd)
  if(HAVE_WBEM AND WBEM_LIBRARIES)
    set(HAVE_WBEM TRUE)
    set(WBEM_FOUND TRUE)
    message(STATUS "Found wbemuuid library: ${WBEM_LIBRARIES}")
  endif()
endif()

############### Give the user the option to build the udisks2 solid backend instead of the udisks backend ###############
option(WITH_SOLID_UDISKS2 "Enable the udisks2 solid backend instead" "ON")

############### Give the user the option to build the deprecated WMI solid backend instead of the new win backend ###############
option(WITH_SOLID_WMI "Enables the deprecated WMI backend on Windows")

include(ECMPoQmTools)

set(KF5_VERSION "4.99.0") # handled by release scripts

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX SOLID
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/solid_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5SolidConfigVersion.cmake"
                        SOVERSION 5)

# TODO: Remove these
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_URL_CAST_FROM_STRING)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()
add_subdirectory(src)
add_subdirectory(autotests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5Solid")

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5SolidConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5SolidConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KF5SolidConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KF5SolidConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KF5SolidTargets  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"  FILE KF5SolidTargets.cmake  NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/solid_version.h  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}  COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
