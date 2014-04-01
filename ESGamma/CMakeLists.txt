project(ESGamma CXX)

cmake_minimum_required(VERSION 2.8.8)

#
# Set up some variables
#

set(ESG_ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(ESG_SOURCE_DIR "${ESG_ROOT_DIR}/source")
set(ESG_INCLUDE_DIR "${ESG_ROOT_DIR}/include")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

#
# ESGamma
#

add_definitions(-DELYSIAN_ENGINE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 -stdlib=libc++")
endif()

set(ESG_SOURCES
	${ESG_SOURCE_DIR}/main.cpp
	${ESG_SOURCE_DIR}/elysian_adventure_log.cpp
	${ESG_SOURCE_DIR}/elysian_animation_system.cpp
	${ESG_SOURCE_DIR}/elysian_audio_system.cpp
	${ESG_SOURCE_DIR}/elysian_behavior_system.cpp
	${ESG_SOURCE_DIR}/elysian_camera_system.cpp
	${ESG_SOURCE_DIR}/elysian_character_system.cpp
	${ESG_SOURCE_DIR}/elysian_collision_system.cpp
	${ESG_SOURCE_DIR}/elysian_combat_system.cpp
	${ESG_SOURCE_DIR}/elysian_day_system.cpp
	${ESG_SOURCE_DIR}/elysian_debug.cpp
	${ESG_SOURCE_DIR}/elysian_dialog_system.cpp
	${ESG_SOURCE_DIR}/elysian_engine.cpp
	${ESG_SOURCE_DIR}/elysian_examinable_system.cpp
	${ESG_SOURCE_DIR}/elysian_font.cpp
	${ESG_SOURCE_DIR}/elysian_force_generator.cpp
	${ESG_SOURCE_DIR}/elysian_game_input.cpp
	${ESG_SOURCE_DIR}/elysian_gui_system.cpp
	${ESG_SOURCE_DIR}/elysian_level_system.cpp
	${ESG_SOURCE_DIR}/elysian_light_system.cpp
	${ESG_SOURCE_DIR}/elysian_lua_manager.cpp
	${ESG_SOURCE_DIR}/elysian_lua_script.cpp
	${ESG_SOURCE_DIR}/elysian_lua_singleton.cpp
	${ESG_SOURCE_DIR}/elysian_lua_wrappers.cpp
	${ESG_SOURCE_DIR}/elysian_map_system.cpp
	${ESG_SOURCE_DIR}/elysian_particle_system.cpp
	${ESG_SOURCE_DIR}/elysian_party_system.cpp
	${ESG_SOURCE_DIR}/elysian_quad_attrib.cpp
	${ESG_SOURCE_DIR}/elysian_rigid_body_system.cpp
	${ESG_SOURCE_DIR}/elysian_shader_manager.cpp
	${ESG_SOURCE_DIR}/elysian_system.cpp
	${ESG_SOURCE_DIR}/elysian_timer_system.cpp
	${ESG_SOURCE_DIR}/elysian_warp_system.cpp
)

set(ESG_INCLUDES
	${ESG_INCLUDE_DIR}/elysian_adventure_log.hpp
	${ESG_INCLUDE_DIR}/elysian_animation_system.hpp
	${ESG_INCLUDE_DIR}/elysian_audio_system.hpp
	${ESG_INCLUDE_DIR}/elysian_behavior_system.hpp
	${ESG_INCLUDE_DIR}/elysian_camera_system.hpp
	${ESG_INCLUDE_DIR}/elysian_character_system.hpp
	${ESG_INCLUDE_DIR}/elysian_collision_system.hpp
	${ESG_INCLUDE_DIR}/elysian_combat_system.hpp
	${ESG_INCLUDE_DIR}/elysian_day_system.hpp
	${ESG_INCLUDE_DIR}/elysian_debug.hpp
	${ESG_INCLUDE_DIR}/elysian_dialog_system.hpp
	${ESG_INCLUDE_DIR}/elysian_engine.hpp
	${ESG_INCLUDE_DIR}/elysian_examinable_system.hpp
	${ESG_INCLUDE_DIR}/elysian_font.hpp
	${ESG_INCLUDE_DIR}/elysian_force_generator.hpp
	${ESG_INCLUDE_DIR}/elysian_game_input.hpp
	${ESG_INCLUDE_DIR}/elysian_gui_system.hpp
	${ESG_INCLUDE_DIR}/elysian_level_system.hpp
	${ESG_INCLUDE_DIR}/elysian_light_system.hpp
	${ESG_INCLUDE_DIR}/elysian_lua_manager.hpp
	${ESG_INCLUDE_DIR}/elysian_lua_script.hpp
	${ESG_INCLUDE_DIR}/elysian_lua_singleton.hpp
	${ESG_INCLUDE_DIR}/elysian_lua_wrappers.hpp
	${ESG_INCLUDE_DIR}/elysian_map_system.hpp
	${ESG_INCLUDE_DIR}/elysian_model.hpp
	${ESG_INCLUDE_DIR}/elysian_particle_system.hpp
	${ESG_INCLUDE_DIR}/elysian_party_system.hpp
	${ESG_INCLUDE_DIR}/elysian_quad_sprite.hpp
	${ESG_INCLUDE_DIR}/elysian_rigid_body_system.hpp
	${ESG_INCLUDE_DIR}/elysian_shader_manager.hpp
	${ESG_INCLUDE_DIR}/elysian_sprite.hpp
	${ESG_INCLUDE_DIR}/elysian_system.hpp
	${ESG_INCLUDE_DIR}/elysian_timer_system.hpp
	${ESG_INCLUDE_DIR}/elysian_warp_system.hpp
)

include_directories(${ESG_INCLUDE_DIR})

if(APPLE)
	add_executable(ESGamma MACOSX_BUNDLE
		${ESG_SOURCES}
		${AssetIO_SOURCES}
		${ESG_INCLUDES}
		${AssetIO_INCLUDES}
	)
	set_target_properties(ESGamma PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "ESGamma")
else()
	add_executable(ESGamma
		${ESG_SOURCES}
		${AssetIO_SOURCES}
		${ESG_INCLUDES}
		${AssetIO_INCLUDES}
	)
endif()

if(APPLE)
	set_target_properties(ESGamma PROPERTIES
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${es_VERSION_FULL}
		MACOSX_BUNDLE_LONG_VERSION_STRING ${es_VERSION_FULL}
	)
endif()

target_link_libraries(ESGamma ${Gyro_LIBRARIES} OOLua lua52)

install(TARGETS ESGamma
	BUNDLE DESTINATION bin
    RUNTIME DESTINATION bin)

group_by_folder(${ESG_ROOT_DIR} "${ESG_SOURCES}")
group_by_folder(${ESG_ROOT_DIR} "${ESG_INCLUDES}")
