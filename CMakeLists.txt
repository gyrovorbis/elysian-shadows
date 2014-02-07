project(libGyro C)

cmake_minimum_required(VERSION 2.8.8)

#
# Options
#

option(GY_BUILD_EXAMPLES "Build libGyro examples" ON)

#
# Set up some variables
#

set(GY_ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(GY_ASSETS_DIR "${GY_ROOT_DIR}/assets")
set(GY_CONTRIB_DIR "${GY_ROOT_DIR}/contrib")
set(GY_SCRIPTS_DIR "${GY_ROOT_DIR}/scripts")
set(GY_SOURCE_DIR "${GY_ROOT_DIR}/source")
set(GY_INCLUDE_DIR "${GY_ROOT_DIR}/include")
set(GY_OUT_DIR "${GY_ROOT_DIR}/out")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

set(GYRO_PLATFORM_x86      1)
set(GYRO_PLATFORM_DC       2)
set(GYRO_PLATFORM_PSP      4)
set(GYRO_PLATFORM_iOS      8)
set(GYRO_PLATFORM_ANDROID 16)
set(GYRO_PLATFORM_Qt      32)

if(NOT GYRO_PLATFORM)
	set(GYRO_PLATFORM GYRO_PLATFORM_x86)
endif()

list(APPEND CMAKE_MODULE_PATH "${GY_SCRIPTS_DIR}/cmake/modules")

#
# Modules/macros
#

include(GetGitRevisionDescription)
include(GroupByFolder)
include(MergeStaticLibs)
include(TargetArch)

#
# Target Arch/OS detection
#

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(GY_TARGET_OS "darwin")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(GY_TARGET_OS "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(GY_TARGET_OS "windows")
else()
	set(GY_TARGET_OS "${CMAKE_SYSTEM_NAME}")
endif()

target_architecture(GY_TARGET_ARCH)

set(GY_TARGET_OUT_DIR "${GY_OUT_DIR}/${GY_TARGET_OS}_${GY_TARGET_ARCH}")
set(GY_TARGET_CONTRIB_OUT_DIR "${GY_TARGET_OUT_DIR}/contrib")

set(CMAKE_INSTALL_PREFIX ${GY_TARGET_OUT_DIR})

#
# Generate version information from git-describe
#

git_describe(gitdesc)

if("${gitdesc}" EQUAL "GIT-NOTFOUND")
	message(FATAL_ERROR "libGyro must be built within a live Git root")
	return()
endif()
set(gy_VERSION_FULL "${gitdesc}")
if(gy_VERSION_FULL MATCHES "([0-9]+)\\.([0-9]+)\\.([0-9]+)-?([0-9]+-g[0-9a-f]+)?")
	set(gy_VERSION_MAJOR ${CMAKE_MATCH_1})
	set(gy_VERSION_MINOR ${CMAKE_MATCH_2})
	set(gy_VERSION_PATCH ${CMAKE_MATCH_3})
	set(gy_VERSION_EXTRA ${CMAKE_MATCH_4})
endif()

message(STATUS "Building version: ${gy_VERSION_FULL}")
configure_file("${GY_SOURCE_DIR}/gyro_version.c.in" "${CMAKE_CURRENT_BINARY_DIR}/gyro_version.c")

#
# Dependencies
#

find_package(Threads REQUIRED)

include("${GY_CONTRIB_DIR}/CMakeLists.txt")

#
# libGyro
#

add_definitions(-DGYRO_PLATFORM=${GYRO_PLATFORM})
message(STATUS "GYRO_PLATFORM = ${${GYRO_PLATFORM}}")

set(gyro_SOURCES
	${CMAKE_CURRENT_BINARY_DIR}/gyro_version.c
	${GY_SOURCE_DIR}/common/gyro_asset_bank.c
	${GY_SOURCE_DIR}/common/gyro_asset_bank.h
	${GY_SOURCE_DIR}/common/gyro_matrix_stack.c
	${GY_SOURCE_DIR}/common/gyro_shader.c
	${GY_SOURCE_DIR}/common/gyro_system.c
	${GY_SOURCE_DIR}/common/gyro_texture.c
	${GY_SOURCE_DIR}/common/gyro_wav.c

	${GY_SOURCE_DIR}/glal/gyro_audio_glal.c
	${GY_SOURCE_DIR}/glal/gyro_lighting_glal.c
	${GY_SOURCE_DIR}/glal/gyro_ogg_glal.c
	${GY_SOURCE_DIR}/glal/gyro_shaders_glal.c
	${GY_SOURCE_DIR}/glal/gyro_shaders_glal.h
	${GY_SOURCE_DIR}/glal/gyro_video_glal.c
	${GY_SOURCE_DIR}/glal/gyro_video_glal.h
	${GY_SOURCE_DIR}/glal/gyro_wav_glal.c
)

set(gyro_INCLUDES
	${GY_INCLUDE_DIR}/gyro.h
	${GY_INCLUDE_DIR}/gyro_audio_api.h
	${GY_INCLUDE_DIR}/gyro_defines.h
	${GY_INCLUDE_DIR}/gyro_file_api.h
	${GY_INCLUDE_DIR}/gyro_input_api.h
	${GY_INCLUDE_DIR}/gyro_math_api.h
	${GY_INCLUDE_DIR}/gyro_matrix_api.h
	${GY_INCLUDE_DIR}/gyro_ogg_api.h
	${GY_INCLUDE_DIR}/gyro_platform.h
	${GY_INCLUDE_DIR}/gyro_shader_api.h
	${GY_INCLUDE_DIR}/gyro_system_api.h
	${GY_INCLUDE_DIR}/gyro_texture_api.h
	${GY_INCLUDE_DIR}/gyro_timer_api.h
	${GY_INCLUDE_DIR}/gyro_version.h
	${GY_INCLUDE_DIR}/gyro_video_api.h
	${GY_INCLUDE_DIR}/gyro_wav_api.h
	${GY_INCLUDE_DIR}/common/gyro_matrix_stack.h
	${GY_INCLUDE_DIR}/common/gyro_system_common.h
	${GY_INCLUDE_DIR}/common/gyro_texture_common.h
	${GY_INCLUDE_DIR}/common/gyro_video_common.h
	${GY_INCLUDE_DIR}/common/gyro_wav_common.h

	${GY_INCLUDE_DIR}/glal/gyro_audio_glal.inl
	${GY_INCLUDE_DIR}/glal/gyro_glal.h
	${GY_INCLUDE_DIR}/glal/gyro_texture_glal.inl
)

if(GY_TARGET_OS MATCHES "i386" OR GY_TARGET_ARCH MATCHES "x86_64")
	set(gyro_SOURCES
		${gyro_SOURCES}
		${GY_SOURCE_DIR}/x86/gyro_input_x86.c
		${GY_SOURCE_DIR}/x86/gyro_matrix_x86.c
		${GY_SOURCE_DIR}/x86/gyro_system_x86.c
		${GY_SOURCE_DIR}/x86/gyro_texture_x86.c
		${GY_SOURCE_DIR}/x86/gyro_timer_x86.c
		${GY_SOURCE_DIR}/x86/gyro_video_x86.c
	)
	set(gyro_INCLUDES
		${gyro_INCLUDES}
		${GY_INCLUDE_DIR}/x86/gyro_input_x86.inl
		${GY_INCLUDE_DIR}/x86/gyro_math_sse_x86.inl
		${GY_INCLUDE_DIR}/x86/gyro_matrix_sse_x86.inl
		${GY_INCLUDE_DIR}/x86/gyro_sdl_x86.inl
		${GY_INCLUDE_DIR}/x86/gyro_video_x86.inl
	)
endif()

include_directories(${GY_INCLUDE_DIR})
include_directories(SYSTEM "${GY_TARGET_CONTRIB_OUT_DIR}/include")
include_directories(SYSTEM "${GY_TARGET_OUT_DIR}/include")

link_directories("${GY_TARGET_CONTRIB_OUT_DIR}/lib")
link_directories("${GY_TARGET_OUT_DIR}/lib")

add_library(Gyro STATIC ${gyro_SOURCES} ${gyro_INCLUDES})
add_dependencies(Gyro
	glew
	glfw
	libpng
	openal
	ogg
	vorbis
	zlib
)
set_target_properties(Gyro PROPERTIES
	OUTPUT_NAME Gyro_nodeps
)

merge_static_libs(Gyro_combined OUTPUT_NAME Gyro
	Gyro
	glew
	glfw
	libpng
	openal
	ogg
	vorbis
	zlib
)

#
# "FindGyro"
#

find_library(Gyro_LIBRARY Gyro
	PATHS @CMAKE_INSTALL_PREFIX@/lib
	NO_DEFAULT_PATH
)

set(Gyro_LIBRARIES ${Gyro_LIBRARY})
set(Gyro_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
set(Gyro_DEFINITIONS -DGYRO_PLATFORM=${GYRO_PLATFORM})

if(${GYRO_PLATFORM} EQUAL GYRO_PLATFORM_x86)
	find_package(OpenGL REQUIRED)
	list(APPEND Gyro_LIBRARIES ${OPENGL_gl_LIBRARY})
	if(APPLE)
		find_library(COCOA_FRAMEWORK Cocoa)
		find_library(IOKIT_FRAMEWORK IOKit)
		find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
		find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
		list(APPEND Gyro_LIBRARIES
			${COCOA_FRAMEWORK}
			${IOKIT_FRAMEWORK}
			${CORE_FOUNDATION_FRAMEWORK}
			${CORE_VIDEO_FRAMEWORK}
		)
	elseif(UNIX)
		find_package(X11 REQUIRED)
		list(APPEND Gyro_LIBRARIES
			${X11_X11_LIB}
			${CMAKE_THREAD_LIBS_INIT}
#			${RT_LIBRARY}
			${X11_Xrandr_LIB}
			m
		)
		if(X11_Xinput_LIB)
			list(APPEND Gyro_LIBRARIES ${X11_Xinput_LIB})
		else()
			list(APPEND Gyro_LIBRARIES Xi)
		endif()
		if(X11_Xxf86vm_LIB)
			list(APPEND Gyro_LIBRARIES ${X11_Xxf86vm_LIB})
		else()
			list(APPEND Gyro_LIBRARIES Xxf86vm)
		endif()
	endif()
endif()

if(GY_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

configure_file(${GY_ROOT_DIR}/GyroConfig.cmake.in
	${libGyro_BINARY_DIR}/GyroConfig.cmake @ONLY)

install(TARGETS Gyro_combined
    ARCHIVE DESTINATION lib)
install(FILES ${gyro_INCLUDES}
	DESTINATION include/gyro)

install(FILES ${libGyro_BINARY_DIR}/GyroConfig.cmake
	DESTINATION lib${LIB_SUFFIX}/cmake/gyro)

group_by_folder(${GY_ROOT_DIR} "${gyro_SOURCES}")
group_by_folder(${GY_ROOT_DIR} "${gyro_INCLUDES}")
