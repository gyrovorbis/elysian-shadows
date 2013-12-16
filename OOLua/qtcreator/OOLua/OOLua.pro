TEMPLATE = lib
CONFIG += staticlib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11 \
  -fno-rtti \
  -fno-exceptions
#  -mstackrealign

#Required to get C++11x to work on OSX
macx {
  QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
  LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
}


INCLUDEPATH += ../../include \
    ../../../ESGamma/lib/include

DEPENDPATH += ../../include

SOURCES += \
    ../../source/stack_get.cpp \
    ../../source/push_pointer_internal.cpp \
    ../../source/proxy_storage.cpp \
    ../../source/oolua.cpp \
    ../../source/oolua_table.cpp \
    ../../source/oolua_string.cpp \
    ../../source/oolua_stack_dump.cpp \
    ../../source/oolua_script.cpp \
    ../../source/oolua_registration.cpp \
    ../../source/oolua_ref.cpp \
    ../../source/oolua_push.cpp \
    ../../source/oolua_pull.cpp \
    ../../source/oolua_open.cpp \
    ../../source/oolua_helpers.cpp \
    ../../source/oolua_function.cpp \
    ../../source/oolua_error.cpp \
    ../../source/oolua_chunk.cpp \
    ../../source/oolua_check_result.cpp \
    ../../source/class_from_stack.cpp

HEADERS += \
    ../../include/typelist_structs.h \
    ../../include/type_list.h \
    ../../include/type_converters.h \
    ../../include/stack_get.h \
    ../../include/push_pointer_internal.h \
    ../../include/proxy_userdata.h \
    ../../include/proxy_test.h \
    ../../include/proxy_tags.h \
    ../../include/proxy_tag_info.h \
    ../../include/proxy_storage.h \
    ../../include/proxy_stack_helper.h \
    ../../include/proxy_public_member.h \
    ../../include/proxy_operators.h \
    ../../include/proxy_none_member_function.h \
    ../../include/proxy_member_function.h \
    ../../include/proxy_function_exports.h \
    ../../include/proxy_function_dispatch.h \
    ../../include/proxy_constructor.h \
    ../../include/proxy_constructor_param_tester.h \
    ../../include/proxy_class.h \
    ../../include/proxy_caller.h \
    ../../include/proxy_base_checker.h \
    ../../include/platform_check.h \
    ../../include/oolua.h \
    ../../include/oolua_version.h \
    ../../include/oolua_traits.h \
    ../../include/oolua_traits_fwd.h \
    ../../include/oolua_table.h \
    ../../include/oolua_string.h \
    ../../include/oolua_stack.h \
    ../../include/oolua_stack_fwd.h \
    ../../include/oolua_stack_dump.h \
    ../../include/oolua_script.h \
    ../../include/oolua_registration.h \
    ../../include/oolua_registration_fwd.h \
    ../../include/oolua_ref.h \
    ../../include/oolua_push.h \
    ../../include/oolua_pull.h \
    ../../include/oolua_open.h \
    ../../include/oolua_helpers.h \
    ../../include/oolua_function.h \
    ../../include/oolua_exception.h \
    ../../include/oolua_error.h \
    ../../include/oolua_dsl.h \
    ../../include/oolua_dsl_export.h \
    ../../include/oolua_config.h \
    ../../include/oolua_chunk.h \
    ../../include/oolua_check_result.h \
    ../../include/oolua_boilerplate.h \
    ../../include/lvd_types.h \
    ../../include/lvd_type_traits.h \
    ../../include/lua_includes.h \
    ../../include/dsl_va_args.h \
    ../../include/default_trait_caller.h \
    ../../include/class_from_stack.h \
    ../../include/char_arrays.h
