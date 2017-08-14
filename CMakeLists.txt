
cmake_minimum_required(VERSION 3.0)

set(KF5_VERSION "5.37.0") # handled by release scripts
project(Solid VERSION ${KF5_VERSION})

include(FeatureSummary)
find_package(ECM 5.38.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.7.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Xml DBus Widgets)

if(WIN32)
    find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Network)
endif()


include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMMarkNonGuiExecutable)
include(ECMAddQch)

find_package(FLEX REQUIRED)
set_package_properties(FLEX PROPERTIES
    URL "http://flex.sourceforge.net"
    DESCRIPTION "Fast Lexical Analyzer"
    TYPE REQUIRED
    PURPOSE "Required for the Predicate parser"
)

find_package(BISON REQUIRED)
set_package_properties(BISON PROPERTIES
    URL "http://www.gnu.org/software/bison"
    DESCRIPTION "general-purpose parser generator"
    TYPE REQUIRED
    PURPOSE "Required for the Predicate parser"
)

if (CMAKE_SYSTEM_NAME MATCHES Linux)
    find_package( UDev )

    set_package_properties(UDev PROPERTIES  URL "http://www.kernel.org/pub/linux/utils/kernel/hotplug/udev.html"
                                            DESCRIPTION "UDev support for Solid"
                                            TYPE REQUIRED
                                            PURPOSE "Allows Solid to use UDev to provide information about devices on Linux"
                            )
endif()


include(ECMPoQmTools)


option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX SOLID
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

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5Solid_QCH
        FILE KF5SolidQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5SolidQchTargets.cmake\")")
endif()

configure_package_config_file(
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
