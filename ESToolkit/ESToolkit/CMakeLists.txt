project(ESToolkit CXX)

cmake_minimum_required(VERSION 2.8.11)

#
# Set up some variables
#

set(EST_ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(EST_SOURCE_DIR "${EST_ROOT_DIR}/source")
set(EST_INCLUDE_DIR "${EST_ROOT_DIR}/include")
set(EST_FORM_DIR "${EST_ROOT_DIR}/form")
set(EST_RESOURCE_DIR "${EST_ROOT_DIR}/resource")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Check if environment variable QTDIR is set
if (DEFINED ENV{QTDIR})
    set(CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH})
else ()
    message (FATAL_ERROR "ERROR: Environment variable QTDIR is not set. Please locate your Qt5 folder.")
endif ()

#
# Find Qt
#

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

include_directories(${Qt5Widgets_INCLUDES} ${Qt5OpenGL_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5OpenGL_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")

#
# ESToolkit
#

add_definitions(-DGYRO_PLATFORM=GYRO_PLATFORM_Qt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 -stdlib=libc++")
endif()

if(ES_TARGET_ARCH MATCHES "i386" OR ES_TARGET_ARCH MATCHES "x86_64")
    if(${GYRO_PLATFORM} EQUAL GYRO_PLATFORM_x86)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse")
    endif()
endif()

set(EST_SOURCES
    ${EST_SOURCE_DIR}/asset/project.cpp
    ${EST_SOURCE_DIR}/asset/level.cpp
    ${EST_SOURCE_DIR}/asset/entity.cpp
    ${EST_SOURCE_DIR}/asset/assets.cpp
    ${EST_SOURCE_DIR}/asset/assetManager.cpp
    ${EST_SOURCE_DIR}/asset/area.cpp
    ${EST_SOURCE_DIR}/core/undoItem.cpp
    ${EST_SOURCE_DIR}/core/treenode.cpp
    ${EST_SOURCE_DIR}/core/tileSheetUtilities.cpp
    ${EST_SOURCE_DIR}/core/selectable.cpp
    ${EST_SOURCE_DIR}/core/main.cpp
    ${EST_SOURCE_DIR}/core/fileUtilities.cpp
    ${EST_SOURCE_DIR}/core/debug.cpp
    ${EST_SOURCE_DIR}/estk/tileSelectable.cpp
    ${EST_SOURCE_DIR}/estk/sheetViewTileGraphicsItem.cpp
    ${EST_SOURCE_DIR}/estk/sheetView.cpp
    ${EST_SOURCE_DIR}/estk/sheetDockWidget.cpp
    ${EST_SOURCE_DIR}/estk/selectionDockWidget.cpp
    ${EST_SOURCE_DIR}/estk/mapView.cpp
    ${EST_SOURCE_DIR}/estk/layerDockWidget.cpp
    ${EST_SOURCE_DIR}/estk/estoolkit.cpp
    ${EST_SOURCE_DIR}/estk/entityTreeWidget.cpp
    ${EST_SOURCE_DIR}/estk/entityDockWidget.cpp
    ${EST_SOURCE_DIR}/estk/assetTreeWidget.cpp
    ${EST_SOURCE_DIR}/estk/assetDockWidget.cpp
    ${EST_SOURCE_DIR}/sheetManager/sheetManagerSourceView.cpp
    ${EST_SOURCE_DIR}/sheetManager/sheetManagerDestView.cpp
    ${EST_SOURCE_DIR}/sheetManager/SheetManagerAssetManager.cpp
    ${EST_SOURCE_DIR}/sheetManager/sheetmanager.cpp
    ${EST_SOURCE_DIR}/tileEngine/sourceView.cpp
    ${EST_SOURCE_DIR}/widget/historyDockWidget.cpp
    ${EST_SOURCE_DIR}/asset/animation.cpp
    ${EST_SOURCE_DIR}/asset/character.cpp
    ${EST_SOURCE_DIR}/asset/collidable.cpp
    ${EST_SOURCE_DIR}/asset/enemy.cpp
    ${EST_SOURCE_DIR}/asset/friendly.cpp
    ${EST_SOURCE_DIR}/asset/item.cpp
    ${EST_SOURCE_DIR}/asset/renderable.cpp
    ${EST_SOURCE_DIR}/asset/warp.cpp
    ${EST_SOURCE_DIR}/core/propertyStackModel.cpp
    ${EST_SOURCE_DIR}/dialog/engineDialog.cpp
    ${EST_SOURCE_DIR}/dialog/engineSettingsDialog.cpp
    ${EST_SOURCE_DIR}/sheetManager/sheetManagerFileDockWidget.cpp
    ${EST_SOURCE_DIR}/sheetManager/sheetManagerSourceTileGraphicsItem.cpp
    ${EST_SOURCE_DIR}/estk/tileSelectableSelectionViewDelegate.cpp
    ${EST_INCLUDE_DIR}/tileEngine/tileGraphicsItem.inl
    ${EST_INCLUDE_DIR}/tileEngine/destView.inl
    ${EST_INCLUDE_DIR}/tileEngine/tileSelection.inl
    ${EST_SOURCE_DIR}/sheetManager/SheetManagerDestTileGraphicsItem.cpp
    ${EST_SOURCE_DIR}/estk/mapViewTileGraphicsItem.cpp
    ${EST_SOURCE_DIR}/asset/mapEntry.cpp
)

set(EST_INCLUDES
    ${EST_INCLUDE_DIR}/asset/project.h
    ${EST_INCLUDE_DIR}/asset/level.h
    ${EST_INCLUDE_DIR}/asset/entity.h
    ${EST_INCLUDE_DIR}/asset/assets.h
    ${EST_INCLUDE_DIR}/asset/assetManager.h
    ${EST_INCLUDE_DIR}/asset/area.h
    ${EST_INCLUDE_DIR}/core/undoItem.h
    ${EST_INCLUDE_DIR}/core/treenode.h
    ${EST_INCLUDE_DIR}/core/tileSheetUtilities.h
    ${EST_INCLUDE_DIR}/core/selectionScene.h
    ${EST_INCLUDE_DIR}/core/selectable.h
    ${EST_INCLUDE_DIR}/core/fileUtilities.h
    ${EST_INCLUDE_DIR}/core/debug.h
    ${EST_INCLUDE_DIR}/estk/tileSelectable.h
    ${EST_INCLUDE_DIR}/estk/sheetViewTileGraphicsItem.h
    ${EST_INCLUDE_DIR}/estk/sheetView.h
    ${EST_INCLUDE_DIR}/estk/sheetDockWidget.h
    ${EST_INCLUDE_DIR}/estk/selectionDockWidget.h
    ${EST_INCLUDE_DIR}/estk/mapViewTileGraphicsItem.h
    ${EST_INCLUDE_DIR}/estk/mapView.h
    ${EST_INCLUDE_DIR}/estk/layerDockWidget.h
    ${EST_INCLUDE_DIR}/estk/estoolkit.h
    ${EST_INCLUDE_DIR}/estk/entityTreeWidget.h
    ${EST_INCLUDE_DIR}/estk/entityDockWidget.h
    ${EST_INCLUDE_DIR}/estk/assetTreeWidget.h
    ${EST_INCLUDE_DIR}/estk/assetDockWidget.h
    ${EST_INCLUDE_DIR}/sheetManager/sheetManagerTileSelection.h
    ${EST_INCLUDE_DIR}/sheetManager/sheetManagerSourceView.h
    ${EST_INCLUDE_DIR}/sheetManager/sheetManagerPlaceTileGroupUndoItem.h
    ${EST_INCLUDE_DIR}/sheetManager/sheetManagerDestView.h
    ${EST_INCLUDE_DIR}/sheetManager/SheetManagerAssets.h
    ${EST_INCLUDE_DIR}/sheetManager/SheetManagerAssetManager.h
    ${EST_INCLUDE_DIR}/tileEngine/tileSelection.h
    ${EST_INCLUDE_DIR}/tileEngine/tileGroupUndoItem.h
    ${EST_INCLUDE_DIR}/tileEngine/tileGraphicsItem.h
    ${EST_INCLUDE_DIR}/tileEngine/sourceView.h
    ${EST_INCLUDE_DIR}/tileEngine/selectionItemGroup.h
    ${EST_INCLUDE_DIR}/tileEngine/destView.h
    ${EST_INCLUDE_DIR}/widget/historyDockWidget.h
    ${EST_INCLUDE_DIR}/asset/animation.h
    ${EST_INCLUDE_DIR}/asset/character.h
    ${EST_INCLUDE_DIR}/asset/collidable.h
    ${EST_INCLUDE_DIR}/asset/component.h
    ${EST_INCLUDE_DIR}/asset/enemy.h
    ${EST_INCLUDE_DIR}/asset/friendly.h
    ${EST_INCLUDE_DIR}/asset/item.h
    ${EST_INCLUDE_DIR}/asset/renderable.h
    ${EST_INCLUDE_DIR}/asset/sprite.h
    ${EST_INCLUDE_DIR}/asset/warp.h
    ${EST_INCLUDE_DIR}/core/propertyStackModel.h
    ${EST_INCLUDE_DIR}/core/propertyStandardItem.h
    ${EST_INCLUDE_DIR}/dialog/engineDialog.h
    ${EST_INCLUDE_DIR}/dialog/engineSettingsDialog.h
    ${EST_INCLUDE_DIR}/sheetManager/SheetManagerDestTileGraphicsItem.h
    ${EST_INCLUDE_DIR}/sheetManager/sheetManagerFileDockWidget.h
    ${EST_INCLUDE_DIR}/sheetManager/sheetManagerSourceTileGraphicsItem.h
    ${EST_INCLUDE_DIR}/sheetManager/sheetManager.h
    ${EST_INCLUDE_DIR}/core/tileImage.h
    ${EST_INCLUDE_DIR}/estk/mapViewPlaceTileGroupUndoItem.h
    ${EST_INCLUDE_DIR}/core/checkeredAlphaRectGraphicsItem.h
    ${EST_INCLUDE_DIR}/estk/tileSelectableSelectionViewDelegate.h
    ${EST_INCLUDE_DIR}/estk/sheetViewColliderGraphicsItem.h
    ${EST_INCLUDE_DIR}/sheetManager/sheetManagerTileEntry.h
    ${EST_INCLUDE_DIR}/estk/mapViewDepthBufferGraphicsItem.h
    ${EST_INCLUDE_DIR}/asset/mapEntry.h
    ${EST_INCLUDE_DIR}/core/glUtilities.h
)

set(EST_FORMS
    ${EST_FORM_DIR}/estk/sheetdockwidget.ui
    ${EST_FORM_DIR}/estk/selectiondockwidget.ui
    ${EST_FORM_DIR}/estk/layerdockwidget.ui
    ${EST_FORM_DIR}/estk/estoolkit.ui
    ${EST_FORM_DIR}/estk/entitydockwidget.ui
    ${EST_FORM_DIR}/estk/assetdockwidget.ui
    ${EST_FORM_DIR}/sheetManager/sheetmanager.ui
    ${EST_FORM_DIR}/widget/historyDockWidget.ui
    ${EST_FORM_DIR}/dialog/engineSettings.ui
    ${EST_FORM_DIR}/sheetManager/sheetManagerFileDockWidget.ui
)

qt5_wrap_ui(EST_FORMS_GEN ${EST_FORMS})

set(EST_RESOURCES
    ${EST_RESOURCE_DIR}/resource.qrc
)

qt5_add_resources(EST_RESOURCES_GEN ${EST_RESOURCES})

include_directories(${EST_INCLUDE_DIR}
    ${EST_INCLUDE_DIR}/asset
    ${EST_INCLUDE_DIR}/core
    ${EST_INCLUDE_DIR}/dialog
    ${EST_INCLUDE_DIR}/estk
    ${EST_INCLUDE_DIR}/sheetManager
    ${EST_INCLUDE_DIR}/tileEngine
    ${EST_INCLUDE_DIR}/widget
)

if(APPLE)
    add_executable(ESToolkit MACOSX_BUNDLE
        ${EST_SOURCES}
        ${AssetIO_SOURCES}
        ${EST_INCLUDES}
        ${AssetIO_INCLUDES}
        ${EST_FORMS_GEN}
        ${EST_RESOURCES_GEN}
    )
    set_target_properties(ESToolkit PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "ESToolkit")
else()
    add_executable(ESToolkit
        ${EST_SOURCES}
        ${AssetIO_SOURCES}
        ${EST_INCLUDES}
        ${AssetIO_INCLUDES}
        ${EST_FORMS_GEN}
        ${EST_RESOURCES_GEN}
    )
endif()


qt5_use_modules(ESToolkit Widgets OpenGL)

install(TARGETS ESToolkit
    BUNDLE DESTINATION bin
    LIBRARY DESTINATION bin
    RUNTIME DESTINATION bin)
