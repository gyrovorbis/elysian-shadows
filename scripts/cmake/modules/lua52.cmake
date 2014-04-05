project(lua52 C)

cmake_minimum_required(VERSION 2.8.8)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(lua52_SRC_CORE
	src/lapi.c
	src/lcode.c
	src/lctype.c
	src/ldebug.c
	src/ldo.c
	src/ldump.c
	src/lfunc.c
	src/lgc.c
	src/llex.c
	src/lmem.c
	src/lobject.c
	src/lopcodes.c
	src/lparser.c
	src/lstate.c
	src/lstring.c
	src/ltable.c
	src/ltm.c
	src/lundump.c
	src/lvm.c
	src/lzio.c
)

set(lua52_SRC_LIB
	src/lauxlib.c
	src/lbaselib.c
	src/lbitlib.c
	src/lcorolib.c
	src/ldblib.c
	src/liolib.c
	src/lmathlib.c
	src/loslib.c
	src/lstrlib.c
	src/ltablib.c
	src/linit.c
	src/loadlib.c
)

set(lua52_SRC_LUA
	src/lua.c
)

set(lua52_SRC_LUAC
	src/luac.c
)

include_directories(src)

if(NOT MINGW AND NOT MSVC)
  add_definitions(-DLUA_USE_POSIX -DLUA_USE_DLOPEN)
endif()

add_library(lua52 STATIC ${lua52_SRC_CORE} ${lua52_SRC_LIB})

install(TARGETS lua52 EXPORT lua52 ARCHIVE DESTINATION lib)
install(FILES src/lua.h src/luaconf.h src/lualib.h src/lauxlib.h src/lua.hpp
	DESTINATION include/lua5.2)
