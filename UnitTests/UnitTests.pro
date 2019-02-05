#-------------------------------------------------
#
# Project created by QtCreator 2015-06-03T02:44:53
#
#-------------------------------------------------

#include(../qtcreator/ESDev/ESDev.pri)

QT       += widgets network opengl sql testlib

CONFIG += c++14
CONFIG -= exceptions rtti

DEFINES += ELYSIAN_OPENCL_ACCELERATED

DEPENDPATH += \
    ../AssetIO \
    ../ESGamma \
    ../ESToolkit \
    ../libGyro \
    ../libGyro-Kinetic \
    ../QESTkUtils


#PRIORITIZED TO REDUCE BUILD TIME ON FUCKING MINGW
INCLUDEPATH +=  \
    include/UnitTests \
    ../libGyro/include \
    ../libGyro/api \
    ../libGyro-Kinetic/api \
    ../AssetIO/include \
    ../ESGamma/include \
    ../QESTkUtils/include \
    $${GLOBAL_INCLUDE_PATH} \
    ../Lua5.2/include \
    ../OOLua/include/oolua \
    ../Spine/include \

win32 {
    INCLUDEPATH += ../libGyro/lib/win32
}

CONFIG   += staticlib

TEMPLATE = lib


SOURCES += \
    source/test_estk.cpp

HEADERS += \
    include/UnitTests/test_libgyro_file_api.h \
    include/UnitTests/test_libgyro_video_api.h \
    include/UnitTests/test_entity.h \
    include/UnitTests/test_lua_behavior.h \
    include/UnitTests/test_lua_table_proxy.h \
    include/UnitTests/test_lua_manager.h \
    include/UnitTests/test_level.h \
    include/UnitTests/test_area.h \
    include/UnitTests/test_light_system.h \
    include/UnitTests/test_audio_system.h \
    include/UnitTests/test_map_view.h \
    include/UnitTests/test_estk.h \
    include/UnitTests/test_estoolkit.h \
    include/UnitTests/test_init.h \
    include/UnitTests/test_project_assets.h \
    include/UnitTests/test_level_assets.h \
    include/UnitTests/test_area_assets.h \
    include/UnitTests/test_lua_variant.h \
    include/UnitTests/test_dialog.h \
    include/UnitTests/test_instance_transform.h \
    include/UnitTests/test_lua_field_ref.h \
    include/UnitTests/test_functional.h \
    include/UnitTests/test_cpp_lua_callback.h \
    include/UnitTests/test_associative_aggregate_array.h \
    include/UnitTests/test_libgyro_compute_api.h \
    include/UnitTests/test_string.h \
    include/UnitTests/test_memory_mapped_byte_array.h

