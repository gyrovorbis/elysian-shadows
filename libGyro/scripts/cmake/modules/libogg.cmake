cmake_minimum_required(VERSION 2.6)

project(libogg C)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
	message("No CMAKE_BUILD_TYPE specified, defaulting to ${CMAKE_BUILD_TYPE}")
endif()

set(libogg_SRCS src/bitwise.c src/framing.c)
set(libogg_HDRS include/ogg/ogg.h include/ogg/os_types.h)

include_directories(include)

if(MSVC)
	add_definitions(/D_UNICODE /DUNICODE)
	list(APPEND libogg_SRCS win32/ogg.def)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	list(APPEND libogg_HDRS include/ogg/config_types.h)

	add_custom_command(OUTPUT include/ogg/config_types.h
						COMMAND ./configure
						DEPENDS include/ogg/ogg.h
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
					   )
endif()

add_library(ogg STATIC ${libogg_SRCS} ${libogg_HDRS})

install(TARGETS ogg
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)

install(FILES ${libogg_HDRS} DESTINATION include/ogg)
