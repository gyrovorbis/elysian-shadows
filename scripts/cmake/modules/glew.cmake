project(glew C)

cmake_minimum_required(VERSION 2.8.8)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(glew_SRCS src/glew.c)

include_directories(include)

add_library(glew STATIC ${glew_SRCS})

install(TARGETS glew EXPORT glew ARCHIVE DESTINATION lib)
install(FILES include/GL/glew.h include/GL/glxew.h include/GL/wglew.h
	DESTINATION include/GL)
