project(libGyro_contrib)

cmake_minimum_required(VERSION 2.8.8)

#
# Set up some variables
#

set(GY_ROOT_DIR "${CMAKE_SOURCE_DIR}/..")
set(GY_CONTRIB_DIR "${GY_ROOT_DIR}/contrib")
set(GY_SCRIPTS_DIR "${GY_ROOT_DIR}/scripts")
set(GY_SOURCE_DIR "${GY_ROOT_DIR}/src")
set(GY_INCLUDE_DIR "${GY_ROOT_DIR}/include")
set(GY_OUT_DIR "${GY_ROOT_DIR}/out")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

list(APPEND CMAKE_MODULE_PATH "${GY_SCRIPTS_DIR}/cmake/modules")

if(NOT GY_CONTRIB_DOWNLOAD_DIR)
	set(GY_CONTRIB_DOWNLOAD_DIR ${GY_CONTRIB_DIR}/downloads)
endif()

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

include(ExternalGLEW)
include(ExternalGLFW)
include(ExternalOpenAL)
include(ExternalPNG)
include(ExternalOggVorbis)
