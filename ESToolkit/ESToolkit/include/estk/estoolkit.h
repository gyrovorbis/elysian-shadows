#ifndef ESTOOLKIT_H
#define ESTOOLKIT_H

#include <gyro_system_api.h>
#include <QMainWindow>
#include <QUndoGroup>
#include "assets.h"
#include "elysian_lua_manager.hpp"
#include "elysian_asset_typedefs.hpp"

#define ESTOOLKIT_VERSION				GYRO_LIB_VERSION

ELYSIAN_ASSET_TYPEDEF(Debug)

class QToolButton;
class QActionGroup;
class QUndoGroup;

namespace Ui {
	class ESToolkit;
}

namespace elysian {
    class Engine;
}

class EngineDialog;
class AssetDockWidget;
class EntityDockWidget;
class LayerDockWidget;
class SheetDockWidget;
class SelectionDockWidget;
class HistoryDockWidget;
class SheetManager;
class EngineSettingsDialog;
class LuaExplorerDialog;
class EngineGLSceneWidget;

namespace Ui {
    class ESToolkit;
}

class ESToolkit : public QMainWindow, public elysian::LuaManager {
    Q_OBJECT
public:
                    ESToolkit(QWidget *parent = nullptr);
    virtual         ~ESToolkit(void);
protected:
    virtual void    changeEvent(QEvent *e) override;
    virtual void    closeEvent(QCloseEvent *event) override;
public:
    void            _engineInitialize(void);
    void            _engineLoadAssets(void);
    void            _openProjectDir(const QString dir);
    void            setMainTextureUniform(const int uniform);

    static void     inspectTable(OOLUA::Lua_table_ref table);
private:

    static ESToolkit* _instance;

    struct EngineParameters {
        bool        shadowPassEnabled   = true;
        bool        lightingEnabled     = true;
    } _engineParams;

    Assets                  _assets;
    QUndoGroup              _undoGroup;
    EngineGLSceneWidget*    _engineGLView;

    elysian::Debug *        _debug;

    Ui::ESToolkit*          _ui;

    QTabBar*                _areaTabBar;
    bool                    _ignoreAreaTabChange;

    AssetDockWidget*        _assetDockWidget;
    EntityDockWidget*       _entityDockWidget;
    LayerDockWidget*        _layerDockWidget;
    SheetDockWidget*        _sheetDockWidget;       //Not used anymore
    SelectionDockWidget*    _selectionDockWidget;
    HistoryDockWidget*      _historyDockWidget;
    QDockWidget*            _bottomDockWidget;
    QTabWidget*             _bottomTabWidget;

    EngineDialog*           _engineDialog;
    EngineSettingsDialog*   _engineSettingsDialog;
    SheetManager*           _sheetManager;
    LuaExplorerDialog*      _luaExplorerDialog;

    QToolButton*            m_activePlatform;
    QActionGroup*           m_platformActionGroup;
    QActionGroup*           m_transformActionGroup;

    enum MAP_VIEW_RES {
        MAP_VIEW_RES_480P,
        MAP_VIEW_RES_720P,
        MAP_VIEW_RES_DYNAMIC,
        MAP_VIEW_RES_MAX
    } _mapViewRes = MAP_VIEW_RES_DYNAMIC;

    void                _resizeMapView(void);
    void                _setMapViewResolution(const MAP_VIEW_RES res);
    void                _tkLuaInitialize(void);

    void SyncEntityMenuBar(const Entity *const entity);
    void toggleProjectActions(bool enabled);
    bool MaybeSave();
    void WriteSettings();
    void ReadSettings();
    void ResetViewChecks();

	virtual void keyPressEvent(QKeyEvent *event) override;

private slots:

    //----CONNECTED BY DESIGNER----
    void _onEngineInitialized();
    void on_actionLevelReloadSheets_triggered();
	//void on_actionFileImportArea_triggered();
	//void on_actionFileImportLevel_triggered();
	//void on_actionFileSaveAsCurrentProject_triggered();
	//void on_actionFileExportCurrentLevel_triggered();
	//void on_actionFileExportCurrentArea_triggered();
    void on_actionLevelRemove_triggered();
    void on_actionAreaRemove_triggered();
    void on_actionEntityRemoveCharacter_triggered();
    void on_actionEntityRemoveWarp_triggered();
    void on_actionEntityRemoveCollidable_triggered();
    void on_actionEntityRemoveAnimation_triggered();
    void on_actionEntityAttachCharacter_triggered();
    void on_actionEntityAttachWarp_triggered();
    void on_actionEntityAttachCollidable_triggered();
    void on_actionEntityAttachAnimation_triggered();
    void on_actionEntityRemove_triggered();
    void on_actionDebugViewLog_triggered();
    void on_actionAreaAddEntity_triggered();
    void on_actionFileSaveCurrentArea_triggered();
    void on_actionFileSaveCurrentLevel_triggered();
    void on_actionFileSaveCurrentProject_triggered();
    void on_actionFileOpenProject_triggered();
    void on_actionFileQuit_triggered();

    //----CONNECTED MANUALLY----
    void OnProjectRemoved();
    void onEntityTreeWidgetLayerChange();
    void OnAddArea(Project *const project, Level *const level, Area *const area);
	void OnAreaRenamed(const QString oldName, const QString newName);
    void OnRemoveArea(Project *const project, Level *const level, Area *const area);
    void OnComponentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
    void OnComponentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
    void OnAreaChanged(Area *const area);
    void OnLevelChanged(Level *const level);
    void OnProjectChanged(Project *const project);
    void OnTabChanged(int index);
    void OnSubLayerVisibilityChanged(int index, bool visible);
    void OnActiveLayerChanged(int index);
    void OnLayerChanged(Assets::LAYER_TYPE);
	void updateLayerTreeWidget(void);
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void OnSelectionChanged(const Selectable::TYPE, Selectable *const);
    void on_actionFileNewLevel_triggered();
    void on_actionFileNewArea_triggered();
    void on_actionEntityAttachRenderable_triggered();
    void on_actionEntityRemoveRenderable_triggered();
    void on_actionFileCloseProject_triggered();
    void on_actionEnable_OpenGL_triggered();
    void on_actionNix_triggered();
    void on_actionWindows_triggered();
    void on_actionMac_OS_X_triggered();
    void on_actionDreamcast_triggered();
    void on_actionIPhone_iPad_triggered();
    void on_actionInvoke_triggered();
    void on_actionAsset_View_triggered(bool checked);
    void on_actionEntity_View_triggered(bool checked);
    void on_actionLayer_View_triggered(bool checked);
    void on_actionSelection_View_triggered(bool checked);
    void on_actionSheet_View_triggered(bool checked);
    void on_actionReset_Layout_triggered();
    void on_actionHistory_View_triggered(bool checked);
    void UpdateDockWidgetChecked(int);
    void GiveMapViewFocus();
    void ExitPanMode();
    void on_actionCut_triggered();
    void on_actionPaste_triggered();
    void on_actionDefault_Transform_triggered();
    void on_actionToggle_AutoScroll_triggered();
    void on_actionActionSecret_triggered();
    void on_actionEngine_Settings_triggered();
	void on_actionElevation_toggled(bool arg1);
	void on_actionFlip_Rotation_toggled(bool arg1);
	void on_actionIndeces_toggled(bool arg1);
	void on_actionSolidity_toggled(bool arg1);
	void on_actionZ_Transitional_toggled(bool arg1);
	void on_actionHorizontal_SheetView_triggered(bool checked);
    void on_actionScreenshot_triggered();
    void on_actionScatter_triggered();
    void on_actionGather_triggered();
    void on_actionScatter_2_triggered();
    void on_actionGather_2_triggered();
	void on_actionPrevious_Project_triggered();
	void on_actionInvoke_triggered(bool checked);
	void on_actionAdvance_Frame_triggered();
    void on_actionClear_triggered();
    void on_actionLibGyro_toggled(bool arg1);
    void on_actionEngine_toggled(bool arg1);
    void on_actionToolkit_toggled(bool arg1);
    void on_actionLua_toggled(bool arg1);
    void on_actionQt_toggled(bool arg1);
    void on_actionCommands_toggled(bool arg1);
    void on_actionMap_Background_toggled(bool arg1);
    void on_action480p_toggled(bool arg1);
    void on_action720p_toggled(bool arg1);
    void on_actionDynamic_toggled(bool arg1);
    void on_actionReload_Shaders_triggered();
    void on_actionTilesheet_toggled(bool arg1);
    void on_actionNormalsheet_toggled(bool arg1);
    void on_actionSpecularsheet_toggled(bool arg1);
    void on_actionTile_Map_toggled(bool arg1);
    void on_actionNormal_Map_toggled(bool arg1);
    void on_actionSpecular_Map_toggled(bool arg1);
    void on_actionHidden_Entities_toggled(bool arg1);
    void on_actionReload_triggered();
    void on_actionReload_2_triggered();
    void on_actionShadows_toggled(bool arg1);
    void on_actionLights_toggled(bool arg1);
};
#endif // ESTOOLKIT_H
