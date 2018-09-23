#include(../../../qtcreator/ESDev/ESDev.pri)
TEMPLATE = lib
CONFIG += staticlib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

macx {
# -mmacosx-version-min=10.8
    QMAKE_CXXFLAGS +=-stdlib=libc++
    LIBS +=-stdlib=libc++

    #CONFIG(release, debug|release) {
     #  QMAKE_POST_LINK = "macdeployqt TARGET"
    #}
}


INCLUDEPATH += ../../include/oolua \
	../../../Lua5.2/include

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
    ../../include/oolua/typelist_structs.h \
    ../../include/oolua/type_list.h \
    ../../include/oolua/type_converters.h \
    ../../include/oolua/stack_get.h \
    ../../include/oolua/push_pointer_internal.h \
    ../../include/oolua/proxy_userdata.h \
    ../../include/oolua/proxy_test.h \
    ../../include/oolua/proxy_tags.h \
    ../../include/oolua/proxy_tag_info.h \
    ../../include/oolua/proxy_storage.h \
    ../../include/oolua/proxy_stack_helper.h \
    ../../include/oolua/proxy_public_member.h \
    ../../include/oolua/proxy_operators.h \
    ../../include/oolua/proxy_none_member_function.h \
    ../../include/oolua/proxy_member_function.h \
    ../../include/oolua/proxy_function_exports.h \
    ../../include/oolua/proxy_function_dispatch.h \
    ../../include/oolua/proxy_constructor.h \
    ../../include/oolua/proxy_constructor_param_tester.h \
    ../../include/oolua/proxy_class.h \
    ../../include/oolua/proxy_caller.h \
    ../../include/oolua/proxy_base_checker.h \
    ../../include/oolua/platform_check.h \
    ../../include/oolua/oolua.h \
    ../../include/oolua/oolua_version.h \
    ../../include/oolua/oolua_traits.h \
    ../../include/oolua/oolua_traits_fwd.h \
    ../../include/oolua/oolua_table.h \
    ../../include/oolua/oolua_string.h \
    ../../include/oolua/oolua_stack.h \
    ../../include/oolua/oolua_stack_fwd.h \
    ../../include/oolua/oolua_stack_dump.h \
    ../../include/oolua/oolua_script.h \
    ../../include/oolua/oolua_registration.h \
    ../../include/oolua/oolua_registration_fwd.h \
    ../../include/oolua/oolua_ref.h \
    ../../include/oolua/oolua_push.h \
    ../../include/oolua/oolua_pull.h \
    ../../include/oolua/oolua_open.h \
    ../../include/oolua/oolua_helpers.h \
    ../../include/oolua/oolua_function.h \
    ../../include/oolua/oolua_exception.h \
    ../../include/oolua/oolua_error.h \
    ../../include/oolua/oolua_dsl.h \
    ../../include/oolua/oolua_dsl_export.h \
    ../../include/oolua/oolua_config.h \
    ../../include/oolua/oolua_chunk.h \
    ../../include/oolua/oolua_check_result.h \
    ../../include/oolua/oolua_boilerplate.h \
    ../../include/oolua/lvd_types.h \
    ../../include/oolua/lvd_type_traits.h \
    ../../include/oolua/lua_includes.h \
    ../../include/oolua/dsl_va_args.h \
    ../../include/oolua/default_trait_caller.h \
    ../../include/oolua/class_from_stack.h \
    ../../include/oolua/char_arrays.h
