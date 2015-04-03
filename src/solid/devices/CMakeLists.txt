include (CheckCXXSourceCompiles)

if(WIN32)
    add_definitions(-DYY_NO_UNISTD_H)
endif()

if(MSVC OR (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    check_cxx_source_compiles("int main() { __asm { pxor mm0, mm0 }; }" HAVE_X86_MMX)
    check_cxx_source_compiles("int main() { __asm { xorps xmm0, xmm0 }; }" HAVE_X86_SSE)
    check_cxx_source_compiles("int main() { __asm { xorpd xmm0, xmm0 }; }" HAVE_X86_SSE2)
    check_cxx_source_compiles("int main() { __asm { femms }; }" HAVE_X86_3DNOW)
else()
    check_cxx_source_compiles("     #ifdef __SUNPRO_CC
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

configure_file(devices/config-processor.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-processor.h )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/devices/ ${CMAKE_CURRENT_SOURCE_DIR}/devices/frontend/ ${CMAKE_CURRENT_BINARY_DIR})
set(solid_LIB_SRCS
    ${solid_LIB_SRCS}
    devices/managerbase.cpp
    devices/solidnamespace.cpp
    devices/predicateparse.cpp
    devices/predicate_lexer.c
    devices/predicate_parser.c

    devices/frontend/device.cpp
    devices/frontend/devicemanager.cpp
    devices/frontend/deviceinterface.cpp
    devices/frontend/genericinterface.cpp
    devices/frontend/processor.cpp
    devices/frontend/block.cpp
    devices/frontend/storagedrive.cpp
    devices/frontend/opticaldrive.cpp
    devices/frontend/storagevolume.cpp
    devices/frontend/opticaldisc.cpp
    devices/frontend/storageaccess.cpp
    devices/frontend/camera.cpp
    devices/frontend/portablemediaplayer.cpp
    devices/frontend/networkshare.cpp
    devices/frontend/battery.cpp
    devices/frontend/predicate.cpp

    devices/ifaces/battery.cpp
    devices/ifaces/block.cpp
    devices/ifaces/camera.cpp
    devices/ifaces/opticaldrive.cpp
    devices/ifaces/device.cpp
    devices/ifaces/deviceinterface.cpp
    devices/ifaces/devicemanager.cpp
    devices/ifaces/genericinterface.cpp
    devices/ifaces/networkshare.cpp
    devices/ifaces/opticaldisc.cpp
    devices/ifaces/portablemediaplayer.cpp
    devices/ifaces/processor.cpp
    devices/ifaces/storagedrive.cpp
    devices/ifaces/storagevolume.cpp
    devices/ifaces/storageaccess.cpp

    devices/backends/shared/rootdevice.cpp
    devices/backends/shared/cpufeatures.cpp
)

include(devices/backends/fakehw/CMakeLists.txt)

if(NOT WIN32 AND NOT APPLE)
   if ( UDEV_FOUND )
      message(STATUS "Building Solid UDev backend." )
      include(devices/backends/udev/CMakeLists.txt)
   endif()

   message(STATUS "Building Solid UPower backend." )
   include(devices/backends/upower/CMakeLists.txt)

   # FIXME: this should work on more Unix systems
   if (CMAKE_SYSTEM_NAME MATCHES Linux AND UDEV_FOUND)
        message(STATUS "Building Solid UDisks2 backend." )
        include(devices/backends/udisks2/CMakeLists.txt)
   endif ()

   message(STATUS "Building Solid fstab backend." )
   include(devices/backends/fstab/CMakeLists.txt)

    if (NOT CMAKE_SYSTEM_NAME MATCHES Linux)
        message(STATUS "Building Solid HAL backend." )
        include(devices/backends/hal/CMakeLists.txt)
    endif()

    message(STATUS "Building KDE Connect backend." )
    include(devices/backends/kdeconnect/CMakeLists.txt)
endif()

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckCXXSourceCompiles)

#Needed for the fstab backend
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

configure_file(devices/config-solid.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-solid.h )

if(APPLE)
   find_package(IOKit REQUIRED)

   message(STATUS "-- Building Solid IOKit backend." )
   include(devices/backends/iokit/CMakeLists.txt)
endif()

if(WIN32)
    message(STATUS "-- Building Solid Windows backend." )
    include(devices/backends/win/CMakeLists.txt)
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
