#include <estk/tk_estoolkit.h>
#include "ui_tk_estoolkit.h"
#include "ui_tk_sheet_dock_widget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>

#include <asset/tk_assets.h>
#include <asset/tk_project.h>
#include <asset/tk_level.h>
#include <estk/tk_entity_selectable.h>
#include <estk/tk_selection_dock_widget.h>
#include <estk/tk_sheet_dock_widget.h>
#include <dialog/tk_engine_dialog.h>
#include <dialog/tk_engine_settings_dialog.h>
#include <estk/tk_asset_dock_widget.h>
#include <estk/tk_layer_dock_widget.h>
#include <estk/tk_entity_dock_widget.h>
#include <widget/tk_history_dock_widget.h>
#include <crtp/elysian_camera_system.hpp>
#include <system/elysian_engine.hpp>
#include <sheet_manager/tk_sheet_manager.h>
#include <estk/tk_engine_gl_scene_widget.h>
#include <estk/tk_entity_tree_widget.h>
#include <crtp/elysian_map_system.hpp>
#include <dialog/tk_gitlab_dialog.h>
#include <system/elysian_light_system.hpp>
#include <system/elysian_audio_system.hpp>
#include <dialog/tk_task_monitor_dialog.h>
#include <system/elysian_party_system.hpp>
#include <estk/tk_camera_dock_widget.h>
#include <crtp/elysian_input_system.hpp>
#include <core/tk_selection_scene.h>
#include <widget/tk_documentation_browser.h>
#include <proxy/elysian_entity_proxy.hpp>




void ESToolkit::on_actionFileQuit_triggered() {
    //Check for unsaved changes!!
    QCoreApplication::exit(0);
}

void ESToolkit::on_actionFileOpenProject_triggered(void) {
    openProjectDir(QFileDialog::getExistingDirectory(this, tr("Locate Project Directory"), "./", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

void ESToolkit::on_actionFileCloseProject_triggered(void) {
    //Q_EMIT CloseProject
    _assets.removeProject(_assets.getCurrentProject()->getNodeName());
}

void ESToolkit::on_actionFileSaveCurrentProject_triggered() {
    //check failure!
    Debug::Log(Debug::LOG_VERBOSE, "ESToolkit::on_actionFileSaveCurrentProject_triggered");
    Debug::push();
    _assets.getCurrentProject()->save();
    //_assets.getCurrentProject()->ClearUndoStacks();
}

void ESToolkit::on_actionFileSaveCurrentLevel_triggered() {
    //check failure!
    _assets.getCurrentLevel()->saveAll();
   // _assets.getCurrentLevel()->ClearAllStacks();
}

void ESToolkit::on_actionFileSaveCurrentArea_triggered() {
    _assets.getCurrentArea()->save();
    //Force TreeNode to update the '*' for unsaved area
    //_assets.getCurrentArea()->getUndoStack().setClean();
    //_assets.getCurrentArea()->onUndoCleanStateChanged(true);
}

void ESToolkit::on_actionAreaAddEntity_triggered() {
    Entity* entity = _assets.getCurrentArea()->newEntity();
    entity->setSaveFlag(true); //set to true by default!!!
    _assets.setCurrentSelection(new EntitySelectable(entity));
}

void ESToolkit::on_actionDebugViewLog_triggered()
{
    //Debug::getInstance().show();
}

void ESToolkit::on_actionEntityRemove_triggered() {
    if(QMessageBox::critical(nullptr, tr("Warning!"), tr("Are you SURE that you want to permanently delete the current entity?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    _assets.getCurrentArea()->deleteEntity((Entity *)_assets.getCurrentSelection());
}

void ESToolkit::on_actionAreaRemove_triggered() {
    if(QMessageBox::critical(nullptr, tr("Warning!"), tr("Are you SURE that you want to permanently delete the current area?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    if(_assets.getCurrentSelection() == _assets.getCurrentArea()) _selectionDockWidget->setSelectionModel(nullptr);

    auto* area = _assets.getCurrentArea();
    Level *level = area->getLevel();
    _assets.setCurrentArea(nullptr);
    level->removeArea(area->getNodeName());
    _assets.setCurrentArea(level->getFirstArea());

}


void ESToolkit::on_actionLevelRemove_triggered() {
    if(QMessageBox::critical(nullptr, tr("Warning!"), tr("Are you SURE that you want to permanently delete the current level?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    QString name = _assets.getCurrentLevel()->getNodeName();
    if(_assets.getCurrentSelection() == _assets.getCurrentLevel()) _selectionDockWidget->setSelectionModel(nullptr);
    _assets.setCurrentLevel(nullptr);
    _assets.getCurrentProject()->removeLevel(name);
    _assets.setCurrentLevel(_assets.getCurrentProject()->getFirstLevel());
}


void ESToolkit::on_actionLevelReloadSheets_triggered()
{
    _assets.getCurrentLevel()->reloadSheets();
}

void ESToolkit::on_actionUndo_triggered(){
    if(!_assets.getCurrentArea()) return;
    if(_assets.getCurrentArea()->getUndoStackCount() > 0) {
        _assets.getCurrentArea()->getUndoStack().undo();
    }
}

void ESToolkit::on_actionRedo_triggered() {
    if(!_assets.getCurrentArea())  {
        Debug::Log(Debug::LOG_WARNING, "Action Redo triggered, but there was no Area! Aborting function call...");
        return;
    }
    _assets.getCurrentArea()->getUndoStack().redo();
}

void ESToolkit::on_actionFileNewLevel_triggered() {

    //QMessageBox::critical(this, tr("Error!"), tr("This feature won't be implemented until ESTkV1.5.2. Do it manually. ;)"));
    Level *newLev = _assets.getCurrentProject()->newLevel();
    //_assets.getEngine()->getLevelSys()->addLevel(newLev);
    _assets.setCurrentArea(nullptr);
    _assets.setCurrentLevel(newLev);
    _assets.setCurrentSelection(newLev);
}

void ESToolkit::on_actionFileNewArea_triggered() {
    Area *newArea = _assets.getCurrentLevel()->newArea();
    _assets.setCurrentArea(newArea);
    _assets.setCurrentSelection(newArea);
}

void ESToolkit::on_actionEnable_OpenGL_triggered() {
//	if(!glUtilities::glSupported()) return;

return;
  //  _ui->mapView->setViewport((_ui->actionEnable_OpenGL->isChecked())? _engineGLView : new QWidget);
    _ui->mapView->setMouseTracking(true);
    _ui->mapView->viewport()->setMouseTracking(true);
   // _sheetDockWidget->getSourceView()->setViewport((_ui->actionEnable_OpenGL->isChecked())? new QOpenGLWidget  : new QWidget);
    _sheetDockWidget->getSourceView()->setMouseTracking(true);
    _sheetDockWidget->getSourceView()->viewport()->setMouseTracking(true);

   /* if(_ui->actionEnable_OpenGL->isChecked()) {
        _engine->getMapSys()->updateGL();
    }*/
}

void ESToolkit::on_actionNix_triggered()
{
    m_activePlatform->setIcon(_ui->actionNix->icon());
}

void ESToolkit::on_actionWindows_triggered()
{
    m_activePlatform->setIcon(_ui->actionWindows->icon());
}

void ESToolkit::on_actionMac_OS_X_triggered()
{
    m_activePlatform->setIcon(_ui->actionMac_OS_X->icon());
}

void ESToolkit::on_actionDreamcast_triggered()
{
    m_activePlatform->setIcon(_ui->actionDreamcast->icon());

}

void ESToolkit::on_actionIPhone_iPad_triggered()
{
    m_activePlatform->setIcon(_ui->actionIPhone_iPad->icon());
}

void ESToolkit::on_actionInvoke_triggered()
{

    if(!_assets.getCurrentArea() || !_assets.getCurrentLevel() || !_assets.getCurrentProject()) return;

    if(_ui->actionInvoke->isChecked()) {

        if(_ui->actionDreamcast->isChecked()) {
            _assets.getCurrentArea()->save();
            _engineDialog->show();

            if(!_engineDialog->Invoke(EngineDialog::ENGINE_DC)) {
                _engineDialog->close();
                _engineSettingsDialog->show();
                _engineSettingsDialog->setFocus();
                //Debug::Log(Debug::LOG_WARNING, "InvokeEngine(DREAMCAST) failed! Invalid Settings.");
            }
        }
        else if(_ui->actionIPhone_iPad->isChecked()) {
            _assets.getCurrentArea()->save();
            _engineDialog->show();

            _engineDialog->Invoke(EngineDialog::ENGINE_iOS);

        } else if(_ui->actionIOS_Simulator->isChecked()) {
            _assets.getCurrentArea()->save();
            _engineDialog->show();
            _engineDialog->Invoke(EngineDialog::ENGINE_iOS_Sim);
        } else if(_ui->actionAndroid_Device->isChecked()) {
            QMessageBox::information(nullptr, "Shit isn't done yet.", "I haven't gotten around to Droid integration. Get off my dick.");
            _engineDialog->Invoke(EngineDialog::ENGINE_Droid);
            //_ui->actionAndroid_Device->setChecked(false);
        }
    } else if(_engineDialog->isVisible()) {
        _engineDialog->kill();
        _engineDialog->close();
    }

}

void ESToolkit::on_actionAsset_View_triggered(bool checked)
{
    _assetDockWidget->setVisible(checked);
    _ui->actionAsset_View->setChecked(checked);
}

void ESToolkit::on_actionEntity_View_triggered(bool checked)
{
    _entityDockWidget->setVisible(checked);
    _ui->actionEntity_View->setChecked(checked);
}

void ESToolkit::on_actionLayer_View_triggered(bool checked)
{
    _layerDockWidget->setVisible(checked);
    _ui->actionLayer_View->setChecked(checked);
}

void ESToolkit::on_actionSelection_View_triggered(bool checked)
{
    _selectionDockWidget->setVisible(checked);
    _ui->actionSelection_View->setChecked(checked);
}

void ESToolkit::on_actionSheet_View_triggered(bool checked)
{

    _sheetDockWidget->setVisible(checked);
    _ui->actionSheet_View->setChecked(checked);
}

void ESToolkit::on_actionHistory_View_triggered(bool checked)
{
    _historyDockWidget->setVisible(checked);
    _ui->actionHistory_View->setChecked(checked);
}

void ESToolkit::on_actionReset_Layout_triggered()
{
    if(dockWidgetArea(_assetDockWidget) != Qt::LeftDockWidgetArea || _assetDockWidget->isFloating() || !_assetDockWidget->isVisible()) {
        _assetDockWidget->setFloating(false);
        removeDockWidget(_assetDockWidget);
        addDockWidget(Qt::LeftDockWidgetArea, _assetDockWidget);
        _assetDockWidget->setVisible(true);
        _ui->actionAsset_View->setChecked(true);
    }

    if(dockWidgetArea(_entityDockWidget) != Qt::LeftDockWidgetArea || _entityDockWidget->isFloating() || !_entityDockWidget->isVisible()) {
        _entityDockWidget->setFloating(false);
        removeDockWidget(_entityDockWidget);
        addDockWidget(Qt::LeftDockWidgetArea, _entityDockWidget);
        _entityDockWidget->setVisible(true);
        _ui->actionEntity_View->setChecked(true);
    }

    if(dockWidgetArea(_layerDockWidget) != Qt::LeftDockWidgetArea || _layerDockWidget->isFloating() || !_layerDockWidget->isVisible()) {
        _layerDockWidget->setFloating(false);
        removeDockWidget(_layerDockWidget);
        addDockWidget(Qt::LeftDockWidgetArea, _layerDockWidget);
        _layerDockWidget->setVisible(true);
        _ui->actionLayer_View->setChecked(true);
    }

    if(dockWidgetArea(_sheetDockWidget) != Qt::BottomDockWidgetArea || _sheetDockWidget->isFloating() || !_sheetDockWidget->isVisible()) {
        _sheetDockWidget->setFloating(false);
        removeDockWidget(_sheetDockWidget);
        addDockWidget(Qt::BottomDockWidgetArea, _sheetDockWidget);
        _sheetDockWidget->setVisible(true);
        _ui->actionSheet_View->setChecked(true);
    }

    if(dockWidgetArea(_selectionDockWidget) != Qt::RightDockWidgetArea || _selectionDockWidget->isFloating() || !_selectionDockWidget->isVisible()) {
        _selectionDockWidget->setFloating(false);
        removeDockWidget(_selectionDockWidget);
        addDockWidget(Qt::RightDockWidgetArea, _selectionDockWidget);
        _selectionDockWidget->setVisible(true);
        _ui->actionSelection_View->setChecked(true);
    }

    if(dockWidgetArea(_historyDockWidget) != Qt::RightDockWidgetArea || _historyDockWidget->isFloating() || !_historyDockWidget->isVisible()) {
        _historyDockWidget->setFloating(false);
        removeDockWidget(_historyDockWidget);
        addDockWidget(Qt::RightDockWidgetArea, _historyDockWidget);
        _historyDockWidget->setVisible(true);
        _ui->actionHistory_View->setChecked(true);
    }


}

void ESToolkit::on_actionCut_triggered()
{
    _ui->mapView->selectionCut();
}

void ESToolkit::on_actionCopy_triggered()
{
    if(_ui->mapView->hasFocus()) {
        _ui->mapView->selectionCopy();
    }
}

void ESToolkit::on_actionPaste_triggered()
{
    _ui->mapView->selectionPaste();
}

void ESToolkit::on_actionDefault_Transform_triggered()
{
    elysian::CameraKeyFrame keyFrame;
    unsigned w, h;
    gyVidResolution(&w, &h);
    keyFrame.setPos(w*0.5f, h*0.5f);
    _assets.getEngine()->getCameraSys()->moveToKeyFrame(keyFrame);
    _assets.getEngine()->getCameraSys()->setStaticScale(1.0f);
    //_ui->mapView->defaultTransform();

}

void ESToolkit::on_actionToggle_AutoScroll_triggered()
{
    _ui->mapView->autoScrollToggle(_ui->actionToggle_AutoScroll->isChecked());
    _sheetDockWidget->getSourceView()->autoScrollToggle(_ui->actionToggle_AutoScroll->isChecked());

}

void ESToolkit::on_actionActionSecret_triggered()
{
    _sheetManager->initialize(_ui->actionEnable_OpenGL->isChecked(), _ui->actionToggle_AutoScroll->isChecked());
    _sheetManager->setFocus();
    _sheetManager->showMaximized();
}

void ESToolkit::on_actionEngine_Settings_triggered() {
    _engineSettingsDialog->show();
    _engineSettingsDialog->setFocus();
}

void ESToolkit::on_actionElevation_toggled(bool arg1)
{
    if(arg1) {
        _ui->actionIndeces->setChecked(false);
        _ui->actionFlip_Rotation->setChecked(false);
    }
    TileEntry::renderElev = arg1;
    Area::updateScene();
}

void ESToolkit::on_actionFlip_Rotation_toggled(bool arg1)
{
    if(arg1) {
        _ui->actionIndeces->setChecked(false);
        _ui->actionElevation->setChecked(false);
    }
    TileEntry::renderFlipRot = arg1;
    Area::updateScene();
}

void ESToolkit::on_actionIndeces_toggled(bool arg1)
{
    if(arg1) {
        _ui->actionElevation->setChecked(false);
        _ui->actionFlip_Rotation->setChecked(false);
    }
    TileEntry::renderIndeces = arg1;

    Area::updateScene();
    Level::updateScenes();
}


void ESToolkit::on_actionSolidity_toggled(bool arg1)
{
    TileEntry::renderColliders = arg1;
    if(arg1) {
        _assets.getEngine()->getCollisionSys()->setRenderLayer((elysian::CollisionSystem::RENDER_LAYER)_assets.getCurrentLayer());
    } else {
       // _assets.getEngine()->getCollisionSys()->setRenderLayer(elysian::CollisionSystem::RENDER_DISABLED);
    }
    Area::updateScene();
    Level::updateScenes();
}

void ESToolkit::on_actionZ_Transitional_toggled(bool arg1)
{
    TileEntry::renderZTran = arg1;
    Area::updateScene();
}

void ESToolkit::on_actionHorizontal_SheetView_triggered(bool checked)
{
    //Initialize and set main scenes
    Level::initializeScene(static_cast<SourceView *>(_sheetDockWidget->getSourceView()), checked);
    _sheetDockWidget->SetSheetScene(Level::_sheetViewScene);
    if(_assets.getCurrentLevel()) {
        Level::addBgRectItem();
    }
}


void ESToolkit::on_actionScreenshot_triggered(void) {
        DbgEvent event("Dumping screenshot.");
#if 0
    //Area::editorScene->setSceneRect(Area::editorScene->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
    _ui->mapView->scale(0.1f, 0.1f);
    QImage image(Area::getEditorScene()->sceneRect().size().toSize(), QImage::Format_ARGB32);              // Create the image with the exact size of the shrunk scene
    image.fill(Qt::transparent);                                                                      // Start all pixels transparent

    QPainter painter(&image);
    Area::getEditorScene()->render(&painter);

    QDir dir("");
    if(!dir.exists("screenshots")) dir.mkdir("screenshots");
    QString fileBase("screenshots/estkScreenshot");
    unsigned num = 1;

    while(QFile::exists(fileBase+QString::number(num)+QString(".png"))) ++num;
    image.save(fileBase+QString::number(num)+QString(".png"));

    _ui->mapView->scale(10.0f, 10.0f);
#else

    QDir prevWorking(QDir::current());

    QDir dir(_outputDir);
    if(!dir.exists(ESTOOLKIT_SCREENS_DIR) ) {
        if(!dir.mkdir(ESTOOLKIT_SCREENS_DIR)) {
            errprintf("Could not create directory [%s]", Q_CSTR(dir.absoluteFilePath(ESTOOLKIT_SCREENS_DIR)));
            return;
        }
    }

    dir.cd(ESTOOLKIT_SCREENS_DIR);
    QDir::setCurrent(dir.absolutePath());

    QString fileBase("estkScreenshot");
    unsigned num = 1;

    while(QFile::exists(fileBase+QString::number(num)+QString(".png"))) ++num;
    QString filePath = fileBase+QString::number(num)+QString(".png");

    if(_engineGLView->grabFrameBuffer().save(filePath)) {
        dbgprintf("Screenshot saved [%s].", Q_CSTR(dir.absoluteFilePath(filePath)));
    } else {
        errprintf("Could not save screenshot [%s].", Q_CSTR(dir.absoluteFilePath(filePath)));
    }

    QDir::setCurrent(prevWorking.absolutePath());

#endif
}


void ESToolkit::on_actionScatter_triggered(void) {
    if(_assets.getCurrentLevel()) _assets.getCurrentLevel()->scatterTileNormalMaps();
}

void ESToolkit::on_actionGather_triggered(void) {
    if(_assets.getCurrentLevel()) _assets.getCurrentLevel()->gatherTileNormalMaps();
}

void ESToolkit::on_actionScatter_2_triggered(void) {
    if(_assets.getCurrentLevel()) _assets.getCurrentLevel()->scatterTileSpecularMaps();
}

void ESToolkit::on_actionGather_2_triggered(void) {
    if(_assets.getCurrentLevel()) _assets.getCurrentLevel()->gatherTileSpecularMaps();
}

void ESToolkit::on_actionPrevious_Project_triggered(void) {
    if(!_prevProjectPath.isEmpty()) {
        if(openProjectDir(_prevProjectPath)) {
            auto* level = _assets.getCurrentProject()->getLevel(_prevLevelPath);
            if(level) {
                _assets.setCurrentLevel(level);
                auto* area = _assets.getCurrentLevel()->getArea(_prevAreaPath);
                if(area) {
                    _assets.setCurrentArea(area);
                }
            }
        }
    }
}

void ESToolkit::on_actionInvoke_triggered(bool checked)
{
    if(checked && (_ui->actionWindows->isChecked() || _ui->actionMac_OS_X->isChecked() || _ui->actionNix->isChecked())) {
        _engineGLView->play();
    } else if(!checked) {
        _engineGLView->stop();
    }
}

void ESToolkit::on_actionAdvance_Frame_triggered()
{
    _engineGLView->advanceFrame();
}

void ESToolkit::on_actionMap_Background_toggled(bool arg1)
{
    TkMapSystem::setRenderMapBackground(arg1);
}

void ESToolkit::on_action480p_toggled(bool /*arg1*/)
{
    _setMapViewResolution(MAP_VIEW_RES_480P);
}

void ESToolkit::on_action720p_toggled(bool /*arg1*/)
{
    _setMapViewResolution(MAP_VIEW_RES_720P);
}

void ESToolkit::on_actionDynamic_toggled(bool /*arg1*/)
{
    _setMapViewResolution(MAP_VIEW_RES_DYNAMIC);
}

void ESToolkit::on_actionReload_Shaders_triggered(void) {
    gyShadersReloadAll();
}

void ESToolkit::on_actionTilesheet_toggled(bool /*arg1*/)
{
    setMainTextureUniform(0);
}

void ESToolkit::on_actionNormalsheet_toggled(bool /*arg1*/)
{
    setMainTextureUniform(1);
}

void ESToolkit::on_actionSpecularsheet_toggled(bool /*arg1*/)
{
    setMainTextureUniform(2);
}

void ESToolkit::on_actionTile_Map_toggled(bool arg1)
{
    gyVidDbgTexMappingEnabled(arg1);
}

void ESToolkit::on_actionNormal_Map_toggled(bool arg1)
{
    gyVidDbgNormalMappingEnabled(arg1);
}


void ESToolkit::on_actionSpecular_Map_toggled(bool arg1)
{
    gyVidDbgSpeculaMappingEnabled(arg1);
}

void ESToolkit::on_actionHidden_Entities_toggled(bool arg1)
{
    _entityDockWidget->getTreeWidget()->setShowHidden(arg1);
}

void ESToolkit::on_actionReload_triggered()
{
    _assets.reloadArea();
}

void ESToolkit::on_actionReload_2_triggered()
{
    _assets.reloadLevel();
}

void ESToolkit::on_actionShadows_toggled(bool arg1)
{
    _engineParams.shadowPassEnabled = arg1;
    if(_assets.getEngine()) {
        _assets.getEngine()->getLightSys()->setShadowPassEnabled(arg1);
    }
}

void ESToolkit::on_actionLights_toggled(bool arg1)
{
    _engineParams.lightingEnabled = arg1;
    if(_assets.getEngine()) {
        _assets.getEngine()->getLightSys()->setEnabled(arg1);
    }
    if(!arg1) {
        _ui->actionShadows->setEnabled(false);
        _ui->actionShadows->setChecked(false);
    } else {
        _ui->actionShadows->setEnabled(true);
    }
}

void ESToolkit::on_actionManual_Camera_toggled(bool arg1)
{
    _engineParams.manualCameraEnabled = arg1;
    if(_engineGLView) {
        _engineGLView->setCameraManualOnPlay(arg1);
    }
}

void ESToolkit::on_actionEnable_Audio_toggled(bool arg1)
{
    _engineParams.audioEnabled = arg1;
    if(_assets.getEngine()) {
        _assets.getEngine()->getAudioSys()->setEnabled(arg1);
    }

}


void ESToolkit::on_actionReloadProject_triggered(void)
{
    TkDebug::clear();
    //_libGyroUninitialize();
    //gyAudUninit();
   // gyAudInit();
    _assets.setCurrentSelection(nullptr);
    _assets.reloadProject();
}

void ESToolkit::on_actionDocumentation_triggered()
{
    _helpDockWidget->show();
}

void ESToolkit::on_actionGitLab_Manager_triggered()
{
    _gitlabDialog->show();
}

void ESToolkit::on_actionPerformance_Monitor_triggered()
{
    auto* dialog = new TaskMonitorDialog;
    if(_assets.getCurrentProject()) {
        dialog->setModel(_assets.getEngine()->getTimerSys());
        dialog->show();
    }
}

void ESToolkit::on_actionSheet_Manager_triggered()
{
    on_actionActionSecret_triggered();
}


void ESToolkit::on_actionClear_triggered()
{
    Debug::clear();
}

void ESToolkit::on_actionReset_Stack_Depth_triggered()
{
    Debug::resetStackDepth();
}


void ESToolkit::on_actionHidden_Levels_toggled(bool arg1)
{
    _engineParams.showHiddenLevels = arg1;
    if(_assets.getCurrentProject()) {
        _assets.getCurrentProject()->setIndexHiddenLevels(arg1);
    }
}

void ESToolkit::on_actionIOS_Simulator_triggered()
{
    m_activePlatform->setIcon(_ui->actionIOS_Simulator->icon());
}

void ESToolkit::on_actionAndroid_Device_triggered()
{
    m_activePlatform->setIcon(_ui->actionAndroid_Device->icon());
}

void ESToolkit::on_actionCamera_View_toggled(bool arg1) {
    if(arg1) {
        addDockWidget(Qt::LeftDockWidgetArea, _cameraDockWidget);
        _cameraDockWidget->show();
    } else {
        _cameraDockWidget->hide();
    }
}

void ESToolkit::on_actionInput_Device_Dump_triggered()
{
    if(_assets.getEngine()) {
        _assets.getEngine()->getInputSys()->dumpStats();
    }
}

void ESToolkit::on_actionAdd_Player_triggered()
{
    static int players = 1;

    Entity* leader = _assets.getEngine()->getPartySys()->getPartyLeader();
    if(!leader) {
        errprintf("No party leader to instantiate new player from!");
        return;
    }

    Entity* newPlayer = new Entity;
    newPlayer->fromTable(LuaVariant(leader));
    _assets.getCurrentArea()->addEntity(newPlayer);
    _assets.getEngine()->getPartySys()->addCharacterToParty(newPlayer);

    ++players;

}

void ESToolkit::on_actionRemove_Player_triggered()
{
  //  --players;


}

void ESToolkit::on_actionLua_Manager_Dump_triggered()
{
    elysian::LuaManager::dumpStats();
}

void ESToolkit::on_actionBloom_toggled(bool arg1)
{
    _engineParams.bloomEnabled = arg1;
    if(_assets.getEngine()) {
        _assets.getEngine()->getLightSys()->setBloomEnabled(arg1);
    }
}
