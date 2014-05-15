include(ExternalProject2)

set(libpng_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libpng)

set(libpng_CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
	-DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
	-DCMAKE_PREFIX_PATH=${GY_TARGET_CONTRIB_OUT_DIR}
	-DPNG_SHARED=OFF
	-DBUILD_SHARED_LIBS=FALSE
	-DSKIP_INSTALL_FILES=1
	-Wno-dev
)

ExternalProject_Add(zlib_build
	PREFIX ${libpng_PREFIX}

	DOWNLOAD_DIR ${GY_CONTRIB_DOWNLOAD_DIR}

	GIT_REPOSITORY git://github.com/madler/zlib.git
	GIT_TAG v1.2.8

	INSTALL_DIR ${GY_TARGET_CONTRIB_OUT_DIR}
	CMAKE_ARGS ${libpng_CMAKE_ARGS}
)

ExternalProject_Get_Property(zlib_build install_dir)

if(MINGW)
	set(libpng_CMAKE_ARGS ${libpng_CMAKE_ARGS}
		-DZLIB_LIBRARY=zlib
		-DZLIB_INCLUDE_DIR=${install_dir}/include
	)
endif()


ExternalProject_Add(libpng_build
	DEPENDS zlib_build
	PREFIX ${libpng_PREFIX}

	DOWNLOAD_DIR ${GY_CONTRIB_DOWNLOAD_DIR}

	GIT_REPOSITORY git://git.code.sf.net/p/libpng/code
	GIT_TAG libpng16

	INSTALL_DIR ${GY_TARGET_CONTRIB_OUT_DIR}
	CMAKE_ARGS ${libpng_CMAKE_ARGS}
)

add_library(zlib STATIC IMPORTED)
set_target_properties(zlib PROPERTIES
	IMPORTED_LOCATION ${GY_TARGET_CONTRIB_OUT_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(zlib zlib_build)
add_library(libpng STATIC IMPORTED)
set_target_properties(libpng PROPERTIES
	IMPORTED_LOCATION ${GY_TARGET_CONTRIB_OUT_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}png${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(libpng libpng_build)
