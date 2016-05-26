# Finds Taglib include file and library. This module sets the following
# variables:
#
#  Taglib_FOUND       - Flag if libmad was found
#  Taglib_INCLUDE_DIR - libmad include directory
#  Taglib_LIBRARY     - libmad library path

include(FindPackageHandleStandardArgs)

find_path(Taglib_INCLUDE_DIR tag.h PATH_SUFFIXES taglib)
find_library(Taglib_LIBRARY tag)

set(Taglib_INCLUDE_DIRS ${Taglib_INCLUDE_DIR})
set(Taglib_LIBRARIES ${Taglib_LIBRARY})

find_package_handle_standard_args(
	Taglib
    DEFAULT_MSG
	Taglib_LIBRARY
	Taglib_INCLUDE_DIR
)
