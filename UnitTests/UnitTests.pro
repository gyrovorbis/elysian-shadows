#-------------------------------------------------
#
# Project created by QtCreator 2015-06-03T02:44:53
#
#-------------------------------------------------

include(../qtcreator/ESDev/ESDev.pri)

QT       += widgets network opengl sql testlib



DEPENDPATH += ../AssetIO/include/component \
	../AssetIO/include/crtp \
	../AssetIO/include/level \
	../AssetIO/include/primitive \
	../AssetIO/include/utility \
	../ESGamma/include/crtp \
	../ESGamma/include/gui \
	../ESGamma/include/lua \
	../ESGamma/include/system \
	../ESGamma/include/utility \
	../ESToolkit/ESToolkit/include/asset \
	../ESToolkit/ESToolkit/include/model \
	../ESToolkit/ESToolkit/include/estk \
	../ESToolkit/ESToolkit/include/tile_engine \
	../ESToolkit/ESToolkit/include/core \
	../libGyro/include \
	../libGyro/include/kinetic \
	../ESToolkit/ESToolkit/include/sheet_manager \
	../ESToolkit/ESToolkit/include/dialog \
	../ESToolkit/ESToolkit/include/widget

#PRIORITIZED TO REDUCE BUILD TIME ON FUCKING MINGW
INCLUDEPATH += ../AssetIO/include/component \
	../AssetIO/include/crtp \
	../AssetIO/include/level \
	../AssetIO/include/primitive \
	../AssetIO/include/utility \
	../ESGamma/include/crtp \
	../ESGamma/include/gui \
	../ESGamma/include/lua \
	../ESGamma/include/system \
	../ESGamma/include/utility \
	../ESToolkit/ESToolkit/include/asset \
	../ESToolkit/ESToolkit/include/model \
	../ESToolkit/ESToolkit/include/estk \
	../ESToolkit/ESToolkit/include/tile_engine \
	../ESToolkit/ESToolkit/include/core \
	../libGyro/include \
	../libGyro/include/kinetic \
	../ESToolkit/ESToolkit/include/sheet_manager \
	../ESToolkit/ESToolkit/include/dialog \
	../ESToolkit/ESToolkit/include/widget \
	../Lua5.2/include \
	../OOLua/include \
	../ESGamma/lib/include \
	../Spine/include

win32 {
    INCLUDEPATH += ../libGyro/lib/win32
}

CONFIG   += staticlib

TEMPLATE = lib


SOURCES += \
    test_estk.cpp

HEADERS += \
    test_libgyro_file_api.h \
    test_libgyro_video_api.h \
    test_entity.h \
    test_lua_behavior.h \
    test_lua_table_proxy.h \
    test_lua_manager.h \
    test_level.h \
    test_area.h \
    test_light_system.h \
    test_audio_system.h \
    test_map_view.h \
    test_estk.h \
    test_estoolkit.h \
    test_init.h \
    test_project_assets.h \
    test_level_assets.h \
    test_area_assets.h

OBJECTIVE_SOURCES +=
