include(ExternalProject2)

set(openal_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/openal)

set(openal_CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DALSOFT_CONFIG=OFF
        -DEXAMPLES=OFF
        -DUTILS=OFF
	-DLIBTYPE=STATIC
	-Wno-dev
)

if(ANDROID)
    set(openal_GIT_URL git://github.com/AerialX/openal-soft-android.git)
    set(openal_GIT_TAG master)
else()
    set(openal_GIT_URL git://internal.invalid/esdev/openal-soft.git)
    set(openal_GIT_TAG v1.15.1-es)
endif()

ExternalProject_Add(openal_build
	PREFIX ${openal_PREFIX}

	DOWNLOAD_DIR ${GY_CONTRIB_DOWNLOAD_DIR}

        GIT_REPOSITORY ${openal_GIT_URL}
        GIT_TAG ${openal_GIT_TAG}

	INSTALL_DIR ${GY_TARGET_CONTRIB_OUT_DIR}
	CMAKE_ARGS ${openal_CMAKE_ARGS}
)

add_library(openal STATIC IMPORTED)
if(WIN32)
    set(openal_NAME "OpenAL32")
else()
    set(openal_NAME "openal")
endif()
set_target_properties(openal PROPERTIES
        IMPORTED_LOCATION ${GY_TARGET_CONTRIB_OUT_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${openal_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(openal openal_build)
