# Finds libmad include file and library. This module sets the following
# variables:
#
#  LibMad_FOUND       - Flag if libmad was found
#  LibMad_INCLUDE_DIR - libmad include directory
#  LobMad_LIBRARY     - libmad library path

include(FindPackageHandleStandardArgs)

find_path(LibMad_INCLUDE_DIR mad.h)
find_library(LibMad_LIBRARY mad)

set(LibMad_INCLUDE_DIRS ${LibMad_INCLUDE_DIR})
set(LibMad_LIBRARIES ${LibMad_LIBRARY})

find_package_handle_standard_args(
    LibMad
    DEFAULT_MSG
	LibMad_LIBRARY
	LibMad_INCLUDE_DIR
)
