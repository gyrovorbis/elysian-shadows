#ifndef ESTOOLKIT_H
#define ESTOOLKIT_H

#include <QMainWindow>
#include <QUndoGroup>
#include <gyro_system.h>
#include "assets.h"

#define ESTOOLKIT_VERSION				GYRO_LIB_VERSION

class QToolButton;
class QActionGroup;
class QUndoGroup;

namespace Ui {
	class ESToolkit;
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

namespace Ui {
    class ESToolkit;
}

class ESToolkit : public QMainWindow {
    Q_OBJECT
public:
    ESToolkit(QWidget *parent = 0);
    ~ESToolkit();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
private:
    Ui::ESToolkit *_ui;

    QTabBar *_areaTabBar;
    bool _ignoreAreaTabChange;

    AssetDockWidget *_assetDockWidget;
    EntityDockWidget *_entityDockWidget;
    LayerDockWidget *_layerDockWidget;
    SheetDockWidget *_sheetDockWidget;
    SelectionDockWidget *_selectionDockWidget;
    HistoryDockWidget *_historyDockWidget;

    EngineDialog *_engineDialog;
    EngineSettingsDialog *_engineSettingsDialog;
    SheetManager *_sheetManager;

    Assets _assets;

    QToolButton *m_activePlatform;
    QActionGroup *m_platformActionGroup;
    QActionGroup *m_transformActionGroup;

    QUndoGroup _undoGroup;

    void SyncEntityMenuBar(const Entity *const entity);
    void ToogleProjectActions(bool enabled);
    bool MaybeSave();
    void WriteSettings();
    void ReadSettings();
    void ResetViewChecks();

	virtual void keyPressEvent(QKeyEvent *event) override;

private slots:

    //----CONNECTED BY DESIGNER----
    void on_actionLevelReloadSheets_triggered();
	//void on_actionFileImportArea_triggered();
	//void on_actionFileImportLevel_triggered();
	//void on_actionFileSaveAsCurrentProject_triggered();
	//void on_actionFileExportCurrentLevel_triggered();
	//void on_actionFileExportCurrentArea_triggered();
    void on_actionLevelRemove_triggered();
    void on_actionAreaRemove_triggered();
    void on_actionEntityRemoveEnemy_triggered();
    void on_actionEntityRemoveFriendly_triggered();
    void on_actionEntityRemoveCharacter_triggered();
    void on_actionEntityRemoveItem_triggered();
    void on_actionEntityRemoveWarp_triggered();
    void on_actionEntityRemoveCollidable_triggered();
    void on_actionEntityRemoveAnimation_triggered();
    void on_actionEntityAttachEnemy_triggered();
    void on_actionEntityAttachFriendly_triggered();
    void on_actionEntityAttachCharacter_triggered();
    void on_actionEntityAttachItem_triggered();
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

};
#endif // ESTOOLKIT_H
