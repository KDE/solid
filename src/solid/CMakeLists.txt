include (CheckCXXSourceCompiles)

if(MSVC OR (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    check_cxx_source_compiles("int main() { __asm { pxor mm0, mm0 }; }" HAVE_X86_MMX)
    check_cxx_source_compiles("int main() { __asm { xorps xmm0, xmm0 }; }" HAVE_X86_SSE)
    check_cxx_source_compiles("int main() { __asm { xorpd xmm0, xmm0 }; }" HAVE_X86_SSE2)
    check_cxx_source_compiles("int main() { __asm { femms }; }" HAVE_X86_3DNOW)
else()
    check_cxx_source_compiles("	    #ifdef __SUNPRO_CC
                                            #define __asm__ asm
                                    #endif
                    int main() { __asm__(\"pxor %mm0, %mm0\") ; }" HAVE_X86_MMX)
    check_cxx_source_compiles("     #ifdef __SUNPRO_CC
                                            #define __asm__ asm
                                    #endif
                    int main() { __asm__(\"xorps %xmm0, %xmm0\"); }" HAVE_X86_SSE)
    check_cxx_source_compiles("     #ifdef __SUNPRO_CC
                                            #define __asm__ asm
                                    #endif
                    int main() { __asm__(\"xorpd %xmm0, %xmm0\"); }" HAVE_X86_SSE2)
    check_cxx_source_compiles("     #ifdef __SUNPRO_CC
                                            #define __asm__ asm
                                    #endif
                    int main() { __asm__(\"femms\"); }" HAVE_X86_3DNOW)
endif()
check_cxx_source_compiles("     #ifdef __SUNPRO_CC
                                        #define __asm__ asm
                                #endif
				int main() { __asm__(\"mtspr 256, %0; vand %%v0, %%v0, %%v0\" : : \"r\"(-1) ); }" HAVE_PPC_ALTIVEC)

configure_file(config-processor.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-processor.h )



########### next target ###############

if(WIN32)
    add_definitions(-DYY_NO_UNISTD_H)
endif()

ecm_create_qm_loader(solid_QM_LOADER solid5_qt)

set(solid_LIB_SRCS
   solidnamespace.cpp
   managerbase.cpp
   device.cpp
   devicemanager.cpp
   deviceinterface.cpp
   genericinterface.cpp
   processor.cpp
   block.cpp
   storagedrive.cpp
   opticaldrive.cpp
   storagevolume.cpp
   opticaldisc.cpp
   storageaccess.cpp
   camera.cpp
   portablemediaplayer.cpp
   networkshare.cpp
   battery.cpp
   predicate.cpp
   predicateparse.cpp
   predicate_lexer.c
   predicate_parser.c

   ifaces/battery.cpp
   ifaces/block.cpp
   ifaces/camera.cpp
   ifaces/opticaldrive.cpp
   ifaces/device.cpp
   ifaces/deviceinterface.cpp
   ifaces/devicemanager.cpp
   ifaces/genericinterface.cpp
   ifaces/networkshare.cpp
   ifaces/opticaldisc.cpp
   ifaces/portablemediaplayer.cpp
   ifaces/processor.cpp
   ifaces/storagedrive.cpp
   ifaces/storagevolume.cpp
   ifaces/storageaccess.cpp

   backends/shared/rootdevice.cpp
   backends/shared/cpufeatures.cpp
   ${solid_QM_LOADER}
)

include(backends/fakehw/CMakeLists.txt)

if(NOT WIN32 AND NOT APPLE)
   if ( UDEV_FOUND )
      message(STATUS "Building Solid UDev backend." )
      include(backends/udev/CMakeLists.txt)
      set(UDEV_DETAILED_OUTPUT OFF CACHE BOOL "provide extended output regarding udev events")
      if(UDEV_DETAILED_OUTPUT)
         add_definitions(-DUDEV_DETAILED_OUTPUT)
      endif()

      # check for media-player-info (runtime-only optional dependency)
      find_package(MediaPlayerInfo)
      set_package_properties(MediaPlayerInfo PROPERTIES
         DESCRIPTION "Enables identification and querying of portable media players"
         PURPOSE "Runtime-only dependency of the udev solid backend. Support for m-p-i is included even if not found during build"
         URL "http://www.freedesktop.org/wiki/Software/media-player-info"
         TYPE RUNTIME
      )
   endif()


   message(STATUS "Building Solid HAL backend." )
   include(backends/hal/CMakeLists.txt)

   message(STATUS "Building Solid UPower backend." )
   include(backends/upower/CMakeLists.txt)

   # FIXME: this should work on more Unix systems
   if (CMAKE_SYSTEM_NAME MATCHES Linux AND UDEV_FOUND)
        message(STATUS "Building Solid UDisks2 backend." )
        include(backends/udisks2/CMakeLists.txt)
   endif ()

   message(STATUS "Building Solid fstab backend." )
   include(backends/fstab/CMakeLists.txt)

endif()

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckCXXSourceCompiles)

check_include_files(mntent.h      HAVE_MNTENT_H)
check_include_files(sys/types.h   HAVE_SYS_TYPES_H)
check_include_files(sys/param.h   HAVE_SYS_PARAM_H)
check_include_files("stdio.h;sys/mnttab.h"  HAVE_SYS_MNTTAB_H)
check_include_files("sys/param.h;sys/mount.h"  HAVE_SYS_MOUNT_H)

check_function_exists(getmntinfo HAVE_GETMNTINFO)

check_function_exists(setmntent  HAVE_SETMNTENT)

check_cxx_source_compiles("
  #include <sys/types.h>
  #include <sys/statvfs.h>
  int main(){
    struct statvfs *mntbufp;
    int flags;
    return getmntinfo(&mntbufp, flags);
  }
" GETMNTINFO_USES_STATVFS )

configure_file( config-solid.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-solid.h )

if(APPLE)
   find_package(IOKit REQUIRED)

   message(STATUS "-- Building Solid IOKit backend." )
   include(backends/iokit/CMakeLists.txt)
endif()

if(WIN32)
    message(STATUS "-- Building Solid Windows backend." )
    include(backends/win/CMakeLists.txt)
endif()

set(solid_OPTIONAL_LIBS)

if(WIN32)
    set(solid_OPTIONAL_LIBS ${solid_OPTIONAL_LIBS} Qt5::Network setupapi)
    set(solid_OPTIONAL_LIBS ${solid_OPTIONAL_LIBS} setupapi)
endif()

if(APPLE)
   set(solid_OPTIONAL_LIBS ${IOKIT_LIBRARY})
endif()

if ( UDEV_FOUND )
   set(solid_OPTIONAL_LIBS ${solid_OPTIONAL_LIBS} ${UDEV_LIBS})
endif ()

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

  REQUIRED_HEADERS Solid_HEADERS
  PREFIX Solid
)
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
  solidnamespace.h
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
