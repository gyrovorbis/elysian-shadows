project(OOLua CXX)

cmake_minimum_required(VERSION 2.8.8)

#
# Set up some variables
#

set(OOLUA_ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(OOLUA_SOURCE_DIR "${OOLUA_ROOT_DIR}/source")
set(OOLUA_INCLUDE_DIR "${OOLUA_ROOT_DIR}/include")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

#
# libOOLua
#

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")
if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 -stdlib=libc++")
endif()

set(OOLua_SOURCES
	${OOLUA_SOURCE_DIR}/stack_get.cpp
	${OOLUA_SOURCE_DIR}/push_pointer_internal.cpp
	${OOLUA_SOURCE_DIR}/proxy_storage.cpp
	${OOLUA_SOURCE_DIR}/oolua.cpp
	${OOLUA_SOURCE_DIR}/oolua_table.cpp
	${OOLUA_SOURCE_DIR}/oolua_string.cpp
	${OOLUA_SOURCE_DIR}/oolua_stack_dump.cpp
	${OOLUA_SOURCE_DIR}/oolua_script.cpp
	${OOLUA_SOURCE_DIR}/oolua_registration.cpp
	${OOLUA_SOURCE_DIR}/oolua_ref.cpp
	${OOLUA_SOURCE_DIR}/oolua_push.cpp
	${OOLUA_SOURCE_DIR}/oolua_pull.cpp
	${OOLUA_SOURCE_DIR}/oolua_open.cpp
	${OOLUA_SOURCE_DIR}/oolua_helpers.cpp
	${OOLUA_SOURCE_DIR}/oolua_function.cpp
	${OOLUA_SOURCE_DIR}/oolua_error.cpp
	${OOLUA_SOURCE_DIR}/oolua_chunk.cpp
	${OOLUA_SOURCE_DIR}/oolua_check_result.cpp
	${OOLUA_SOURCE_DIR}/class_from_stack.cpp
)

set(OOLua_INCLUDES
	${OOLUA_INCLUDE_DIR}/typelist_structs.h
	${OOLUA_INCLUDE_DIR}/type_list.h
	${OOLUA_INCLUDE_DIR}/type_converters.h
	${OOLUA_INCLUDE_DIR}/stack_get.h
	${OOLUA_INCLUDE_DIR}/push_pointer_internal.h
	${OOLUA_INCLUDE_DIR}/proxy_userdata.h
	${OOLUA_INCLUDE_DIR}/proxy_test.h
	${OOLUA_INCLUDE_DIR}/proxy_tags.h
	${OOLUA_INCLUDE_DIR}/proxy_tag_info.h
	${OOLUA_INCLUDE_DIR}/proxy_storage.h
	${OOLUA_INCLUDE_DIR}/proxy_stack_helper.h
	${OOLUA_INCLUDE_DIR}/proxy_public_member.h
	${OOLUA_INCLUDE_DIR}/proxy_operators.h
	${OOLUA_INCLUDE_DIR}/proxy_none_member_function.h
	${OOLUA_INCLUDE_DIR}/proxy_member_function.h
	${OOLUA_INCLUDE_DIR}/proxy_function_exports.h
	${OOLUA_INCLUDE_DIR}/proxy_function_dispatch.h
	${OOLUA_INCLUDE_DIR}/proxy_constructor.h
	${OOLUA_INCLUDE_DIR}/proxy_constructor_param_tester.h
	${OOLUA_INCLUDE_DIR}/proxy_class.h
	${OOLUA_INCLUDE_DIR}/proxy_caller.h
	${OOLUA_INCLUDE_DIR}/proxy_base_checker.h
	${OOLUA_INCLUDE_DIR}/platform_check.h
	${OOLUA_INCLUDE_DIR}/oolua.h
	${OOLUA_INCLUDE_DIR}/oolua_version.h
	${OOLUA_INCLUDE_DIR}/oolua_traits.h
	${OOLUA_INCLUDE_DIR}/oolua_traits_fwd.h
	${OOLUA_INCLUDE_DIR}/oolua_table.h
	${OOLUA_INCLUDE_DIR}/oolua_string.h
	${OOLUA_INCLUDE_DIR}/oolua_stack.h
	${OOLUA_INCLUDE_DIR}/oolua_stack_fwd.h
	${OOLUA_INCLUDE_DIR}/oolua_stack_dump.h
	${OOLUA_INCLUDE_DIR}/oolua_script.h
	${OOLUA_INCLUDE_DIR}/oolua_registration.h
	${OOLUA_INCLUDE_DIR}/oolua_registration_fwd.h
	${OOLUA_INCLUDE_DIR}/oolua_ref.h
	${OOLUA_INCLUDE_DIR}/oolua_push.h
	${OOLUA_INCLUDE_DIR}/oolua_pull.h
	${OOLUA_INCLUDE_DIR}/oolua_open.h
	${OOLUA_INCLUDE_DIR}/oolua_helpers.h
	${OOLUA_INCLUDE_DIR}/oolua_function.h
	${OOLUA_INCLUDE_DIR}/oolua_exception.h
	${OOLUA_INCLUDE_DIR}/oolua_error.h
	${OOLUA_INCLUDE_DIR}/oolua_dsl.h
	${OOLUA_INCLUDE_DIR}/oolua_dsl_export.h
	${OOLUA_INCLUDE_DIR}/oolua_config.h
	${OOLUA_INCLUDE_DIR}/oolua_chunk.h
	${OOLUA_INCLUDE_DIR}/oolua_check_result.h
	${OOLUA_INCLUDE_DIR}/oolua_boilerplate.h
	${OOLUA_INCLUDE_DIR}/lvd_types.h
	${OOLUA_INCLUDE_DIR}/lvd_type_traits.h
	${OOLUA_INCLUDE_DIR}/lua_includes.h
	${OOLUA_INCLUDE_DIR}/dsl_va_args.h
	${OOLUA_INCLUDE_DIR}/default_trait_caller.h
	${OOLUA_INCLUDE_DIR}/class_from_stack.h
	${OOLUA_INCLUDE_DIR}/char_arrays.h
)

include_directories(${OOLUA_INCLUDE_DIR})

add_library(OOLua STATIC ${OOLua_SOURCES} ${OOLua_INCLUDES})

install(TARGETS OOLua
    ARCHIVE DESTINATION lib)
install(FILES ${OOLua_INCLUDES}
	DESTINATION include/oolua)

group_by_folder(${OOLUA_ROOT_DIR} "${OOLua_SOURCES}")
group_by_folder(${OOLUA_ROOT_DIR} "${OOLua_INCLUDES}")
