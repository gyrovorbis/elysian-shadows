include(ExternalProject2)

set(oggvorbis_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/oggvorbis)

set(oggvorbis_CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
	-DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
	-DCMAKE_PREFIX_PATH=${GY_TARGET_CONTRIB_OUT_DIR}
	-Wno-dev
)

ExternalProject_Add(libogg_build
	PREFIX ${oggvorbis_PREFIX}

	DOWNLOAD_DIR ${GY_CONTRIB_DOWNLOAD_DIR}

	URL http://downloads.xiph.org/releases/ogg/libogg-1.3.1.tar.gz
	URL_MD5 ba526cd8f4403a5d351a9efaa8608fbc

	PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GY_SCRIPTS_DIR}/cmake/modules/libogg.cmake <SOURCE_DIR>/CMakeLists.txt

	INSTALL_DIR ${GY_TARGET_CONTRIB_OUT_DIR}
	CMAKE_ARGS ${oggvorbis_CMAKE_ARGS}
)

ExternalProject_Get_Property(libogg_build install_dir)

if(MINGW)
	set(oggvorbis_CMAKE_ARGS ${oggvorbis_CMAKE_ARGS}
		-DOGG_LIBRARY=ogg
		-DOGG_INCLUDE_DIR=${install_dir}/include
	)
endif()

ExternalProject_Add(libvorbis_build
	DEPENDS libogg_build
	PREFIX ${oggvorbis_PREFIX}

	DOWNLOAD_DIR ${GY_CONTRIB_DOWNLOAD_DIR}

	URL http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.3.tar.gz
	URL_MD5 6b1a36f0d72332fae5130688e65efe1f

	PATCH_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GY_SCRIPTS_DIR}/cmake/modules/libvorbis.cmake <SOURCE_DIR>/CMakeLists.txt

	INSTALL_DIR ${GY_TARGET_CONTRIB_OUT_DIR}
	CMAKE_ARGS ${oggvorbis_CMAKE_ARGS}
)

add_library(ogg STATIC IMPORTED)
set_target_properties(ogg PROPERTIES
	IMPORTED_LOCATION ${GY_TARGET_CONTRIB_OUT_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ogg${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(ogg libogg_build)
add_library(vorbis STATIC IMPORTED)
set_target_properties(vorbis PROPERTIES
	IMPORTED_LOCATION ${GY_TARGET_CONTRIB_OUT_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}vorbis${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(vorbis libvorbis_build)
add_library(vorbisfile STATIC IMPORTED)
set_target_properties(vorbisfile PROPERTIES
	IMPORTED_LOCATION ${GY_TARGET_CONTRIB_OUT_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}vorbisfile${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(vorbisfile libvorbis_build)
