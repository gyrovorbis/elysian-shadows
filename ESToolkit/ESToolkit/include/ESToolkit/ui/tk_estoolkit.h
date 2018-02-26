#ifndef ESTOOLKIT_H
#define ESTOOLKIT_H

#include <gyro_system_api.h>
#include <QMainWindow>
#include <QUndoGroup>
#include <QDir>
#include <ESToolkit/core/tk_assets.h>
#include "elysian_lua_manager.hpp"
#include <crtp/elysian_lua_variant.hpp>

#define ESTOOLKIT_VERSION				GYRO_LIB_VERSION
#define ESTOOLKIT_VERSION_NAME          "\"Redemption\""
#define ESTOOLKIT_OUTPUT_DIR            "ESTk"
#define ESTOOLKIT_SCREENS_DIR           "screenshots"
#define ESTOOLKIT_CRASH_CMD_LINE_ARG    "--crash"
#define ESTOOLKIT_SETTINGS_FILE         "estk_settings.ini"
#define ESTOOLKIT_EVMU_BIOS_FILE_PATH   "/assets/bin/vmu_bios_jap.bin"
#define ESTOOLKIT_STATUSBAR_MSG_TIMEOUT 2000

ELYSIAN_ASSET_TYPEDEF(Debug)

class QToolButton;
class QActionGroup;
class QUndoGroup;
class QOpenGLDebugLogger;
class QOpenGLDebugMessage;

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
class EngineGLSceneWidget;
class CameraDockWidget;
class GitlabDialog;
class HelpDockWidget;
class VMUMainWindow;
class TkArea;

namespace Ui {
    class ESToolkit;
}

namespace tktest {
    class ESToolkitTest;
}

class ESToolkit : public QMainWindow, public elysian::LuaManager {
    friend class tktest::ESToolkitTest;
    Q_OBJECT
public:
                    ESToolkit(QWidget *parent = nullptr);
    virtual         ~ESToolkit(void);


    static ESToolkit*   getInstance(void);
    static void         select(const LuaVariant& ref);
    static int          pushSelection(lua_State*const vm);
    static void         searchDocs(QString query);
    static void         openDoc(QString page, QString section=QString());
    static void         showStatusBarMessage(QString message);

    bool            verifySingleArea(const QString path);
    bool            openProjectDir(const QString dir);
    Assets&         getAssets(void);
    void            showGitlabBugReportTab(void) const;
    void            setOutputDir(const QDir dir);
    QDir            getOutputDir(void) const;
    void            setIndexHiddenLevels(const bool val);
    AssetDockWidget* getAssetDockWidget(void) const;

protected:
    virtual void    changeEvent(QEvent *e) override;
    //virtual bool    event(QEvent* event) override;
    virtual void    closeEvent(QCloseEvent *event) override;

    void            _onGYSysSegFault();
    void            _libGyroInitialize(void);
    void            _libGyroUninitialize(void);
    void            _engineLoadAssets(void);
    void            setMainTextureUniform(const int uniform);

    void            _repopulateTabbar(void);
    void            _addAreaToTabBar(const Area* area);

private:

    static ESToolkit* _instance;

    struct EngineParameters {
        bool        shadowPassEnabled       = true;
        bool        lightingEnabled         = true;
        bool        bloomEnabled            = false;
        bool        manualCameraEnabled     = false;
        bool        audioEnabled            = true;
        bool        showHiddenLevels        = false;
    } _engineParams;

    Assets                  _assets;
    QUndoGroup              _undoGroup;
    EngineGLSceneWidget*    _engineGLView       = nullptr;
    bool                    _libGyroInitialized = false;

    elysian::Debug *        _debug;
    QDir                    _outputDir;
    QString                 _prevProjectPath;
    QString                 _prevLevelPath;
    QString                 _prevAreaPath;

    Ui::ESToolkit*          _ui;
    QTabBar*                _areaTabBar;
    QMenu*                  _filterMenu     = nullptr;

    AssetDockWidget*        _assetDockWidget;
    EntityDockWidget*       _entityDockWidget;
    LayerDockWidget*        _layerDockWidget;
    SheetDockWidget*        _sheetDockWidget;       //Not used anymore
    SelectionDockWidget*    _selectionDockWidget;
    HistoryDockWidget*      _historyDockWidget;
    CameraDockWidget*       _cameraDockWidget;
    QDockWidget*            _bottomDockWidget;
    QDockWidget*            _vmuDockWidget;
    QTabWidget*             _bottomTabWidget;

    VMUMainWindow*          _evmuWindow = nullptr;
    EngineDialog*           _engineDialog;
    EngineSettingsDialog*   _engineSettingsDialog;
    SheetManager*           _sheetManager;
    GitlabDialog*           _gitlabDialog;
    HelpDockWidget*         _helpDockWidget;

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

    void toggleProjectActions(bool enabled);
    bool MaybeSave();
    void WriteSettings();
    void ReadSettings();
    void ResetViewChecks();

    virtual void keyPressEvent(QKeyEvent *event) override;

private Q_SLOTS:

    //----CONNECTED BY DESIGNER----
    void _onEngineInitialized();
    void on_actionLevelReloadSheets_triggered();
    void on_actionLevelRemove_triggered();
    void on_actionAreaRemove_triggered();
    void on_actionEntityRemove_triggered();
    void on_actionDebugViewLog_triggered();
    void on_actionAreaAddEntity_triggered();
    void on_actionFileSaveCurrentArea_triggered();
    void on_actionFileSaveCurrentLevel_triggered();
    void on_actionFileSaveCurrentProject_triggered();
    void on_actionFileOpenProject_triggered();
    void on_actionFileQuit_triggered();

    //----CONNECTED MANUALLY----
    void onAltTabShortcutTriggered(void);
    void OnProjectRemoved();
    void onEntityTreeWidgetLayerChange();
    void OnAddArea(Project *const project, Level *const level, Area *const area);
    void OnAreaRenamed(const QString oldName, const QString newName);
    void OnRemoveArea(Project *const project, Level *const level, Area *const area);
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
    void on_actionFileCloseProject_triggered();
    void on_actionEnable_OpenGL_triggered();
    void on_actionNix_triggered();
    void on_actionWindows_triggered();
    void on_actionMac_OS_X_triggered();
    void on_actionDreamcast_triggered();
    void on_actionIPhone_iPad_triggered();
    void on_actionInvoke_triggered();
    void onInvokeEngineStopped();
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
    void on_actionManual_Camera_toggled(bool arg1);
    void on_actionEnable_Audio_toggled(bool arg1);
    void on_actionReloadProject_triggered();
    void on_actionDocumentation_triggered();
    void on_actionGitLab_Manager_triggered();
    void on_actionPerformance_Monitor_triggered();
    void on_actionSheet_Manager_triggered();
    void on_actionClear_triggered();
    void on_actionReset_Stack_Depth_triggered();
    void on_actionCopy_triggered();
    void on_actionHidden_Levels_toggled(bool arg1);
    void on_actionIOS_Simulator_triggered();
    void on_actionAndroid_Device_triggered();
    void on_actionCamera_View_toggled(bool arg1);
    void on_actionInput_Device_Dump_triggered();
    void on_actionAdd_Player_triggered();
    void on_actionRemove_Player_triggered();
    void on_actionLua_Manager_Dump_triggered();
    void on_actionBloom_toggled(bool arg1);
    void on_actionToggle_Fullscreen_toggled(bool arg1);
    void on_actionSelect_All_triggered();
    void on_actionSelect_Eraser_triggered();
    void on_actionDelete_Selection_triggered();
    void on_actionPerformance_Monitor_Dump_triggered();
    void on_actionAudio_Dump_triggered();
    void on_actionTexture_Dump_triggered();
    void on_actionVMU_View_triggered(bool checked);
    void on_actionConnectEVMUTcp_triggered();
};








//===== INLINEZ =====

inline Assets& ESToolkit::getAssets(void) {
    return _assets;
}

inline void ESToolkit::setOutputDir(const QDir dir) { _outputDir = dir; }
inline QDir ESToolkit::getOutputDir(void) const { return _outputDir; }
inline ESToolkit* ESToolkit::getInstance(void) { return _instance; }
inline AssetDockWidget* ESToolkit::getAssetDockWidget(void) const { return _assetDockWidget; }

#endif // ESTOOLKIT_H
