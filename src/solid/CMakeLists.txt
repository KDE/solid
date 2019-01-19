ecm_create_qm_loader(solid_QM_LOADER solid5_qt)

include(devices/CMakeLists.txt)

if (WITH_NEW_SOLID_JOB)
    include(power/CMakeLists.txt)
endif()

set(solid_LIB_SRCS ${solid_LIB_SRCS} ${solid_QM_LOADER})

add_library(KF5Solid  ${solid_LIB_SRCS})
target_include_directories(KF5Solid PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}/..;${CMAKE_CURRENT_BINARY_DIR}/..>")
generate_export_header(KF5Solid BASE_NAME Solid)
add_library(KF5::Solid ALIAS KF5Solid)

# Apps must include <Solid/File> or <solid/file.h>
target_include_directories(KF5Solid INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/Solid>")

target_link_libraries(KF5Solid PUBLIC Qt5::Core
                               PRIVATE Qt5::Xml
                                       Qt5::Widgets
                                       ${solid_OPTIONAL_LIBS}
 )
if (TARGET Qt5::DBus)
  target_link_libraries(KF5Solid PRIVATE Qt5::DBus)
endif()

set_target_properties(KF5Solid PROPERTIES  VERSION ${SOLID_VERSION_STRING}
                                           SOVERSION ${SOLID_SOVERSION}
                                           EXPORT_NAME Solid
)

ecm_generate_headers(Solid_CamelCase_HEADERS
  HEADER_NAMES
  Device
  DeviceNotifier
  DeviceInterface
  GenericInterface
  Processor
  Block
  StorageAccess
  StorageDrive
  OpticalDrive
  StorageVolume
  OpticalDisc
  Camera
  PortableMediaPlayer
  Battery
  Predicate
  NetworkShare
  SolidNamespace

  RELATIVE devices/frontend
  REQUIRED_HEADERS Solid_HEADERS
  PREFIX Solid
)

if (WITH_NEW_SOLID_JOB)
    ecm_generate_headers(Solid_CamelCase_HEADERS
    HEADER_NAMES
    Job

    RELATIVE power/
    REQUIRED_HEADERS Solid_HEADERS
    PREFIX Solid
    )
endif()

if(WITH_NEW_SOLID_JOB AND WITH_NEW_POWER_ASYNC_API)
    ecm_generate_headers(Solid_CamelCase_HEADERS
        HEADER_NAMES
        AcPluggedJob
        InhibitionJob
        Inhibition
        Power
        StatesJob
        RequestStateJob

        RELATIVE power/
        REQUIRED_HEADERS Solid_HEADERS
        PREFIX Solid
    )
endif()

install(FILES ${Solid_CamelCase_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/Solid/Solid COMPONENT Devel)

install(TARGETS KF5Solid EXPORT KF5SolidTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

########### static lib for tests  ###############

add_library(KF5Solid_static STATIC ${solid_LIB_SRCS})
set_target_properties(KF5Solid_static PROPERTIES COMPILE_FLAGS -DSOLID_STATIC_DEFINE=1)

target_link_libraries(KF5Solid_static PUBLIC Qt5::Core)
target_link_libraries(KF5Solid_static PRIVATE Qt5::Xml Qt5::Widgets ${solid_OPTIONAL_LIBS})
if (TARGET Qt5::DBus)
  target_link_libraries(KF5Solid_static PRIVATE Qt5::DBus)
endif()
target_include_directories(KF5Solid_static PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR};${CMAKE_CURRENT_SOURCE_DIR}/..;${CMAKE_CURRENT_BINARY_DIR}/..>")

########### install files ###############

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/solid_export.h
  ${Solid_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/Solid/solid COMPONENT Devel
)

if(BUILD_QCH)
    ecm_add_qch(
        KF5Solid_QCH
        NAME Solid
        BASE_NAME KF5Solid
        VERSION ${KF5_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${Solid_HEADERS}
            "${CMAKE_SOURCE_DIR}/docs/song.md"
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
        BLANK_MACROS
            SOLID_EXPORT
            SOLID_DEPRECATED
            SOLID_DEPRECATED_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME Solid LIB_NAME KF5Solid DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/Solid)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
