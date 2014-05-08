ecm_create_qm_loader(solid_QM_LOADER solid5_qt)

include(devices/CMakeLists.txt)

if (WITH_NEW_SOLID_JOB)
    include(power/CMakeLists.txt)
endif()

set(solid_LIB_SRCS ${solid_LIB_SRCS} ${solid_QM_LOADER})

add_library(KF5Solid  ${solid_LIB_SRCS})
target_include_directories(KF5Solid PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..;${CMAKE_CURRENT_BINARY_DIR}/..>")
generate_export_header(KF5Solid BASE_NAME Solid)
add_library(KF5::Solid ALIAS KF5Solid)

# Apps must include <Solid/File> or <solid/file.h>
target_include_directories(KF5Solid INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/Solid>")

target_link_libraries(KF5Solid PUBLIC Qt5::Core
                               PRIVATE Qt5::DBus
                                       Qt5::Xml
                                       Qt5::Widgets
                                       ${solid_OPTIONAL_LIBS}
 )

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
install(FILES ${Solid_CamelCase_HEADERS} DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/Solid/Solid COMPONENT Devel)

install(TARGETS KF5Solid EXPORT KF5SolidTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

########### static lib for tests  ###############

add_library(KF5Solid_static STATIC ${solid_LIB_SRCS})
set_target_properties(KF5Solid_static PROPERTIES COMPILE_FLAGS -DSOLID_STATIC_DEFINE=1)

target_link_libraries(KF5Solid_static PUBLIC Qt5::Core)
target_link_libraries(KF5Solid_static PRIVATE Qt5::DBus Qt5::Xml Qt5::Widgets ${solid_OPTIONAL_LIBS})
target_include_directories(KF5Solid_static PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..;${CMAKE_CURRENT_BINARY_DIR}/..>")

########### install files ###############

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/solid_export.h
  ${Solid_HEADERS}
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/Solid/solid COMPONENT Devel
)

########### parser build ###############

set(lexer_FILE predicate_lexer)
set(parser_FILE predicate_parser)

find_package(FLEX)
set_package_properties(FLEX PROPERTIES DESCRIPTION "Allows the Solid predicate parser to be updated"
                       URL "http://flex.sourceforge.net"
                       TYPE OPTIONAL
                       PURPOSE "Required by the UpdateSolidPredicateParser target (mainly useful for developers)"
                      )

find_package(BISON)
set_package_properties(BISON PROPERTIES
    URL "http://www.gnu.org/software/bison"
    DESCRIPTION "Allows the Solid predicate parser to be updated"
    TYPE OPTIONAL
    PURPOSE "Required by the UpdateSolidPredicateParser target (mainly useful for developers)"
  )

if (FLEX_FOUND AND BISON_FOUND)

   add_custom_target(UpdateSolidPredicateParser
                     COMMAND ${FLEX_EXECUTABLE} -P Solid -o${lexer_FILE}.c ${lexer_FILE}.l
                     COMMAND ${BISON_EXECUTABLE} -p Solid -d -b ${parser_FILE} ${parser_FILE}.y
                     COMMAND ${CMAKE_COMMAND} -E copy ${parser_FILE}.tab.c ${CMAKE_CURRENT_SOURCE_DIR}/${parser_FILE}.c
                     COMMAND ${CMAKE_COMMAND} -E copy ${parser_FILE}.tab.h ${CMAKE_CURRENT_SOURCE_DIR}/${parser_FILE}.h
                     COMMAND ${CMAKE_COMMAND} -E remove ${parser_FILE}.tab.c ${parser_FILE}.tab.h
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

else ()
   add_custom_target(UpdateSolidPredicateParser
                     COMMAND echo "flex and/or bison not found, so target UpdateSolidPredicateParser inactive")
endif ()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME Solid LIB_NAME KF5Solid DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KF5_INCLUDE_INSTALL_DIR}/Solid)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
