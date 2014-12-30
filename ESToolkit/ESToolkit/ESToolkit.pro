# -------------------------------------------------
# Project created by QtCreator 2010-03-06T14:23:27
# And modified to all fuck by Falco ALL THE DAYS
# -------------------------------------------------
TARGET = ESToolkit
TEMPLATE = app
QT		+= opengl
QMAKE_CXXFLAGS += -std=c++11 -msse
DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_Qt
CONFIG += c++11


ICON = resource/images/estk.icns

DEPENDPATH += include/asset \
    include/core \
    include/dialog \
    include/estk \
    include/sheetManager \
    include/tileEngine \
    include/widget \
    ../../AssetIO/include \
    ../../libGyro/include \
	../../libGyro/include/kinetic \
	../../ESGamma/include

INCLUDEPATH += include/asset \
    include/core \
    include/dialog \
    include/estk \
    include/sheetManager \
    include/tileEngine \
    include/widget \
    ../../AssetIO/include \
    ../../libGyro/include \
	../../libGyro/include/kinetic \
	../../ESGamma/lib/include \
	../../ESGamma/include \
	../../OOLua/include \
	../../Lua5.2/include


SOURCES += source/asset/project.cpp \
    source/asset/level.cpp \
    source/asset/entity.cpp \
    source/asset/assets.cpp \
    source/asset/assetManager.cpp \
    source/asset/area.cpp \
    source/core/undoItem.cpp \
    source/core/treenode.cpp \
    source/core/tileSheetUtilities.cpp \
    source/core/selectable.cpp \
    source/core/main.cpp \
    source/core/fileUtilities.cpp \
    source/estk/tileSelectable.cpp \
    source/estk/sheetViewTileGraphicsItem.cpp \
    source/estk/sheetView.cpp \
    source/estk/sheetDockWidget.cpp \
    source/estk/selectionDockWidget.cpp \
    source/estk/mapView.cpp \
    source/estk/layerDockWidget.cpp \
    source/estk/estoolkit.cpp \
    source/estk/entityTreeWidget.cpp \
    source/estk/entityDockWidget.cpp \
    source/estk/assetTreeWidget.cpp \
    source/estk/assetDockWidget.cpp \
    source/sheetManager/sheetManagerSourceView.cpp \
    source/sheetManager/sheetManagerDestView.cpp \
    source/sheetManager/SheetManagerAssetManager.cpp \
    source/sheetManager/sheetmanager.cpp \
    source/tileEngine/sourceView.cpp \
    source/widget/historyDockWidget.cpp \
    source/asset/character.cpp \
    source/asset/collidable.cpp \
    source/asset/renderable.cpp \
    source/asset/warp.cpp \
    source/core/propertyStackModel.cpp \
    source/dialog/engineDialog.cpp \
    source/dialog/engineSettingsDialog.cpp \
    source/sheetManager/sheetManagerFileDockWidget.cpp \
    source/sheetManager/sheetManagerSourceTileGraphicsItem.cpp \
    source/estk/tileSelectableSelectionViewDelegate.cpp \
    include/tileEngine/tileGraphicsItem.inl \
    include/tileEngine/destView.inl \
    include/tileEngine/tileSelection.inl \
    source/sheetManager/SheetManagerDestTileGraphicsItem.cpp \
    source/estk/mapViewTileGraphicsItem.cpp \
    source/asset/mapEntry.cpp \
    ../../AssetIO/source/elysian_zcoords.cpp \
    ../../AssetIO/source/elysian_warp.cpp \
    ../../AssetIO/source/elysian_tile.cpp \
    ../../AssetIO/source/elysian_renderable.cpp \
    ../../AssetIO/source/elysian_lua_behavior.cpp \
    ../../AssetIO/source/elysian_level_manager.cpp \
    ../../AssetIO/source/elysian_level.cpp \
    ../../AssetIO/source/elysian_examinable.cpp \
    ../../AssetIO/source/elysian_entity.cpp \
    ../../AssetIO/source/elysian_dialog.cpp \
    ../../AssetIO/source/elysian_collidable.cpp \
    ../../AssetIO/source/elysian_character.cpp \
    ../../AssetIO/source/elysian_behavior.cpp \
    ../../AssetIO/source/elysian_asset.cpp \
    ../../AssetIO/source/elysian_area.cpp \
    ../../AssetIO/source/elysian_audio_source.cpp \
    ../../AssetIO/source/elysian_light_source.cpp \
    source/asset/elysianTileWrapper.cpp \
    ../../AssetIO/source/elysian_particle_emitter.cpp \
    ../../AssetIO/source/elysian_rigid_body.cpp \
    ../../AssetIO/source/elysian_combat.cpp \
    ../../AssetIO/source/elysian_lua_table_model.cpp \
	../../AssetIO/source/luatables.cc \
	../../ESGamma/source/elysian_lua_manager.cpp \
    ../../ESGamma/source/elysian_adventure_log.cpp \
    ../../ESGamma/source/elysian_animation_system.cpp \
    ../../ESGamma/source/elysian_audio_system.cpp \
    ../../ESGamma/source/elysian_behavior_system.cpp \
    ../../ESGamma/source/elysian_camera_system.cpp \
    ../../ESGamma/source/elysian_character_system.cpp \
    ../../ESGamma/source/elysian_collision_system.cpp \
    ../../ESGamma/source/elysian_combat_system.cpp \
    ../../ESGamma/source/elysian_day_system.cpp \
    ../../ESGamma/source/elysian_dialog_system.cpp \
    ../../ESGamma/source/elysian_engine.cpp \
    ../../ESGamma/source/elysian_event_monitor.cpp \
    ../../ESGamma/source/elysian_examinable_system.cpp \
    ../../ESGamma/source/elysian_font.cpp \
    ../../ESGamma/source/elysian_force_generator.cpp \
    ../../ESGamma/source/elysian_game_input.cpp \
    ../../ESGamma/source/elysian_gui_system.cpp \
    ../../ESGamma/source/elysian_level_system.cpp \
    ../../ESGamma/source/elysian_light_system.cpp \
    ../../ESGamma/source/elysian_lua_script.cpp \
    ../../ESGamma/source/elysian_lua_singleton.cpp \
    ../../ESGamma/source/elysian_lua_wrappers.cpp \
    ../../ESGamma/source/elysian_map_system.cpp \
    ../../ESGamma/source/elysian_particle_system.cpp \
    ../../ESGamma/source/elysian_party_system.cpp \
    ../../ESGamma/source/elysian_quad_attrib.cpp \
    ../../ESGamma/source/elysian_rigid_body_system.cpp \
    ../../ESGamma/source/elysian_sort_sweep.cpp \
    ../../ESGamma/source/elysian_system.cpp \
    ../../ESGamma/source/elysian_timer_system.cpp \
    ../../ESGamma/source/elysian_warp_system.cpp \
    source/dialog/luaExplorerDialog.cpp \
    source/core/rowsJoinerProxyModel.cpp \
    ../../AssetIO/source/elysian_instance_transform.cpp \
    source/estk/engineGLSceneWidget.cpp \
    source/estk/tkMapSystem.cpp \
    source/estk/entitySelectable.cpp \
    ../../ESGamma/source/elysian_debug_base.cpp \
    source/core/tkDebug.cpp \
    ../../AssetIO/source/elysian_animation_base.cpp \
    source/asset/tkAnimation.cpp

HEADERS += include/asset/project.h \
    include/asset/level.h \
    include/asset/entity.h \
    include/asset/assets.h \
    include/asset/assetManager.h \
    include/asset/area.h \
    include/core/undoItem.h \
    include/core/treenode.h \
    include/core/tileSheetUtilities.h \
    include/core/selectionScene.h \
    include/core/selectable.h \
    include/core/fileUtilities.h \
    include/estk/tileSelectable.h \
    include/estk/sheetViewTileGraphicsItem.h \
    include/estk/sheetView.h \
    include/estk/sheetDockWidget.h \
    include/estk/selectionDockWidget.h \
    include/estk/mapViewTileGraphicsItem.h \
    include/estk/mapView.h \
    include/estk/layerDockWidget.h \
    include/estk/estoolkit.h \
    include/estk/entityTreeWidget.h \
    include/estk/entityDockWidget.h \
    include/estk/assetTreeWidget.h \
    include/estk/assetDockWidget.h \
    include/sheetManager/sheetManagerTileSelection.h \
    include/sheetManager/sheetManagerSourceView.h \
    include/sheetManager/sheetManagerPlaceTileGroupUndoItem.h \
    include/sheetManager/sheetManagerDestView.h \
    include/sheetManager/SheetManagerAssets.h \
    include/sheetManager/SheetManagerAssetManager.h \
    include/tileEngine/tileSelection.h \
    include/tileEngine/tileGroupUndoItem.h \
    include/tileEngine/tileGraphicsItem.h \
    include/tileEngine/sourceView.h \
    include/tileEngine/selectionItemGroup.h \
    include/tileEngine/destView.h \
    include/widget/historyDockWidget.h \
    include/asset/character.h \
    include/asset/collidable.h \
    include/asset/component.h \
    include/asset/renderable.h \
    include/asset/sprite.h \
    include/asset/warp.h \
    include/core/propertyStackModel.h \
    include/core/propertyStandardItem.h \
    include/dialog/engineDialog.h \
    include/dialog/engineSettingsDialog.h \
    include/sheetManager/SheetManagerDestTileGraphicsItem.h \
    include/sheetManager/sheetManagerFileDockWidget.h \
    include/sheetManager/sheetManagerSourceTileGraphicsItem.h \
    include/sheetManager/sheetManager.h \
    include/core/tileImage.h \
    include/estk/mapViewPlaceTileGroupUndoItem.h \
    include/core/checkeredAlphaRectGraphicsItem.h \
    include/estk/tileSelectableSelectionViewDelegate.h \
    include/estk/sheetViewColliderGraphicsItem.h \
    include/sheetManager/sheetManagerTileEntry.h \
    include/estk/mapViewDepthBufferGraphicsItem.h \
    include/asset/mapEntry.h \
    include/core/glUtilities.h \
    ../../AssetIO/include/elysian_zcoords.hpp \
    ../../AssetIO/include/elysian_warp.hpp \
    ../../AssetIO/include/elysian_tile.hpp \
    ../../AssetIO/include/elysian_textured_attrib.hpp \
    ../../AssetIO/include/elysian_stl_algorithm.hpp \
    ../../AssetIO/include/elysian_sprite_attrib.hpp \
    ../../AssetIO/include/elysian_renderable_object.hpp \
    ../../AssetIO/include/elysian_renderable.hpp \
    ../../AssetIO/include/elysian_quad_sprite_attrib.hpp \
    ../../AssetIO/include/elysian_prim_attrib.hpp \
    ../../AssetIO/include/elysian_model_attrib.hpp \
    ../../AssetIO/include/elysian_map_depth_buffer.hpp \
    ../../AssetIO/include/elysian_lua_behavior.hpp \
    ../../AssetIO/include/elysian_level_manager.hpp \
    ../../AssetIO/include/elysian_level.hpp \
    ../../AssetIO/include/elysian_instance_transform.hpp \
    ../../AssetIO/include/elysian_examinable.hpp \
    ../../AssetIO/include/elysian_entity.hpp \
    ../../AssetIO/include/elysian_direction.hpp \
    ../../AssetIO/include/elysian_dialog.hpp \
    ../../AssetIO/include/elysian_cstr_map.hpp \
    ../../AssetIO/include/elysian_component.hpp \
    ../../AssetIO/include/elysian_collidable.hpp \
    ../../AssetIO/include/elysian_character.hpp \
    ../../AssetIO/include/elysian_behavior.hpp \
    ../../AssetIO/include/elysian_asset.hpp \
    ../../AssetIO/include/elysian_area.hpp \
    ../../AssetIO/include/elysian_audio_source.hpp \
    include/asset/elysianTileWrapper.h \
    ../../AssetIO/include/elysian_particle_emitter.hpp \
    ../../AssetIO/include/elysian_combat.hpp \
    ../../AssetIO/include/elysian_rigid_body.hpp \
    ../../AssetIO/include/luatables.h \
    ../../AssetIO/include/elysian_light_source.hpp \
    include/dialog/luaExplorerDialog.h \
    include/core/rowsJoinerProxyModel.h \
    ../../AssetIO/include/elysian_asset_typedefs.hpp \
    ../../ESGamma/include/elysian_adventure_log.hpp \
    ../../ESGamma/include/elysian_animation_system.hpp \
    ../../ESGamma/include/elysian_audio_system.hpp \
    ../../ESGamma/include/elysian_behavior_system.hpp \
    ../../ESGamma/include/elysian_camera_system.hpp \
    ../../ESGamma/include/elysian_character_system.hpp \
    ../../ESGamma/include/elysian_collision_system.hpp \
    ../../ESGamma/include/elysian_combat_system.hpp \
    ../../ESGamma/include/elysian_day_system.hpp \
    ../../ESGamma/include/elysian_dialog_system.hpp \
    ../../ESGamma/include/elysian_engine.hpp \
    ../../ESGamma/include/elysian_event_monitor.hpp \
    ../../ESGamma/include/elysian_examinable_system.hpp \
    ../../ESGamma/include/elysian_font.hpp \
    ../../ESGamma/include/elysian_force_generator.hpp \
    ../../ESGamma/include/elysian_game_input.hpp \
    ../../ESGamma/include/elysian_gui_system.hpp \
    ../../ESGamma/include/elysian_level_system.hpp \
    ../../ESGamma/include/elysian_light_system.hpp \
    ../../ESGamma/include/elysian_lua_manager.hpp \
    ../../ESGamma/include/elysian_lua_script.hpp \
    ../../ESGamma/include/elysian_lua_singleton.hpp \
    ../../ESGamma/include/elysian_lua_wrappers.hpp \
    ../../ESGamma/include/elysian_map_system.hpp \
    ../../ESGamma/include/elysian_model.hpp \
    ../../ESGamma/include/elysian_particle_system.hpp \
    ../../ESGamma/include/elysian_party_system.hpp \
    ../../ESGamma/include/elysian_quad_sprite.hpp \
    ../../ESGamma/include/elysian_rigid_body_system.hpp \
    ../../ESGamma/include/elysian_sort_sweep.hpp \
    ../../ESGamma/include/elysian_sprite.hpp \
    ../../ESGamma/include/elysian_subprocess_manager.hpp \
    ../../ESGamma/include/elysian_system.hpp \
    ../../ESGamma/include/elysian_timer_system.hpp \
    ../../ESGamma/include/elysian_warp_system.hpp \
    ../../AssetIO/include/elysian_lua_table_proxy.hpp \
    include/asset/luaTableStackModel.h \
    include/estk/engineGLSceneWidget.h \
    include/estk/tkMapSystem.h \
    include/estk/entitySelectable.h \
    include/core/tkDebug.h \
	../../ESGamma/include/elysian_debug_base.hpp \
	../../ESGamma/include/elysian_debug.hpp \
    ../../AssetIO/include/elysian_animation_base.hpp \
    ../../AssetIO/include/elysian_animation.hpp \
    include/asset/tkAnimation.h

FORMS += form/estk/sheetdockwidget.ui \
    form/estk/selectiondockwidget.ui \
    form/estk/layerdockwidget.ui \
    form/estk/estoolkit.ui \
    form/estk/entitydockwidget.ui \
    form/estk/assetdockwidget.ui \
    form/sheetManager/sheetmanager.ui \
    form/widget/historyDockWidget.ui \
    form/dialog/engineSettings.ui \
    form/sheetManager/sheetManagerFileDockWidget.ui \
    form/dialog/luaExplorer.ui


#Create windows manifest requesting ESTk to be run as admin (to run dc-tool as admin)
#win32 {
#    WINSDK_DIR = C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A
#    WIN_PWD = $$replace(PWD, /, \\)
#    OUT_PWD_WIN = $$replace(OUT_PWD, /, \\)
#    QMAKE_POST_LINK = "$$WINSDK_DIR/bin/x64/mt.exe -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest) -outputresource:$$quote($$OUT_PWD_WIN\\${DESTDIR_TARGET};1)"
#}

#Required to get C++11x to work on OSX
macx {
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
    LIBS += -mmacosx-version-min=10.7 -stdlib=libc++

    #CONFIG(release, debug|release) {
     #  QMAKE_POST_LINK = "macdeployqt TARGET"
    #}

}

RESOURCES += resource/resource.qrc \

macx {
    CONFIG(release, debug|release) {
        DESTDIR = ../../Project
    }
    QMAKE_POST_LINK = "./ESToolkitAppDereest.command"
}


win32 {
      CONFIG(release, debug|release) {
            DESTDIR = ../../Project
      }
}

linux {
      CONFIG(release, debug|release) {
            DESTDIR = ../../Project
      }
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libGyro/qtcreator/libGyro/release/ -llibGyro
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libGyro/qtcreator/libGyro/debug/ -llibGyro
else:unix: LIBS += -L$$OUT_PWD/../../libGyro/qtcreator/libGyro/ -llibGyro

INCLUDEPATH += $$PWD/../../libGyro/qtcreator/libGyro
DEPENDPATH += $$PWD/../../libGyro/qtcreator/libGyro

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/release/liblibGyro.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/debug/liblibGyro.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/release/libGyro.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/debug/libGyro.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/liblibGyro.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../OOLua/qtcreator/OOLua/release/ -lOOLua
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../OOLua/qtcreator/OOLua/debug/ -lOOLua
else:unix: LIBS += -L$$OUT_PWD/../../OOLua/qtcreator/OOLua/ -lOOLua

INCLUDEPATH += $$PWD/../../OOLua/qtcreator/OOLua
DEPENDPATH += $$PWD/../../OOLua/qtcreator/OOLua

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/release/libOOLua.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/debug/libOOLua.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/release/OOLua.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/debug/OOLua.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/libOOLua.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/release/ -lLua5_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/debug/ -lLua5_2
else:unix: LIBS += -L$$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/ -lLua5_2

INCLUDEPATH += $$PWD/../../Lua5.2/qtcreator/Lua5_2
DEPENDPATH += $$PWD/../../Lua5.2/qtcreator/Lua5_2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/release/libLua5_2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/debug/libLua5_2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/release/Lua5_2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/debug/Lua5_2.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/libLua5_2.a


win32 {
        QMAKE_CFLAGS += -msse -std=c99
        INCLUDEPATH += ../../libGyro/lib/win32
        LIBS += -L../../lib/win32 \
                #-llua52 \
                -L../../libGyro/lib/win32 \
                -lopengl32 \
                -lopenal32.dll \
                -lglu32 \
                -lz \
                -lz.dll \
               # -lmingw32 \
                -logg \
                -lvorbis \
                #-lvorbis.dll \
                -lvorbisenc \
                #-lvorbisenc.dll \
                -lvorbisfile \
                -lglew32 \
                -lglew32.dll \
                #-lvorbisfile.dll \
                -L../../ESGamma/lib/osx
                #-mconsole
                #-llua52
}

macx {
  LIBS +=  -L../../libGyro/lib/osx \
        -framework OpenGL \
        -framework OpenAL \
        -lz \
        -F../../ESGamma/lib/osx \
        -L../../ESGamma/lib/osx \
        #-llua52 \
        -framework Foundation \
        -framework Cocoa \
        -lobjc \
        -logg \
        -lvorbis \
        -lvorbisenc \
        -lvorbisfile \
        -L../../ESGamma/lib/osx
        #-llua52
}

linux {
        LIBS += \
                -llua5.2 \
                -lGL \
                -lopenal \
                -lGLU \
                -lz \
                -logg \
                -lvorbis \
                -lvorbisenc \
                -lvorbisfile \
                -lX11 \
                -lpthread \
                -L../../ESGamma/lib/osx  \
                #-llua52
}
