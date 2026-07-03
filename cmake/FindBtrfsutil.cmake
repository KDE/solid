find_package(PkgConfig QUIET)
pkg_search_module(PC_libbtrfsutil QUIET libbtrfsutil)

find_path(Btrfsutil_INCLUDE_DIRS NAMES btrfsutil.h HINTS ${PC_libbtrfsutil_INCLUDE_DIRS})
find_library(Btrfsutil_LIBRARIES NAMES btrfsutil HINTS ${PC_libbtrfsutil_LIBRARY_DIRS})

set(Btrfsutil_VERSION ${PC_libbtrfsutil_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Btrfsutil
    FOUND_VAR Btrfsutil_FOUND
    REQUIRED_VARS Btrfsutil_INCLUDE_DIRS Btrfsutil_LIBRARIES
    VERSION_VAR Btrfsutil_VERSION
)

if (Btrfsutil_FOUND AND NOT TARGET Btrfsutil::Btrfsutil)
    add_library(Btrfsutil::Btrfsutil UNKNOWN IMPORTED)
    set_target_properties(Btrfsutil::Btrfsutil PROPERTIES
        IMPORTED_LOCATION "${Btrfsutil_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Btrfsutil_INCLUDE_DIRS}"
        INTERFACE_COMPILE_OPTIONS "${PC_libbtrfsutil_CFLAGS_OTHER}"
    )
endif()

include(FeatureSummary)
set_package_properties(Btrfsutil PROPERTIES
    DESCRIPTION "library for managing Btrfs filesystems"
    URL "https://github.com/kdave/btrfs-progs/"
)
