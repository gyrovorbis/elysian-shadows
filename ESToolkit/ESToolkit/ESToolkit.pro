# -------------------------------------------------
# Project created by QtCreator 2010-03-06T14:23:27
# And modified to all fuck by Falco ALL THE DAYS
# -------------------------------------------------
include(../../qtcreator/ESDev/ESDev.pri)
win32 {
  #  QMAKE_CXX=../../qtcreator/ESDev/ccache.exe g++
  #  QMAKE_CC=../../qtcreator/ESDev/ccache.exe gcc
}
TARGET = ESToolkit
TEMPLATE = app
QT		+= gui opengl
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs -Wno-empty-body

ICON = resource/images/estk.icns

DEPENDPATH += ../../AssetIO/include/component \
	../../AssetIO/include/crtp \
	../../AssetIO/include/level \
	../../AssetIO/include/primitive \
	../../AssetIO/include/utility \
	../../ESGamma/include/crtp \
	../../ESGamma/include/gui \
	../../ESGamma/include/lua \
	../../ESGamma/include/system \
	../../ESGamma/include/utility \
	include/asset \
	include/estk \
	include/tileEngine \
    include/core \
	../../libGyro/include \
	../../libGyro/include/kinetic \
    include/sheetManager \
	include/dialog \
    include/widget \

#PRIORITIZED TO REDUCE BUILD TIME ON FUCKING MINGW
INCLUDEPATH += ../../AssetIO/include/component \
	../../AssetIO/include/crtp \
	../../AssetIO/include/level \
	../../AssetIO/include/primitive \
	../../AssetIO/include/utility \
	../../ESGamma/include/crtp \
	../../ESGamma/include/gui \
	../../ESGamma/include/lua \
	../../ESGamma/include/system \
	../../ESGamma/include/utility \
	include/asset \
	include/estk \
	include/tileEngine \
	include/core \
	../../libGyro/include \
	../../libGyro/include/kinetic \
	include/sheetManager \
	include/dialog \
	include/widget \
	../../Lua5.2/include \
	../../OOLua/include \
	../../ESGamma/lib/include \
	../../Spine/include


SOURCES += source/asset/project.cpp \
    source/asset/assets.cpp \
    source/asset/assetManager.cpp \
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
    source/asset/elysianTileWrapper.cpp \
    source/dialog/luaExplorerDialog.cpp \
    source/core/rowsJoinerProxyModel.cpp \
    source/estk/engineGLSceneWidget.cpp \
    source/estk/entitySelectable.cpp \
    source/core/tkDebug.cpp \
    source/asset/tkAnimation.cpp \
    source/asset/tkArea.cpp \
    source/asset/tkLevel.cpp \
    source/asset/tkEntity.cpp \
    source/estk/tkMapSystem.cpp \
    source/asset/luaTableStackModel.cpp \
    source/asset/tkLuaTableProxy.cpp \
    source/estk/tkLuaWrappers.cpp \
    source/estk/tkLuaManager.cpp \
    ../../AssetIO/source/component/elysian_animation_base.cpp \
    ../../AssetIO/source/component/elysian_audio_source_base.cpp \
    ../../AssetIO/source/component/elysian_behavior_base.cpp \
    ../../AssetIO/source/component/elysian_character_base.cpp \
    ../../AssetIO/source/component/elysian_collidable_base.cpp \
    ../../AssetIO/source/component/elysian_combat_base.cpp \
    ../../AssetIO/source/component/elysian_examinable_base.cpp \
    ../../AssetIO/source/component/elysian_light_source_base.cpp \
    ../../AssetIO/source/component/elysian_lua_behavior.cpp \
    ../../AssetIO/source/component/elysian_particle_emitter_base.cpp \
    ../../AssetIO/source/component/elysian_renderable_base.cpp \
    ../../AssetIO/source/component/elysian_rigid_body_base.cpp \
    ../../AssetIO/source/component/elysian_skeleton_base.cpp \
    ../../AssetIO/source/component/elysian_warp_base.cpp \
    ../../AssetIO/source/level/elysian_area_base.cpp \
    ../../AssetIO/source/level/elysian_dialog.cpp \
    ../../AssetIO/source/level/elysian_entity_base.cpp \
    ../../AssetIO/source/level/elysian_level_base.cpp \
    ../../AssetIO/source/level/elysian_level_manager.cpp \
    ../../AssetIO/source/level/elysian_tile.cpp \
    ../../AssetIO/source/level/elysian_zcoords.cpp \
    ../../AssetIO/source/primitive/elysian_color.cpp \
    ../../AssetIO/source/primitive/elysian_instance_transform.cpp \
    ../../AssetIO/source/primitive/elysian_material.cpp \
    ../../AssetIO/source/primitive/elysian_quad_sprite_attrib.cpp \
    ../../AssetIO/source/primitive/elysian_sprite_attrib.cpp \
    ../../AssetIO/source/utility/elysian_lua_table_proxy_base.cpp \
    ../../AssetIO/source/utility/elysian_new.cpp \
    ../../AssetIO/source/utility/luatables.cc \
    ../../ESGamma/source/gui/elysian_adventure_log.cpp \
    ../../ESGamma/source/gui/elysian_font.cpp \
    ../../ESGamma/source/lua/elysian_lua_manager_base.cpp \
    ../../ESGamma/source/lua/elysian_lua_script.cpp \
    ../../ESGamma/source/lua/elysian_lua_singleton.cpp \
    ../../ESGamma/source/lua/elysian_lua_wrappers.cpp \
    ../../ESGamma/source/system/elysian_animation_system.cpp \
    ../../ESGamma/source/system/elysian_audio_system.cpp \
    ../../ESGamma/source/system/elysian_behavior_system.cpp \
    ../../ESGamma/source/system/elysian_camera_system.cpp \
    ../../ESGamma/source/system/elysian_character_system.cpp \
    ../../ESGamma/source/system/elysian_collision_system.cpp \
    ../../ESGamma/source/system/elysian_combat_system.cpp \
    ../../ESGamma/source/system/elysian_day_system.cpp \
    ../../ESGamma/source/system/elysian_dialog_system.cpp \
    ../../ESGamma/source/system/elysian_engine.cpp \
    ../../ESGamma/source/system/elysian_examinable_system.cpp \
    ../../ESGamma/source/system/elysian_gui_system.cpp \
    ../../ESGamma/source/system/elysian_level_system.cpp \
    ../../ESGamma/source/system/elysian_light_system.cpp \
    ../../ESGamma/source/system/elysian_map_system_base.cpp \
    ../../ESGamma/source/system/elysian_particle_system.cpp \
    ../../ESGamma/source/system/elysian_party_system.cpp \
    ../../ESGamma/source/system/elysian_rigid_body_system.cpp \
    ../../ESGamma/source/system/elysian_skeleton_system.cpp \
    ../../ESGamma/source/system/elysian_system.cpp \
    ../../ESGamma/source/system/elysian_timer_system.cpp \
    ../../ESGamma/source/system/elysian_warp_system.cpp \
    ../../ESGamma/source/utility/elysian_debug_base.cpp \
    ../../ESGamma/source/utility/elysian_event_monitor.cpp \
    ../../ESGamma/source/utility/elysian_force_generator.cpp \
    ../../ESGamma/source/utility/elysian_game_input.cpp \
    ../../ESGamma/source/utility/elysian_sort_sweep.cpp \
    source/core/tk_menu_stack.cpp \
    source/asset/tkRenderable.cpp \
    source/asset/tkCollidable.cpp \
    source/asset/tkRigidBody.cpp \
    source/asset/tkLightSource.cpp \
    source/asset/tkCombat.cpp \
    source/asset/tkParticleEmitter.cpp

HEADERS += include/asset/project.h \
    include/asset/assets.h \
    include/asset/assetManager.h \
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
    include/asset/elysianTileWrapper.h \
    include/dialog/luaExplorerDialog.h \
    include/core/rowsJoinerProxyModel.h \
    include/asset/luaTableStackModel.h \
    include/estk/engineGLSceneWidget.h \
    include/estk/tkMapSystem.h \
    include/estk/entitySelectable.h \
    include/core/tkDebug.h \
    include/asset/tkAnimation.h \
    include/asset/tkArea.h \
    include/asset/tkLevel.h \
    include/asset/tkEntity.h \
    include/asset/tkLuaTableProxy.h \
    include/estk/tkLuaWrappers.h \
    include/estk/tkLuaManager.h \
    include/asset/tkComponent.h \
    include/asset/tkRenderable.h \
	include/asset/tkCollidable.h \
    ../../AssetIO/include/component/elysian_animation_base.hpp \
    ../../AssetIO/include/component/elysian_audio_source_base.hpp \
    ../../AssetIO/include/component/elysian_behavior_base.hpp \
    ../../AssetIO/include/component/elysian_character_base.hpp \
    ../../AssetIO/include/component/elysian_collidable_base.hpp \
    ../../AssetIO/include/component/elysian_combat_base.hpp \
    ../../AssetIO/include/component/elysian_component_base.hpp \
    ../../AssetIO/include/component/elysian_examinable_base.hpp \
    ../../AssetIO/include/component/elysian_light_source_base.hpp \
    ../../AssetIO/include/component/elysian_lua_behavior.hpp \
    ../../AssetIO/include/component/elysian_particle_emitter_base.hpp \
    ../../AssetIO/include/component/elysian_renderable_base.hpp \
    ../../AssetIO/include/component/elysian_rigid_body_base.hpp \
    ../../AssetIO/include/component/elysian_warp_base.hpp \
    ../../AssetIO/include/crtp/elysian_animation.hpp \
    ../../AssetIO/include/crtp/elysian_area.hpp \
    ../../AssetIO/include/crtp/elysian_collidable.hpp \
    ../../AssetIO/include/crtp/elysian_component.hpp \
    ../../AssetIO/include/crtp/elysian_entity.hpp \
    ../../AssetIO/include/crtp/elysian_level.hpp \
    ../../AssetIO/include/crtp/elysian_lua_table_proxy.hpp \
    ../../AssetIO/include/crtp/elysian_renderable.hpp \
    ../../AssetIO/include/level/elysian_area_base.hpp \
    ../../AssetIO/include/level/elysian_dialog.hpp \
    ../../AssetIO/include/level/elysian_direction.hpp \
    ../../AssetIO/include/level/elysian_entity_base.hpp \
    ../../AssetIO/include/level/elysian_level_base.hpp \
    ../../AssetIO/include/level/elysian_level_manager.hpp \
    ../../AssetIO/include/level/elysian_map_depth_buffer.hpp \
    ../../AssetIO/include/level/elysian_tile.hpp \
    ../../AssetIO/include/level/elysian_zcoords.hpp \
    ../../AssetIO/include/primitive/elysian_color.hpp \
    ../../AssetIO/include/primitive/elysian_instance_transform.hpp \
    ../../AssetIO/include/primitive/elysian_material.hpp \
    ../../AssetIO/include/primitive/elysian_model_attrib.hpp \
    ../../AssetIO/include/primitive/elysian_prim_attrib.hpp \
    ../../AssetIO/include/primitive/elysian_quad_sprite_attrib.hpp \
    ../../AssetIO/include/primitive/elysian_renderable_object.hpp \
    ../../AssetIO/include/primitive/elysian_sprite_attrib.hpp \
    ../../AssetIO/include/primitive/elysian_textured_attrib.hpp \
    ../../AssetIO/include/utility/elysian_asset_typedefs.hpp \
    ../../AssetIO/include/utility/elysian_cstr_map.hpp \
    ../../AssetIO/include/utility/elysian_generic.hpp \
    ../../AssetIO/include/utility/elysian_lua_table_proxy_base.hpp \
    ../../AssetIO/include/utility/elysian_mem_pool.hpp \
    ../../AssetIO/include/utility/elysian_stl_algorithm.hpp \
    ../../AssetIO/include/utility/luatables.h \
    ../../ESGamma/include/crtp/elysian_debug.hpp \
    ../../ESGamma/include/crtp/elysian_lua_manager.hpp \
    ../../ESGamma/include/crtp/elysian_map_system.hpp \
    ../../ESGamma/include/gui/elysian_adventure_log.hpp \
    ../../ESGamma/include/gui/elysian_font.hpp \
    ../../ESGamma/include/lua/elysian_lua_manager_base.hpp \
    ../../ESGamma/include/lua/elysian_lua_script.hpp \
    ../../ESGamma/include/lua/elysian_lua_singleton.hpp \
    ../../ESGamma/include/lua/elysian_lua_wrappers.hpp \
    ../../ESGamma/include/system/elysian_animation_system.hpp \
    ../../ESGamma/include/system/elysian_audio_system.hpp \
    ../../ESGamma/include/system/elysian_behavior_system.hpp \
    ../../ESGamma/include/system/elysian_camera_system.hpp \
    ../../ESGamma/include/system/elysian_character_system.hpp \
    ../../ESGamma/include/system/elysian_collision_system.hpp \
    ../../ESGamma/include/system/elysian_combat_system.hpp \
    ../../ESGamma/include/system/elysian_day_system.hpp \
    ../../ESGamma/include/system/elysian_dialog_system.hpp \
    ../../ESGamma/include/system/elysian_engine.hpp \
    ../../ESGamma/include/system/elysian_examinable_system.hpp \
    ../../ESGamma/include/system/elysian_gui_system.hpp \
    ../../ESGamma/include/system/elysian_level_system.hpp \
    ../../ESGamma/include/system/elysian_light_system.hpp \
    ../../ESGamma/include/system/elysian_map_system_base.hpp \
    ../../ESGamma/include/system/elysian_particle_system.hpp \
    ../../ESGamma/include/system/elysian_party_system.hpp \
    ../../ESGamma/include/system/elysian_rigid_body_system.hpp \
    ../../ESGamma/include/system/elysian_skeleton_system.hpp \
    ../../ESGamma/include/system/elysian_system.hpp \
    ../../ESGamma/include/system/elysian_timer_system.hpp \
    ../../ESGamma/include/system/elysian_warp_system.hpp \
    ../../ESGamma/include/utility/elysian_debug_base.hpp \
    ../../ESGamma/include/utility/elysian_event_monitor.hpp \
    ../../ESGamma/include/utility/elysian_force_generator.hpp \
    ../../ESGamma/include/utility/elysian_game_input.hpp \
    ../../ESGamma/include/utility/elysian_sort_sweep.hpp \
    ../../ESGamma/include/utility/elysian_subprocess_manager.hpp \
    ../../AssetIO/include/crtp/elysian_behavior.hpp \
    include/asset/tkBehavior.h \
    ../../AssetIO/include/crtp/elysian_particle_emitter.hpp \
    ../../AssetIO/include/crtp/elysian_character.hpp \
    include/asset/tkcharacter.h \
    ../../AssetIO/include/crtp/elysian_rigid_body.hpp \
    ../../AssetIO/include/crtp/elysian_audio_source.hpp \
    include/asset/tkaudiosource.h \
    ../../AssetIO/include/crtp/elysian_combat.hpp \
    ../../AssetIO/include/crtp/elysian_light_source.hpp \
    ../../AssetIO/include/crtp/elysian_examinable.hpp \
    include/asset/tkexaminable.h \
    ../../AssetIO/include/level/elysian_skeleton_base.hpp \
    ../../AssetIO/include/component/elysian_skeleton_base.hpp \
    ../../AssetIO/include/crtp/elysian_skeleton.hpp \
    include/asset/tkskeleton.h \
    ../../AssetIO/include/crtp/elysian_warp.hpp \
    include/asset/tkwarp.h \
    ../../ESGamma/include/gui/elysian_sprite.hpp \
    ../../ESGamma/include/gui/elysian_quad_sprite.hpp \
    include/core/tk_menu_stack.h \
    include/asset/tkRigidBody.h \
    include/asset/tkLightSource.h \
    include/asset/tkCombat.h \
    include/asset/tkParticleEmitter.h

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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Spine/qtcreator/Spine/release/ -lSpine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Spine/qtcreator/Spine/debug/ -lSpine
else:unix: LIBS += -L$$OUT_PWD/../../Spine/qtcreator/Spine/ -lSpine

INCLUDEPATH += $$PWD/../../Spine/qtcreator/Spine
DEPENDPATH += $$PWD/../../Spine/qtcreator/Spine

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/release/libSpine.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/debug/libSpine.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/release/Spine.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/debug/Spine.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/libSpine.a


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
                -limagehlp
                #-lvorbisfile.dll \
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
                #-llua52
}

DISTFILES +=
