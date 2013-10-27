# -------------------------------------------------
# Project created by QtCreator 2010-03-06T14:23:27
# And modified to all fuck by Falco ALL THE DAYS
# -------------------------------------------------
TARGET = ESToolkit
TEMPLATE = app
QT		+= opengl
QMAKE_CXXFLAGS += -std=c++11 #fuuuuck
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
    ../../libGyro/include/kinetic
INCLUDEPATH += include/asset \
    include/core \
    include/dialog \
    include/estk \
    include/sheetManager \
    include/tileEngine \
    include/widget \
    ../../AssetIO/include \
    ../../libGyro/include \
    ../../libGyro/include/kinetic
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
    source/core/debug.cpp \
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
    source/asset/animation.cpp \
    source/asset/character.cpp \
    source/asset/collidable.cpp \
    source/asset/enemy.cpp \
    source/asset/friendly.cpp \
    source/asset/item.cpp \
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
    ../../AssetIO/source/elysian_item.cpp \
    ../../AssetIO/source/elysian_friendly.cpp \
    ../../AssetIO/source/elysian_examinable.cpp \
    ../../AssetIO/source/elysian_entity.cpp \
    ../../AssetIO/source/elysian_enemy.cpp \
    ../../AssetIO/source/elysian_dialog.cpp \
    ../../AssetIO/source/elysian_component.cpp \
    ../../AssetIO/source/elysian_collidable.cpp \
    ../../AssetIO/source/elysian_character.cpp \
    ../../AssetIO/source/elysian_behavior.cpp \
    ../../AssetIO/source/elysian_asset.cpp \
    ../../AssetIO/source/elysian_area.cpp \
	../../AssetIO/source/elysian_animation.cpp \
	../../AssetIO/source/elysian_audio_source.cpp
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
    include/core/debug.h \
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
    include/sheetManager/sheetmanager.h \
    include/tileEngine/tileSelection.h \
    include/tileEngine/tileGroupUndoItem.h \
    include/tileEngine/tileGraphicsItem.h \
    include/tileEngine/sourceView.h \
    include/tileEngine/selectionItemGroup.h \
    include/tileEngine/destView.h \
    include/widget/historyDockWidget.h \
    include/asset/animation.h \
    include/asset/character.h \
    include/asset/collidable.h \
    include/asset/component.h \
    include/asset/enemy.h \
    include/asset/friendly.h \
    include/asset/item.h \
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
    ../../AssetIO/include/elysian_rect_attrib.hpp \
    ../../AssetIO/include/elysian_quad_sprite_attrib.hpp \
    ../../AssetIO/include/elysian_quad_attrib.hpp \
    ../../AssetIO/include/elysian_prim_attrib.hpp \
    ../../AssetIO/include/elysian_pool.hpp \
    ../../AssetIO/include/elysian_model_attrib.hpp \
    ../../AssetIO/include/elysian_map_depth_buffer.hpp \
    ../../AssetIO/include/elysian_lua_behavior.hpp \
    ../../AssetIO/include/elysian_linked_list.hpp \
    ../../AssetIO/include/elysian_level_manager.hpp \
    ../../AssetIO/include/elysian_level.hpp \
    ../../AssetIO/include/elysian_item.hpp \
    ../../AssetIO/include/elysian_instance_transform.hpp \
    ../../AssetIO/include/elysian_friendly.hpp \
    ../../AssetIO/include/elysian_examinable.hpp \
    ../../AssetIO/include/elysian_entity.hpp \
    ../../AssetIO/include/elysian_enemy.hpp \
    ../../AssetIO/include/elysian_direction.hpp \
    ../../AssetIO/include/elysian_dialog.hpp \
    ../../AssetIO/include/elysian_cstr_map.hpp \
    ../../AssetIO/include/elysian_component.hpp \
    ../../AssetIO/include/elysian_collidable.hpp \
    ../../AssetIO/include/elysian_character.hpp \
    ../../AssetIO/include/elysian_behavior.hpp \
    ../../AssetIO/include/elysian_asset.hpp \
    ../../AssetIO/include/elysian_area.hpp \
	../../AssetIO/include/elysian_animation.hpp \
	../../AssetIO/include/elysian_audio_source.hpp
FORMS += \
    form/estk/sheetdockwidget.ui \
    form/estk/selectiondockwidget.ui \
    form/estk/layerdockwidget.ui \
    form/estk/estoolkit.ui \
    form/estk/entitydockwidget.ui \
    form/estk/assetdockwidget.ui \
    form/sheetManager/sheetmanager.ui \
    form/widget/historyDockWidget.ui \
    form/dialog/engineSettings.ui \
    form/sheetManager/sheetManagerFileDockWidget.ui

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

