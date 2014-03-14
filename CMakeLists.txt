project(ESDev C CXX)

cmake_minimum_required(VERSION 2.8.8)

#
# Set up some variables
#

set(ES_ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(ES_ASSETS_DIR "${ES_ROOT_DIR}/assets")
set(ES_CONTRIB_DIR "${ES_ROOT_DIR}/contrib")
set(ES_SCRIPTS_DIR "${ES_ROOT_DIR}/scripts")
set(ES_OUT_DIR "${ES_ROOT_DIR}/out")

set(ES_GYRO_DIR "${ES_ROOT_DIR}/libGyro")
set(ES_ASSETIO_DIR "${ES_ROOT_DIR}/AssetIO")
set(ES_OOLUA_DIR "${ES_ROOT_DIR}/OOLua")
set(ES_GAMMA_DIR "${ES_ROOT_DIR}/ESGamma")
set(ES_ESTK_DIR "${ES_ROOT_DIR}/ESToolkit")

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

list(APPEND CMAKE_MODULE_PATH "${ES_GYRO_DIR}/scripts/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${ES_SCRIPTS_DIR}/cmake/modules")

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
	set(ES_TARGET_OS "darwin")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(ES_TARGET_OS "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(ES_TARGET_OS "windows")
else()
	set(ES_TARGET_OS "${CMAKE_SYSTEM_NAME}")
endif()

target_architecture(ES_TARGET_ARCH)

set(ES_TARGET_OUT_DIR "${ES_OUT_DIR}/${ES_TARGET_OS}_${ES_TARGET_ARCH}")
set(ES_TARGET_CONTRIB_OUT_DIR "${ES_TARGET_OUT_DIR}/contrib")

set(CMAKE_INSTALL_PREFIX ${ES_TARGET_OUT_DIR})

#
# Generate version information from git-describe
#

git_describe(gitdesc)

if("${gitdesc}" EQUAL "GIT-NOTFOUND")
	message(FATAL_ERROR "ESDev must be built within a live Git root")
	return()
endif()
set(es_VERSION_FULL "${gitdesc}")
if(es_VERSION_FULL MATCHES "([0-9]+)\\.([0-9]+)\\.([0-9]+)-?([0-9]+-g[0-9a-f]+)?")
	set(es_VERSION_MAJOR ${CMAKE_MATCH_1})
	set(es_VERSION_MINOR ${CMAKE_MATCH_2})
	set(es_VERSION_PATCH ${CMAKE_MATCH_3})
	set(es_VERSION_EXTRA ${CMAKE_MATCH_4})
endif()

message(STATUS "Building version: ${es_VERSION_FULL}")

#
# Include/link directories
#

include_directories(SYSTEM "${ES_TARGET_CONTRIB_OUT_DIR}/include")
include_directories(SYSTEM "${ES_TARGET_OUT_DIR}/include")
include_directories(SYSTEM "${ES_TARGET_OUT_DIR}/include/oolua")

link_directories("${ES_TARGET_CONTRIB_OUT_DIR}/lib")
link_directories("${ES_TARGET_OUT_DIR}/lib")

#
# Dependencies
#

find_package(Threads REQUIRED)

include("${ES_CONTRIB_DIR}/CMakeLists.txt")

# libGyro
add_subdirectory(libGyro)

# OOLua
add_subdirectory(OOLua)
add_dependencies(OOLua lua52)
