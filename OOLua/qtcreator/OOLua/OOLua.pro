TEMPLATE = lib
CONFIG += staticlib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../include \
    ../../../ESGamma/lib/include

SOURCES += \
    ../../source/push_pointer_internal.cpp \
    ../../source/oolua_storage.cpp \
    ../../source/oolua_registration.cpp \
    ../../source/oolua_push_pull.cpp \
    ../../source/oolua_error.cpp \
    ../../source/oolua_check_result.cpp \
    ../../source/oolua.cpp \
    ../../source/lua_table.cpp \
    ../../source/lua_stack_dump.cpp \
    ../../source/lua_ref.cpp \
    ../../source/lua_function.cpp \
    ../../source/class_from_stack.cpp

HEADERS += \
    ../../include/typelist_structs.h \
    ../../include/type_list.h \
    ../../include/push_pointer_internal.h \
    ../../include/proxy_class.h \
    ../../include/proxy_caller.h \
    ../../include/platform_check.h \
    ../../include/param_traits.h \
    ../../include/oolua_version.h \
    ../../include/oolua_va_args.h \
    ../../include/oolua_userdata.h \
    ../../include/oolua_typedefs.h \
    ../../include/oolua_storage.h \
    ../../include/oolua_registration.h \
    ../../include/oolua_push_pull.h \
    ../../include/oolua_parameter_helper.h \
    ../../include/oolua_paramater_macros.h \
    ../../include/oolua_member_function.h \
    ../../include/oolua_exception.h \
    ../../include/oolua_error.h \
    ../../include/oolua_converters.h \
    ../../include/oolua_config.h \
    ../../include/oolua_check_result.h \
    ../../include/oolua_char_arrays.h \
    ../../include/oolua_c_func.h \
    ../../include/oolua.h \
    ../../include/member_func_helper.h \
    ../../include/lvd_types.h \
    ../../include/lua_table.h \
    ../../include/lua_stack_dump.h \
    ../../include/lua_ref.h \
    ../../include/lua_operator.h \
    ../../include/lua_includes.h \
    ../../include/lua_function.h \
    ../../include/fwd_push_pull.h \
    ../../include/export_func_to_lua.h \
    ../../include/determin_qualifier.h \
    ../../include/cpp_member_func.h \
    ../../include/cpp_constructor.h \
    ../../include/class_public_member.h \
    ../../include/class_from_stack.h \
    ../../include/base_checker.h

