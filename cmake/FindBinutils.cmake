find_library(BINUTILS_LIBRARY
    NAMES
        libbfd bfd
    PATHS
        /usr/
        /usr/bin/
        /usr/include/
        /usr/lib/
        /usr/local/
        /usr/local/bin/
        /opt/)

find_path(BINUTILS_INCLUDE_DIR
    NAMES
        bfd.h
    PATHS
        /usr/
        /usr/include/
        /usr/local/
        /usr/local/include/
        /opt/
    PATH_SUFFIXES
        include)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Binutils DEFAULT_MSG BINUTILS_LIBRARY BINUTILS_INCLUDE_DIR)

message(STATUS "Binutils_FOUND: ${BINUTILS_FOUND}")
message(STATUS "Binutils_LIBRARY: ${BINUTILS_LIBRARY}")
message(STATUS "Binutils_INCLUDE_DIR: ${BINUTILS_INCLUDE_DIR}")
