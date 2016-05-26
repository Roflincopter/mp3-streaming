# Finds Portaudio include file and library. This module sets the following
# variables:
#
#  Portaudio_FOUND       - Flag if libmad was found
#  Portaudio_INCLUDE_DIR - libmad include directory
#  Portaudio_LIBRARY     - libmad library path

include(FindPackageHandleStandardArgs)

find_path(Portaudio_INCLUDE_DIR portaudio.h)
find_library(Portaudio_LIBRARY portaudio)

set(Portaudio_INCLUDE_DIRS ${Portaudio_INCLUDE_DIR})
set(Portaudio_LIBRARIES ${Portaudio_LIBRARY})

find_package_handle_standard_args(
	Portaudio
    DEFAULT_MSG
	Portaudio_LIBRARY
	Portaudio_INCLUDE_DIR
)

