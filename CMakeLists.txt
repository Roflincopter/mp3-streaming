cmake_minimum_required(VERSION 3.5.2)

project(mp3_streaming)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

find_package(LibMad REQUIRED)
find_package(Portaudio REQUIRED)
find_package(Taglib REQUIRED)

add_executable(mp3_streaming
	main.cpp
)

target_compile_options(mp3_streaming
	PRIVATE "-std=c++14"
	PRIVATE "-g"
)

target_include_directories(mp3_streaming
	PRIVATE /usr/include/
	PRIVATE ${LibMad_INCLUDE_DIRS}
	PRIVATE ${Portaudio_INCLUDE_DIRS}
	PRIVATE ${Taglib_INCLUDE_DIRS}
)

target_link_libraries(mp3_streaming
	PRIVATE ${LibMad_LIBRARIES}
	PRIVATE ${Portaudio_LIBRARIES}
	PRIVATE ${Taglib_LIBRARIES}
)
