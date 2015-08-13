#include <QOpenGLContext>
#include <asset/tk_area.h>
#include <QSizePolicy>
#include <QToolButton>
#include <QCloseEvent>
#include <QSplitter>
#include <QSettings>
#include <QActionGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QDebug>
#include <gyro_video_api.h>
#include <tk_task_monitor_dialog.h>
#include <QOpenGLDebugLogger>
#include <tk_gitlab_dialog.h>
#include <QBrush>

#include <gyro_file_api.h>
#include "tk_engine_gl_scene_widget.h"

#include "elysian_debug.hpp"
#include <asset/tk_level.h>
#include "tk_assets.h"
#include <asset/tk_entity.h>
#include <tk_project.h>
#include <tk_estoolkit.h>
#include "ui_tk_estoolkit.h"
#include "ui_tk_sheet_dock_widget.h"
#include "tk_asset_manager.h"
#include "tk_engine_dialog.h"
#include "tk_tile_selectable.h"
#include <asset/tk_behavior.h>
#include "tk_sheet_manager.h"
#include "tk_engine_settings_dialog.h"
#include <tk_sheet_view_tile_graphics_item.h>
#include "tk_gl_utilities.h"
#include <tk_selection_scene.h>
#include <estk/tk_camera_dock_widget.h>

#include "tk_asset_dock_widget.h"
#include "tk_layer_dock_widget.h"
#include "tk_sheet_dock_widget.h"
#include "tk_entity_dock_widget.h"
#include "tk_history_dock_widget.h"
#include "tk_selection_dock_widget.h"

#include "elysian_engine.hpp"
#include "elysian_debug.hpp"
#include "tk_lua_table_stack_model.h"
#include "tk_lua_explorer_dialog.h"
#include "tk_rows_joiner_proxy_model.h"
#include "elysian_map_system.hpp"
#include <asset/tk_light_source.h>
#include "tk_entity_selectable.h"
#include "tk_entity_tree_widget.h"
#include "elysian_level_system.hpp"
#include "elysian_audio_system.hpp"
#include "tk_documentation_browser.h"
#include <QHelpContentWidget>
#include <QHelpIndexWidget>
#include <tk_gitlab_manager.h>

#ifdef ESTK_UNIT_TESTS
#include <QtTest>
#endif

ESToolkit* ESToolkit::_instance = nullptr;

ESToolkit::ESToolkit(QWidget *parent):
    QMainWindow(parent),
    _ui(new Ui::ESToolkit)
{
    _instance = this;
    // VERY IMPROTANT GLOBAL STATE/STATIC INITIALIZATION
        //elysian::Behavior::staticConstructor();
    Debug::getInstance();
    AssetManager::_assets = &_assets;
    //elysian::Behavior::staticConstructor();

    const QString verString = QString("ESTkv")+QString(ESTOOLKIT_VERSION)+QString(" ") + QString(ESTOOLKIT_VERSION_NAME);
    const char* platString =
#ifdef __APPLE__
    "OSX";
#elif defined(WIN32)
    "WINDOWS";
#else
    "LINUX";
#endif
    _libGyroInitialize();
    DbgEvent event("Initializing %s [%s]", verString.toStdString().c_str(), platString);

    _ui->setupUi(this);
    setWindowTitle(verString);

    //Instantiate external dialogs/tools
    _sheetManager           = new SheetManager(/*this*/);
    _engineDialog           = new EngineDialog(/*this*/);
    _engineSettingsDialog   = new EngineSettingsDialog(/*this*/);
    _luaExplorerDialog      = new LuaExplorerDialog(/*this*/);
    _gitlabDialog           = new GitlabDialog(/*this*/);
    _helpDockWidget         = new HelpDockWidget(/*this*/);
    connect(_engineSettingsDialog, SIGNAL(enginePathx86Changed(QString)), _engineDialog, SLOT(onEnginePathx86Changed(QString)));
    connect(_engineSettingsDialog, SIGNAL(dcIpChanged(QString)), _engineDialog, SLOT(OnDreamcastIpChanged(QString)));
    connect(_engineSettingsDialog, SIGNAL(dcMacChanged(QString)), _engineDialog, SLOT(OnDreamcastMacChanged(QString)));
    connect(_engineSettingsDialog, SIGNAL(sudoPswdChanged(QString)), _engineDialog, SLOT(OnSudoPswdChanged(QString)));

    _engineSettingsDialog->restoreSettings();

    _areaTabBar = new QTabBar();
    QGridLayout *boxLayout = qobject_cast<QGridLayout*>(_ui->centralWidget->layout());
    Q_ASSERT(boxLayout);
    //boxLayout->insertWidget(0, _areaTabBar, 0, Qt::AlignLeft);
    boxLayout->addWidget(_areaTabBar, 0, 0, Qt::AlignLeft | Qt::AlignCenter);
    boxLayout->addItem(new QSpacerItem(0, 0), 2, 0,  Qt::AlignBottom);

    // set action groups here
    m_platformActionGroup = new QActionGroup(_ui->menuSelect_Platform);
    m_platformActionGroup->addAction(_ui->actionNix);
    m_platformActionGroup->addAction(_ui->actionWindows);
    m_platformActionGroup->addAction(_ui->actionMac_OS_X);
    m_platformActionGroup->addAction(_ui->actionDreamcast);
    m_platformActionGroup->addAction(_ui->actionIPhone_iPad);
    m_platformActionGroup->addAction(_ui->actionIOS_Simulator);
    m_platformActionGroup->addAction(_ui->actionAndroid_Device);
    m_platformActionGroup->setExclusive(true);

    // add QToolButton dropdown for platforms
    m_activePlatform = new QToolButton();
    m_activePlatform->setToolTip("Select Platform...");
    m_activePlatform->setMenu(_ui->menuSelect_Platform);
    m_activePlatform->setPopupMode(QToolButton::MenuButtonPopup);

    // activate/disable platforms based on native OS
#ifdef Q_OS_WIN
        m_activePlatform->setIcon(_ui->actionWindows->icon());
        _ui->actionWindows->setChecked(true);
        _ui->actionMac_OS_X->setEnabled(false);
        _ui->actionNix->setEnabled(false);
#elif defined(Q_OS_LINUX)
        m_activePlatform->setIcon(_ui->actionNix->icon());
        _ui->actionNix->setChecked(true);
        _ui->actionMac_OS_X->setEnabled(false);
        _ui->actionWindows->setEnabled(false);
#elif defined(Q_OS_MAC)
        m_activePlatform->setIcon(_ui->actionMac_OS_X->icon());
        _ui->actionMac_OS_X->setChecked(true);
        _ui->actionWindows->setEnabled(false);
        _ui->actionNix->setEnabled(false);
#endif

    _ui->mainToolBar->addWidget(m_activePlatform);
    _ui->mainToolBar->addSeparator();
    _ui->mainToolBar->addAction(_ui->actionActionSecret);

    //Enable/Disable hardware acceleration based on OGL poll
    _ui->actionEnable_OpenGL->setEnabled(glUtilities::glSupported());

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // create/initialize ASSET dock and widget
    _assetDockWidget = new AssetDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, _assetDockWidget);

    // create/initialize ENTITY dock and widget
    _entityDockWidget = new EntityDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, _entityDockWidget);

    // create/initialize LAYER dock and widget
    _layerDockWidget = new LayerDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, _layerDockWidget);

    // create/initialize SHEET dock and widget
    _sheetDockWidget        = new SheetDockWidget;
    _sheetDockWidget->getUi()->sheetTabWidget->addTab(Debug::getCentralWidget(), "Console");
    addDockWidget(Qt::BottomDockWidgetArea, _sheetDockWidget);

    // create/initialize SELECTION dock and widget
    _selectionDockWidget = new SelectionDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, _selectionDockWidget);

    // create/initialize HISTORY dock and widget
    _historyDockWidget = new HistoryDockWidget(this);
    _historyDockWidget->SetActiveGroup(&_undoGroup);
    _historyDockWidget->hide();
    //addDockWidget(Qt::RightDockWidgetArea, _historyDockWidget);

    _cameraDockWidget = new CameraDockWidget(this);
    _cameraDockWidget->hide();

    // Initialize status bar
    QLabel *label = new QLabel;
    label->setPixmap(QPixmap(":/statusbar/images/absPosStatusIcon.png"));
    statusBar()->addWidget(label, 0);
    statusBar()->addWidget(_ui->mapView->getSceneMousePosLabel(), 1);

    label = new QLabel;
    label->setPixmap(QPixmap(":/statusbar/images/tilePosStatusIcon.png"));
    statusBar()->addWidget(label, 0);
    statusBar()->addWidget(_ui->mapView->getSceneMouseTileLabel(), 1);

    label = new QLabel;
    label->setPixmap(QPixmap(":/statusbar/images/selectionSizeStatusIcon.png"));
    statusBar()->addWidget(label, 0);
    statusBar()->addWidget(_ui->mapView->getSceneSelectionSizeLabel(), 1);

    label = new QLabel;
    label->setPixmap(QPixmap(":/statusbar/images/zoomStatusIcon.png"));
    statusBar()->addWidget(label, 0);
    statusBar()->addWidget(_ui->mapView->getZoomLabel(), 2);

    //Initialize and set main scenes
    Level::initializeScene(static_cast<SourceView *>(_sheetDockWidget->getSourceView()));
    Area::staticConstructor(_ui->mapView);
    _sheetDockWidget->SetSheetScene(Level::_sheetViewScene);
    _sheetDockWidget->getSourceView()->setEnabled(false);
    _ui->mapView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //_ui->mapView->setScene(Area::editorScene);
    _ui->mapView->setFocus();
    setFocusPolicy(Qt::ClickFocus);

    //Console->Filters menu
    _filterMenu = Debug::getFilterMenu();
    _ui->menuConsole->addMenu(Debug::getFilterMenu());

    //CONNECT ASSET SIGNALS/SLOTS TO MAIN UI
    connect(&_assets, SIGNAL(engineInitialize()), this, SLOT(_onEngineInitialized()));
    connect(&_assets, SIGNAL(projectRemoveUI()), this, SLOT(OnProjectRemoved()));
    connect(&_assets, SIGNAL(areaChanged(Area *const)), this, SLOT(OnAreaChanged(Area *const)));
    connect(&_assets, SIGNAL(levelChanged(Level *const)), this, SLOT(OnLevelChanged(Level *const)));
    connect(&_assets, SIGNAL(projectChanged(Project*const)), this, SLOT(OnProjectChanged(Project*const)));
    connect(&_assets, SIGNAL(areaAdded(Project*const,Level*const,Area*const)), this, SLOT(OnAddArea(Project*const,Level*const,Area*const)));
    connect(&_assets, SIGNAL(areaRenamed(QString,QString)), this, SLOT(OnAreaRenamed(QString,QString)));
    connect(&_assets, SIGNAL(areaRemoved(Project*const,Level*const,Area*const)), this, SLOT(OnRemoveArea(Project*const,Level*const,Area*const)));
    connect(&_assets, SIGNAL(componentAttached(Project*const,Level*const,Area*const,Entity*const,Component*const)), this, SLOT(OnComponentAttached(Project*const,Level*const,Area*const,Entity*const,Component*const)));
    connect(&_assets, SIGNAL(componentRemoved(Project*const,Level*const,Area*const,Entity*const,Component*const)), this, SLOT(OnComponentRemoved(Project*const,Level*const,Area*const,Entity*const,Component*const)));
    connect(&_assets, SIGNAL(selectionChanged(const Selectable::TYPE, Selectable *const)), this, SLOT(OnSelectionChanged(const Selectable::TYPE, Selectable *const)));
    connect(_areaTabBar, SIGNAL(currentChanged(int)),this, SLOT(OnTabChanged(int)));
    connect(_layerDockWidget, SIGNAL(LayerVisibilityChanged(int, bool)), this, SLOT(OnSubLayerVisibilityChanged(int, bool)));
    connect(_layerDockWidget, SIGNAL(ActiveLayerChanged(int)), this, SLOT(OnActiveLayerChanged(int)));
    connect(_layerDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
    connect(_assetDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
    connect(_entityDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
    connect(_entityDockWidget->getTreeWidget(), SIGNAL(entityLayerActivate()), this, SLOT(onEntityTreeWidgetLayerChange()));
    connect(_sheetDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
    connect(_sheetDockWidget, SIGNAL(ReleaseFocus()), this, SLOT(GiveMapViewFocus()));
    connect(_sheetDockWidget, SIGNAL(ExitPanMode()), this, SLOT(ExitPanMode()));
    connect(_historyDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
    connect(_selectionDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
    connect(_engineDialog, SIGNAL(stopped()), this, SLOT(onInvokeEngineStopped()));

    glUtilities::printGLSupport();
    ReadSettings();

    setMouseTracking(true);
}

ESToolkit::~ESToolkit(void) {
    _assets.setCurrentArea(nullptr);
    _assets.setCurrentProject(nullptr);
    _assets.setCurrentLevel(nullptr);
    _areaTabBar->disconnect();
    _assetDockWidget->disconnect();
    _assets.disconnect();

    delete _helpDockWidget;
    delete _gitlabDialog;
    delete _sheetManager;
    delete _engineDialog;
    delete _engineSettingsDialog;
    delete _luaExplorerDialog;
    delete _ui;
}

void ESToolkit::_libGyroInitialize(void) {
    /* The File subsystem needs to be initialized before anything else, ideally */
    _debug = &elysian::Debug::getInstance();
    char verString[50];

    _debug->parseArguments();
    strcpy(verString, "ESGamma v");
    strcat(verString, GYRO_LIB_VERSION);

#if 0
    GYVidInitParams vidParams =
#if GYRO_PLATFORM == GYRO_PLATFORM_x86 || GYRO_PLATFORM == GYRO_PLATFORM_Qt
    { (GY_VID_RESOLUTION)_debug->getCmdLineArgs().resolution, GY_WIN_RESIZABLE, 480, 270, 1280, 720, verString, "icon.bmp"};
#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
    { GY_VID_1280x720, GY_WIN_FULLSCREEN, 1280, 720, 1280, 720 };
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
    { 0 };
#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
    { 0 };
#else
    { 0 };

#endif
#endif
    GYSysInitParams sysParams = {new std::function<void()>([&](void) {
                        _gitlabDialog->show();
                  }),
                    [](void* ptr) {
                        if(_instance->isVisible()) { //Don't try to relaunch and shit if running in command-line mode!
                            QStringList cmdLineArgs = QApplication::arguments();
                            cmdLineArgs.push_back(ESTOOLKIT_CRASH_CMD_LINE_ARG);
                            if(_instance->_assets.getCurrentProject()) {
                                cmdLineArgs.push_back("--project");
                                cmdLineArgs.push_back(_instance->_assets.getCurrentProject()->getNodePath());
                            }
                             if(_instance->_assets.getCurrentLevel()) {
                                 cmdLineArgs.push_back("--level");
                                 cmdLineArgs.push_back(_instance->_assets.getCurrentLevel()->getNodePath());
                             }
                             if(_instance->_assets.getCurrentProject()) {
                                 cmdLineArgs.push_back("--area");
                                 cmdLineArgs.push_back(_instance->_assets.getCurrentArea()->getNodePath());
                             }

                            QProcess::startDetached(QCoreApplication::applicationFilePath(), cmdLineArgs);
                        }



                                 //else {
                          //  qWarning() << "INVISIBLE";
                       // }
//#ifdef ESTK_UNIT_TESTS

//#else
                        //QThread::currentThread()->terminate();
                        //QThread::currentThread()->wait();
                      //  if(QThread::currentThread() != qApp->thread()) {
                        //          qDebug() << "CHILD";
                                    // QThread::currentThread()->exit(-1);//pthread_kill(pthread_self(), 15);

                               //  }
                     //   else {
                                //     qDebug() << "parent";
                                   //  abort();
}
                                     //#endif
                        //(*static_cast<std::function<void()>*>(ptr))();
               //   }
            };
    gySysInit(&sysParams);
    //gyVidInit(&vidParams);
    gyAudInit();
    //gyTimerInit();

}

void ESToolkit::_engineLoadAssets(void) {

}


void ESToolkit::changeEvent(QEvent *e){
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        _ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ESToolkit::closeEvent(QCloseEvent *event) {
    WriteSettings();
    if(MaybeSave()) {
        //WriteSettings();
        /*
        if(_assets.getCurrentProject() != NULL || _assets.GetUndoStackCount() > 0) {

            QBox quitAlert(QMessageBox::Question, "Save Project?", "Would you like to save changes to your project before you quit?", QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No);
            int result = (int)quitAlert.exec();

            if(result == (int)QMessageBox::Cancel) {
                event->ignore();
                return;
            }

            if(result == (int)QMessageBox::Save) {
                qDebug() << "saving project...";
                _assets.getCurrentProject()->Save();
            }

            event->accept();
        }*/

        event->accept();
    }
    else {
        event->ignore();
    }

    //_assetDockWidget->clearTreeView();
    //MEMORY LEAK, BUT TEMPORARY FIX FOR DOUBLE DELETING AREA/SEVERITY IN ~TREEWIDGETITEM
    removeDockWidget(_assetDockWidget);
    _assetDockWidget->setParent(nullptr);

}

//Synch menuBar with the state of the currentEntity
void ESToolkit::SyncEntityMenuBar(const Entity *const /*entity*/) {
#if 0
    //Entity *entity = _assets.getCurrentEntity();

    if(!entity) {
        //Disable Entity manipulation
        _ui->actionEntityRemove->setDisabled(true);
        _ui->menuEntityAttach->setDisabled(true);
        _ui->menuEntityRemove->setDisabled(true);
        _ui->menuEntity->setDisabled(true);
        return;
    }

    //Reset Everything
    _ui->actionEntityRemove->setDisabled(false);
    _ui->menuEntityAttach->setDisabled(false);
    _ui->menuEntityRemove->setDisabled(false);
    _ui->menuEntity->setDisabled(false);

    _ui->actionEntityAttachRenderable->setDisabled(false);
    _ui->actionEntityAttachAnimation->setDisabled(false);
    _ui->actionEntityAttachCollidable->setDisabled(false);
    _ui->actionEntityAttachWarp->setDisabled(false);
    _ui->actionEntityAttachItem->setDisabled(false);
    _ui->actionEntityAttachCharacter->setDisabled(false);
    _ui->actionEntityAttachFriendly->setDisabled(false);
    _ui->actionEntityAttachEnemy->setDisabled(false);
    _ui->actionEntityAttachConversation->setDisabled(false);

    _ui->actionEntityRemoveRenderable->setDisabled(true);
    _ui->actionEntityRemoveAnimation->setDisabled(true);
    _ui->actionEntityRemoveCollidable->setDisabled(true);
    _ui->actionEntityRemoveItem->setDisabled(true);
    _ui->actionEntityRemoveWarp->setDisabled(true);
    _ui->actionEntityRemoveCharacter->setDisabled(true);
    _ui->actionEntityRemoveFriendly->setDisabled(true);
    _ui->actionEntityRemoveEnemy->setDisabled(true);
    _ui->actionEntityRemoveConversation->setDisabled(true);

    if(entity->getRenderable()) {
        _ui->actionEntityAttachRenderable->setDisabled(true);
        if(!entity->getAnimation()) {
            _ui->actionEntityRemoveRenderable->setDisabled(false);
        }
    }

    if(entity->getAnimation()) {
        _ui->actionEntityAttachAnimation->setDisabled(true);
        if(!entity->getCharacter() && !entity->getItem()) _ui->actionEntityRemoveAnimation->setDisabled(false);
    }

    if(entity->getCollidable()) {
        _ui->actionEntityAttachCollidable->setDisabled(true);
        if(!entity->getWarp() && !entity->getItem() && !entity->getCharacter()) _ui->actionEntityRemoveCollidable->setDisabled(false);
    }

    if(entity->getWarp()) {
        _ui->actionEntityAttachWarp->setDisabled(true);
        _ui->actionEntityRemoveWarp->setDisabled(false);
    }

    if(entity->getItem()) {
        _ui->actionEntityAttachItem->setDisabled(true);
        _ui->actionEntityRemoveItem->setDisabled(false);

    }

    if(entity->getConversation()) {
        _ui->actionEntityAttachConversation->setDisabled(true);
        _ui->actionEntityRemoveConversation->setDisabled(false);
    }

    if(entity->getCharacter()) {
        _ui->actionEntityAttachCharacter->setDisabled(true);
        if(!entity->getFriendly() && !entity->getEnemy()) {
            _ui->actionEntityRemoveCharacter->setDisabled(false);
        }
    }

    if(entity->getFriendly()) {
        _ui->actionEntityAttachFriendly->setDisabled(true);
        _ui->actionEntityRemoveFriendly->setDisabled(false);
    }

    if(entity->getEnemy()) {
        _ui->actionEntityAttachEnemy->setDisabled(true);
        _ui->actionEntityRemoveEnemy->setDisabled(false);
    }
#endif
}

void ESToolkit::toggleProjectActions(bool enabled)
{
    _ui->actionFileSaveCurrentProject->setEnabled(enabled);
    _ui->actionSelect->setEnabled(enabled);
    _ui->actionPan->setEnabled(enabled);
    _ui->actionZoom->setEnabled(enabled);
    _ui->actionActionSecret->setEnabled(enabled);
    _ui->actionCut->setEnabled(enabled);
    _ui->actionPaste->setEnabled(enabled);
    _ui->actionEnable_OpenGL->setEnabled(enabled && glUtilities::glSupported());
    _ui->actionInvoke->setEnabled(enabled);
    _ui->actionToggle_AutoScroll->setEnabled(enabled);
    _ui->actionHorizontal_SheetView->setEnabled(enabled);
    _ui->actionDefault_Transform->setEnabled(enabled);
    _ui->actionReset_Layout->setEnabled(enabled);
    _ui->actionAdvance_Frame->setEnabled(enabled);
    m_activePlatform->setEnabled(enabled);
    _ui->actionManual_Camera->setEnabled(enabled);
    _ui->actionEnable_Audio->setEnabled(enabled);
    _ui->actionReloadProject->setEnabled(enabled);

}

bool ESToolkit::MaybeSave()
{
    // implement "would you like to save changes first?"
    return true;
}

void ESToolkit::WriteSettings()
{
    QSettings settings;
    settings.setValue("settings", "Did Set");
    settings.setValue("geometry", saveGeometry());
    setWindowState(Qt::WindowMaximized);
    settings.setValue("state", saveState());
    settings.setValue("projectDir", _assets.getProjectDir());
    settings.setValue("assetDock", _assetDockWidget->isVisible());
    settings.setValue("entityDock", _entityDockWidget->isVisible());
    settings.setValue("layerDock", _layerDockWidget->isVisible());
    settings.setValue("sheetDock", _sheetDockWidget->isVisible());
    settings.setValue("selectionDock", _selectionDockWidget->isVisible());
    settings.setValue("glEnabled", _ui->actionEnable_OpenGL->isChecked());
    settings.setValue("autoScrollEnabled", _ui->actionToggle_AutoScroll->isChecked());
    settings.setValue("sheetViewHorizontal", _ui->actionHorizontal_SheetView->isChecked());
    settings.setValue("engineResolution", _engineDialog->getResolution());
    settings.setValue("renderMapBackground", TkMapSystem::getRenderMapBackground());
    settings.setValue("mapViewRes", _mapViewRes);
    settings.setValue("showHiddenEntities", _entityDockWidget->getTreeWidget()->getShowHidden());
    settings.setValue("shadowPassEnabled", _engineParams.shadowPassEnabled);
    settings.setValue("lightingEnabled", _engineParams.lightingEnabled);
    settings.setValue("manualCameraEnabled", _engineParams.manualCameraEnabled);
    settings.setValue("audioEnabled", _engineParams.audioEnabled);
    unsigned x, y;
    _engineDialog->getStartPos(x, y);
    settings.setValue("engineStartPosX", x);
    settings.setValue("engineStartPosY", y);
    settings.setValue("previousProject", _assets.getProjectDir());
    if(_gitlabDialog->isAuthenticated()) {
        settings.setValue("gitLabHostName", _gitlabDialog->getHostName());
        settings.setValue("gitLabPrivateToken", _gitlabDialog->getPrivateToken());
        settings.setValue("gitLabProjectId", _gitlabDialog->getProjectId());
    }
    unsigned source = 0;
    for(auto itSource : _filterMenu->actions()) {
        unsigned severity = 0;
        for(auto itSeverity : itSource->menu()->actions()) {
            char buff[30];
            sprintf(buff, "debugFilter_%d_%d", source, severity++);
            settings.setValue(buff, itSeverity->isChecked());
        }
        ++source;
    }
    settings.setValue("showHiddenLevels", _assets.getCurrentProject()? _assets.getCurrentProject()->getIndexHiddenLevels() : false);

}

void ESToolkit::ReadSettings()
{
    DbgEvent event("Restoring user settings.");
    toggleProjectActions(true);
    _assetDockWidget->setEnabled(false);
    _entityDockWidget->setEnabled(false);
    _layerDockWidget->setEnabled(false);
    _sheetDockWidget->setEnabled(false);
    _selectionDockWidget->setEnabled(false);
    _historyDockWidget->setEnabled(false);
    _ui->mapView->setEnabled(false);

    _ui->actionAsset_View->setChecked(true);
    _ui->actionEntity_View->setChecked(true);
    _ui->actionLayer_View->setChecked(true);
    _ui->actionSheet_View->setChecked(true);
    _ui->actionSelection_View->setChecked(true);
    _ui->actionHistory_View->setChecked(true);

    _assets.setCurrentProject(nullptr);

    QSettings settings;
    QString savedSettings = settings.value("settings", "").toString();
    QByteArray geo = settings.value("geometry", QByteArray()).toByteArray();
    QByteArray state = settings.value("state", QByteArray()).toByteArray();
    //QString dir = settings.value("projectDir", QString()).toString();
    bool sheetViewHorizontal = settings.value("sheetViewHorizontal", true).toBool();
    bool glEnabled = settings.value("glEnabled", true).toBool();
    bool autoScrollEnabled = settings.value("autoScrollEnabled", true).toBool();
    _engineDialog->setResolution(settings.value("engineResolution", 0).toUInt());
    bool renderMapBg = settings.value("renderMapBackground", false).toBool();
    unsigned x = settings.value("engineStartPosX", 0).toUInt();
    unsigned y = settings.value("engineStartPosY", 0).toUInt();
    bool showHiddenEntities = settings.value("showHiddenEntities", false).toBool();
    bool shadowPassEnabled = settings.value("shadowPassEnabled", true).toBool();
    bool manualCamera = settings.value("manualCameraEnabled", false).toBool();
    bool audioEnabled = settings.value("audioEnabled", true).toBool();
    bool lightingEnabled = settings.value("lightingEnabled", true).toBool();
    bool showHiddenLevels = settings.value("showHiddenLevels", false).toBool();
    _engineDialog->setStartPos(x, y);
    MAP_VIEW_RES mapViewRes = (MAP_VIEW_RES)settings.value("mapViewRes", MAP_VIEW_RES_DYNAMIC).toInt();
    _setMapViewResolution(mapViewRes);




    //bool assetVisibility = settings.value("assetDock", true).toBool();
    //bool entityVisibility = settings.value("entityDock", true).toBool();
    //bool layerVisibility = settings.value("layerDock", true).toBool();
    //bool sheetVisibility = settings.value("sheetDock", true).toBool();
    //bool selectionVisibility = settings.value("selectionDock", true).toBool();
/*
    if(dir=="") {
        TooeProjectActions(false);
        _assetDockWidget->setEnabled(false);
        _entityDockWidget->setEnabled(false);
        _layerDockWidget->setEnabled(false);
        _sheetDockWidget->setEnabled(false);
        _selectionDockWidget->setEnabled(false);
        _ui->mapView->setEnabled(false);
        _assets.setCurrentProject(NULL);
        return;
    }*/

    if(savedSettings == "") {
        return;
    }

    restoreState(state);
    restoreGeometry(geo);

    _ui->actionEnable_OpenGL->setChecked(glEnabled);
    on_actionEnable_OpenGL_triggered();
    _ui->actionToggle_AutoScroll->setChecked(autoScrollEnabled);
    on_actionToggle_AutoScroll_triggered();
    _ui->actionHorizontal_SheetView->setChecked(sheetViewHorizontal);
    _ui->actionMap_Background->setChecked(renderMapBg);
    on_actionHorizontal_SheetView_triggered(sheetViewHorizontal);
    _ui->actionHidden_Entities->setChecked(showHiddenEntities);
    _ui->actionShadows->setChecked(shadowPassEnabled);
    _ui->actionLights->setChecked(lightingEnabled);
    _ui->actionManual_Camera->setChecked(manualCamera);
    _ui->actionEnable_Audio->setChecked(audioEnabled);
    _ui->actionHidden_Levels->setChecked(showHiddenLevels);
    if(!settings.value("gitLabPrivateToken").isNull()) {
        _gitlabDialog->setHostName(settings.value("gitLabHostName").toString());
        _gitlabDialog->setPrivateToken(settings.value("gitLabPrivateToken").toString());
        _gitlabDialog->authenticate(true);
        _gitlabDialog->setProjectId(settings.value("gitLabProjectId").toInt());
    }

    unsigned source = 0;
    for(auto itSource : _filterMenu->actions()) {
        unsigned severity = 0;
        for(auto itSeverity : itSource->menu()->actions()) {
            char buff[30];
            sprintf(buff, "debugFilter_%d_%d", source, severity);
            const bool enabled = settings.value(buff, true).toBool();
            Debug::setFilter((Debug::TK_SOURCE)source, (Debug::SEVERITY)severity++, enabled);
            itSeverity->setChecked(enabled);
        }
        source++;
    }



    toggleProjectActions(false);

}

void ESToolkit::OnAreaRenamed(const QString /*oldName*/, const QString /*newName*/) {
    const bool blockSigs = _areaTabBar->blockSignals(true);

    while(_areaTabBar->count()) _areaTabBar->removeTab(0);
    for(unsigned int i = 0; i < _assets.getCurrentLevel()->getAreaAmount(); ++i) {
        _addAreaToTabBar(_assets.getCurrentLevel()->getAreaByIndex(i));
    }

    for(int i = 0; i < _areaTabBar->count(); ++i) {
        if(_areaTabBar->tabToolTip(i) == _assets.getCurrentArea()->getNodePath()) _areaTabBar->setCurrentIndex(i);
    }

    _areaTabBar->blockSignals(blockSigs);

}

void ESToolkit::OnAddArea(Project *const /*project*/, Level *const level, Area *const area) {
    static int i = 0;

    if(level && level == _assets.getCurrentLevel()) { //Don't bother updating the tabs otherwise
        _undoGroup.addStack(&area->getUndoStack());
        const bool blockSigs = _areaTabBar->blockSignals(true);
        _addAreaToTabBar(area);
        _areaTabBar->blockSignals(blockSigs);
    }
}

void ESToolkit::OnRemoveArea(Project *const /*project*/, Level *const level, Area *const area) {
    _undoGroup.removeStack(&area->getUndoStack());

    if(level == _assets.getCurrentLevel()) { //Don't bother updating the tabs otherwise
        _undoGroup.setActiveStack(nullptr);
        for(int i = 0; i < _areaTabBar->count(); ++i) {
            if(_areaTabBar->tabToolTip(i) == area->getNodePath()) {
                const bool blockSigs = _areaTabBar->blockSignals(true);
                _areaTabBar->setCurrentIndex(-1);
                _areaTabBar->removeTab(i);
                _areaTabBar->setCurrentIndex(-1);
                _areaTabBar->blockSignals(blockSigs);
                return;
            }
        }
    }
}

void ESToolkit::OnTabChanged(int index) {
    if(index == -1) return;
    if(_assets.getCurrentArea() && _assets.getCurrentArea()->getNodePath() == _areaTabBar->tabToolTip(index)) return;
    _assets.setCurrentArea(_areaTabBar->tabToolTip(index));
}

void ESToolkit::OnSubLayerVisibilityChanged(int index, bool visible)
{

    switch(index) {
    case 1:
        _assets.setLayerVisible(Assets::LAYER_LIGHT, visible);
        break;
    case 2:
        _assets.setLayerVisible(Assets::LAYER_TILE3, visible);
        break;
    case 3:
        _assets.setLayerVisible(Assets::LAYER_TILE2, visible);
        break;
    case 4:
        _assets.setLayerVisible(Assets::LAYER_TILE1, visible);
        break;
    case 5:
        TileEntry::renderColliders = visible;
        Level::updateScenes();
        break;
    case 6:
        Area::setInactiveTransparency(visible);
        break;
    case 7:
        TileEntry::renderElev = visible;
        if(visible) _assets.getEngine()->getMapSys()->setTileDecor(TkMapSystem::TILE_DECOR_ELEV);
        break;
    case 8:
        TileEntry::renderIndeces = visible;
        if(visible) _assets.getEngine()->getMapSys()->setTileDecor(TkMapSystem::TILE_DECOR_INDEX);
        Level::updateScenes();
        break;
    case 9:
        TileEntry::renderZTran = visible;
        break;
    case 10:
        TileEntry::renderFlipRot = visible;
        if(visible) _assets.getEngine()->getMapSys()->setTileDecor(TkMapSystem::TILE_DECOR_ORIENT);
        break;
    case 11:
        TileEntry::renderPlane = visible;
        if(visible) _assets.getEngine()->getMapSys()->setTileDecor(TkMapSystem::TILE_DECOR_PLANE);
        break;
    default:
        break;
    }

    if(!Area::getInactiveTransparency() && !TileEntry::renderElev && !TileEntry::renderIndeces && !TileEntry::renderFlipRot && !TileEntry::renderPlane)
        _assets.getEngine()->getMapSys()->setTileDecor(TkMapSystem::TILE_DECOR_NONE);

    _layerDockWidget->setCurrentLayer(_assets.getCurrentLayer());
    //Area::updateScene();
}

void ESToolkit::OnActiveLayerChanged(int index) {
   switch(index) {
    case 1:
        //_layerDockWidget->SetCurrentLayerIndex();
        OnLayerChanged(Assets::LAYER_ENTITY);
        break;
    case 2:
        OnLayerChanged(Assets::LAYER_TILE3);
        if(TileEntry::renderColliders) _assets.getEngine()->getCollisionSys()->setRenderLayer(elysian::CollisionSystem::RENDER_LAYER_3);
        break;
    case 3:
        OnLayerChanged(Assets::LAYER_TILE2);
        if(TileEntry::renderColliders) _assets.getEngine()->getCollisionSys()->setRenderLayer(elysian::CollisionSystem::RENDER_LAYER_2);
        break;
    case 4:
        OnLayerChanged(Assets::LAYER_TILE1);
        if(TileEntry::renderColliders) _assets.getEngine()->getCollisionSys()->setRenderLayer(elysian::CollisionSystem::RENDER_LAYER_1);
        break;
    default:
       //Fucking LayerTreeWidget just set the current item to an item
       //that shouldn't be selectable... set it back.
       QTimer::singleShot(10, this, SLOT(updateLayerTreeWidget()));
        break;
    }
    //Area::updateScene();
}

/*
  Sooooo fucking reested, but I can't find any other fucking way...
  QTreeWidget's currentItem can't be changed within the context
  of the itemChanged slot.
*/
void ESToolkit::updateLayerTreeWidget(void) {
    _layerDockWidget->setCurrentLayer(_assets.getCurrentLayer());
}

void ESToolkit::OnLayerChanged(Assets::LAYER_TYPE newLayer) {
#if 0
    const Assets::LAYER_TYPE oldLayer = _assets.getCurrentLayer();
    switch (newLayer) {
    case Assets::TERRAIN1_LAYER:
    case Assets::TERRAIN2_LAYER:
        _sheetDockWidget->SetSheetScene(Level::tilePanelScene);
        break;
    case Assets::OBJECT1_LAYER:
    case Assets::OBJECT2_LAYER:
        _sheetDockWidget->SetSheetScene(Level::objectPanelScene);
        break;
    case Assets::ENTITY_LAYER:
        break;
    default:
        break;
    }
#endif
    if(_assets.getCurrentLayer() == Assets::LAYER_ENTITY && _assets.getCurrentLayer() != newLayer) _assets.setCurrentSelection(nullptr);

    _assets.setCurrentLayer(newLayer);

    //Force update for sake of solidity
    //Area::updateScene();
#if 0
    //Only clear the selection if we're changing layer TYPES, so we can promote layers.
    if(Assets::getTileTypeFromLayerType(newLayer) != Assets::getTileTypeFromLayerType(oldLayer)) {
        _ui->mapView->deleteSelectionItemGroup();
        _assets.setCurrentSelection(NULL);
    }
#endif
}

void ESToolkit::_addAreaToTabBar(const TkArea *area) {
    const auto index = _areaTabBar->addTab(area->getNodeName());
    _areaTabBar->setTabToolTip(index, area->getNodePath());
    _areaTabBar->setTabTextColor(index, area->foreground(0).color());
}

void ESToolkit::OnAreaChanged(Area *const area) {
    if(area) {
        _ui->actionAreaAddEntity->setDisabled(false);
        _ui->actionAreaRemove->setDisabled(false);
        _ui->menuArea->setDisabled(false);
        _ui->actionFileSaveCurrentArea->setDisabled(false);
        _undoGroup.setActiveStack(&area->getUndoStack());
        _ui->mapView->setEnabled(true);
    } else {
        _ui->actionAreaAddEntity->setDisabled(true);
        _ui->actionAreaRemove->setDisabled(true);
        _ui->menuArea->setDisabled(true);
        _ui->actionFileSaveCurrentArea->setDisabled(true);
        _ui->mapView->setEnabled(false);
    }

    //Update tabs
    const bool sigs = _areaTabBar->blockSignals(true);
    if(area) {
        for(int i = 0; i < _areaTabBar->count(); ++i) {
            if(_areaTabBar->tabToolTip(i) == area->getNodePath()) {
                _areaTabBar->setCurrentIndex(i);
                _areaTabBar->setTabTextColor(i, area->foreground(0).color());
                break;
            }
        }
    } else {
        _areaTabBar->setCurrentIndex(-1);
    }
    _areaTabBar->blockSignals(sigs);


    _ui->mapView->deleteSelectionItemGroup();
    if(_ui->mapView->scene()) _ui->mapView->scene()->update();
}

void ESToolkit::OnLevelChanged(Level *const level) {

    if(level) {
        _ui->actionLevelReloadSheets->setDisabled(false);
        _ui->actionLevelRemove->setDisabled(false);
        _ui->menuLevel->setDisabled(false);
        _ui->actionFileSaveCurrentLevel->setDisabled(false);
        //_ui->mapView->createSelItemGroup();
        _ui->actionFileNewArea->setEnabled(true);
        _sheetDockWidget->setViewEnabled(true);

    } else {
        _ui->actionLevelReloadSheets->setDisabled(true);
        _ui->actionLevelRemove->setDisabled(true);
        _ui->menuLevel->setEnabled(false);
        _ui->actionFileSaveCurrentLevel->setDisabled(true);
        _ui->actionFileNewArea->setEnabled(false);
        _sheetDockWidget->setViewEnabled(false);
    }

    //Update tabs
    const bool blockSigs = _areaTabBar->blockSignals(true);
    if(level) {
        while(_areaTabBar->count()) _areaTabBar->removeTab(0);

        for(unsigned int i = 0; i < _assets.getCurrentLevel()->getAreaAmount(); ++i) {
            _addAreaToTabBar(_assets.getCurrentLevel()->getAreaByIndex(i));
        }
        //_areaTabBar->setCurrentIndex(0);
    }
    else {
        while(_areaTabBar->count()) _areaTabBar->removeTab(0);
    }
    _areaTabBar->blockSignals(blockSigs);

    _ui->mapView->deleteSelectionItemGroup();
    _sheetDockWidget->UpdateSheetView();
}
/*
void ESToolkit::OnEntityChanged(Entity *const entity) {
    //Update Menu Bar
    SyncEntityMenuBar();

    //Update Entity View
    _inspectorDock->setWidget((QWidget *)entity);

    //center on entity
    if(entity) _ui->mapView->centerOn(entity);
}*/

void ESToolkit::OnProjectChanged(Project *const project) {
    //Update menu bar
    if(project) {
        _ui->menuNew->setDisabled(false);
        _ui->menuClose->setDisabled(false);
        _ui->menuSave->setDisabled(false);
        project->setIndexHiddenLevels(_engineParams.showHiddenLevels);
    }
    else {
        _ui->menuNew->setDisabled(true);
        _ui->menuClose->setDisabled(true);
        _ui->menuSave->setDisabled(true);
    }
}

void ESToolkit::OnComponentAttached(Project *const /*project*/, Level *const /*level*/, Area *const area, Entity *const entity, Component *const /*comp*/) {
    if(area == _assets.getCurrentArea()) SyncEntityMenuBar(entity);
}

void ESToolkit::OnComponentRemoved(Project *const /*project*/, Level *const /*level*/, Area *const area, Entity *const entity, Component *const /*comp*/) {
    if(area == _assets.getCurrentArea()) SyncEntityMenuBar(entity);
}

//======== MENU BAR BAR SLOTS ========

void ESToolkit::on_actionFileQuit_triggered() {
    //Check for unsaved changes!!
    QCoreApplication::exit(0);
}

bool ESToolkit::openProjectDir(const QString dir) {
    DbgEvent dbg("Opening Project [%s]", Q_CSTR(dir));
    bool success = true;

    if(dir=="" || dir.isNull()) {
        success = false;
        return false;
    }

    if(_assets.getCurrentProject()) {
        QMessageBox::critical(this, "You dun fucked up!", "There's already a project open, asswipe.");
        success = false;
        return false;
    }

    if(Assets::validateProjectPath(dir)) {
        QDir::setCurrent(dir);
    }

        auto* viewport = _engineGLView = new EngineGLSceneWidget(this);


        //dipshitLayout->addWidget(viewport);
        //_engineGLView->setFormat(QSurfaceFormat::defaultFormat());
        connect(viewport, SIGNAL(engineUpdated()), _ui->mapView, SLOT(onEngineUpdated()));
    viewport->setMouseTracking(true);
    //_engineGLView->makeCurrent();
    _engineGLView->setMapView(_ui->mapView);
    _engineGLView->makeCurrent();
        _engineGLView->update();
        _engineGLView->show();
        _engineGLView->updateGL();
                _ui->mapView->setViewport(viewport);

   // _ui->mapView->setScene(new QGraphicsScene());
    //setCentralWidget(viewport);
    //assert(_ui->mapView->scene());

    GYInputInitParams inputParams = {
        static_cast<QGraphicsView*>(_ui->mapView),
        static_cast<QGLWidget*>(_engineGLView)
    };


    success &= gyInputInit(&inputParams);
   // assert(_engineGLView->context()->isValid());

    _engineGLView->makeCurrent();
    success &= glalVidGlewInit();

    _engineGLView->doneCurrent();

    _engineGLView->updateGL();

    _engineGLView->startRenderTimer();

    Project *proj = _assets.loadProject(QDir::currentPath());
    success &= (proj != nullptr);
   // _engineGLView->update();

    if(!proj) {
        QString invalidNotice("There's no fucking way that was a valid project directory, cockwheeze.");
        if(isVisible()) QMessageBox::critical(this, "You dun fucked up!", invalidNotice);
        else Debug::printf(Debug::DBG_ERROR, "%s", Q_CSTR(invalidNotice));
        success = false;
        return false;
    }

    _assets.setCurrentProject(proj);
    _assets.setCurrentSelection(_assets.getCurrentArea());

    //_engineGLView->doneCurrent();

  //  _engineGLView->doneCurrent();


    //elysian::Debug::getInstance()._curArea = _assets.getCurrentArea();
    //_assets.getCurrentArea()->populateVbo();

        // ENABLE ALL UI VIEWS
    if(proj) {
        toggleProjectActions(true);
        _assetDockWidget->setEnabled(true);
        _entityDockWidget->setEnabled(true);
        _layerDockWidget->ToggleDefaultItem(true);
        _layerDockWidget->setEnabled(true);
        _sheetDockWidget->CenterSheetView(0, 0);
        _sheetDockWidget->setEnabled(true);
        _selectionDockWidget->setEnabled(true);
        _historyDockWidget->setEnabled(true);
        //_ui->mapView->setEnabled(true);
    }

    return success;

    //_ui->mapView->setFocus();
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
    Debug::Log(Debug::DBG_VERBOSE, "ESToolkit::on_actionFileSaveCurrentProject_triggered");
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
    _assets.setCurrentSelection(new EntitySelectable(_assets.getCurrentArea()->newEntity()));
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

void ESToolkit::on_actionEntityAttachRenderable_triggered() {
//	((Entity *)_assets.getCurrentSelection())->AttachRenderable();
}

void ESToolkit::on_actionEntityAttachAnimation_triggered() {
//	((Entity *)_assets.getCurrentSelection())->AttachAnimation();
}

void ESToolkit::on_actionEntityAttachCollidable_triggered() {
//	((Entity *)_assets.getCurrentSelection())->AttachCollidable();
}

void ESToolkit::on_actionEntityAttachWarp_triggered() {
//	((Entity *)_assets.getCurrentSelection())->AttachWarp();
}

#if 0
void ESToolkit::on_actionEntityAttachItem_triggered() {
   ((Entity *)_assets.getCurrentSelection())->AttachItem();
}


void ESToolkit::on_actionEntityAttachFriendly_triggered() {
    ((Entity *)_assets.getCurrentSelection())->AttachFriendly();
}

void ESToolkit::on_actionEntityAttachEnemy_triggered() {
   ((Entity *)_assets.getCurrentSelection())->AttachEnemy();
}
#endif

void ESToolkit::on_actionEntityAttachCharacter_triggered() {
//	((Entity *)_assets.getCurrentSelection())->AttachCharacter();
}


void ESToolkit::on_actionEntityRemoveRenderable_triggered() {
//	((Entity *)_assets.getCurrentSelection())->RemoveRenderable();
}

void ESToolkit::on_actionEntityRemoveAnimation_triggered() {
//	((Entity *)_assets.getCurrentSelection())->RemoveAnimation();
}

void ESToolkit::on_actionEntityRemoveCollidable_triggered() {
//	((Entity *)_assets.getCurrentSelection())->RemoveCollidable();
}

void ESToolkit::on_actionEntityRemoveWarp_triggered() {
//	((Entity *)_assets.getCurrentSelection())->RemoveWarp();
}

#if 0
void ESToolkit::on_actionEntityRemoveItem_triggered() {
    ((Entity *)_assets.getCurrentSelection())->RemoveItem();
}

void ESToolkit::on_actionEntityRemoveFriendly_triggered() {
    ((Entity *)_assets.getCurrentSelection())->RemoveFriendly();
}

void ESToolkit::on_actionEntityRemoveEnemy_triggered() {
    ((Entity *)_assets.getCurrentSelection())->RemoveEnemy();
}
#endif

void ESToolkit::on_actionEntityRemoveCharacter_triggered() {
//	((Entity *)_assets.getCurrentSelection())->RemoveCharacter();
}

void ESToolkit::on_actionAreaRemove_triggered() {
    if(QMessageBox::critical(nullptr, tr("Warning!"), tr("Are you SURE that you want to permanently delete the current area?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    if(_assets.getCurrentSelection() == _assets.getCurrentArea()) _selectionDockWidget->SetSelectionModel(nullptr);

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
    if(_assets.getCurrentSelection() == _assets.getCurrentLevel()) _selectionDockWidget->SetSelectionModel(nullptr);
    _assets.setCurrentLevel(nullptr);
    _assets.getCurrentProject()->removeLevel(name);
    _assets.setCurrentLevel(_assets.getCurrentProject()->getFirstLevel());
}
/*
void ESToolkit::on_actionFileExportCurrentArea_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), _assets.getCurrentProject()->getLocation());
    dir += "/" + _assets.getCurrentArea()->getName();
    _assets.getCurrentArea()->Export(dir);
}

void ESToolkit::on_actionFileExportCurrentLevel_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), _assets.getCurrentProject()->getLocation());
    dir += "/" + _assets.getCurrentLevel()->getName();
    _assets.getCurrentLevel()->Export(dir);
}*/
/*
void ESToolkit::on_actionFileSaveAsCurrentProject_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory for Export"), _assets.getCurrentProject()->getLocation());
    dir += "/" + _assets.getCurrentProject()->getName();
    _assets.getCurrentProject()->Export(dir);
}*/
/*
void ESToolkit::on_actionFileImportLevel_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Level to Import"), _assets.getCurrentProject()->getLocation());
    _assets.getCurrentProject()->ImportLevel(dir);
}

void ESToolkit::on_actionFileImportArea_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Area to Import"), _assets.getCurrentProject()->getLocation());
    _assets.getCurrentLevel()->ImportArea(dir);
}*/

void ESToolkit::on_actionLevelReloadSheets_triggered()
{
    _assets.getCurrentLevel()->reloadSheets();
}


void ESToolkit::OnSelectionChanged(const Selectable::TYPE type, Selectable *const selection) {

    _ui->mapView->deleteSelectionItemGroup();
    /* Must be cleared after every selection change so that the model isn't pointing
      to a deleted TerrainSelection */
    _selectionDockWidget->SetSelectionModel(nullptr);
    _selectionDockWidget->restoreSelectionTreeWidgetDefaultItemDelegate();

    switch(type) {
    case Selectable::PROJECT:
        break;
    case Selectable::LEVEL:
        _selectionDockWidget->SetSelectionTreeWidget();
        static_cast<Level *>(selection)->populateModel();
        _selectionDockWidget->SetSelectionModel(static_cast<Level *>(selection));
        _selectionDockWidget->ExpandSelectionTree();
        break;
    case Selectable::AREA:
        _selectionDockWidget->SetSelectionTreeWidget();
        //static_cast<Area *>(selection)->populateModel();
        _selectionDockWidget->SetSelectionModel(selection->getModel());
        _selectionDockWidget->ExpandSelectionTree();
        break;
    case Selectable::TILE:
         _ui->mapView->createSelectionItemGroup();
         static_cast<TileSelectable *>(selection)->populateModel();
         _selectionDockWidget->SetSelectionModel(static_cast<TileSelectable *>(selection));
         /*if(static_cast<TileSelectable *>(selection)->getSize() == 1)*/ _selectionDockWidget->ExpandSelectionTree();
         selection->installSelectionViewCustomDelegates(_selectionDockWidget->getSelectionTreeWidget());
        _selectionDockWidget->SetSelectionTreeWidget();
        break;
    case Selectable::ENTITY:
        //Update Menu Bar
        //SyncEntityMenuBar(static_cast<Entity *>(selection));
#if 1
        //Update Entity View
        _selectionDockWidget->SetSelectionTreeWidget();
        //static_cast<Entity*>(selection)->populateModel();
        _selectionDockWidget->SetSelectionModel(static_cast<EntitySelectable*>(selection));
        _selectionDockWidget->ExpandSelectionTree();
#else
        select(static_cast<EntitySelectable*>(selection)->)
#endif

        //center on entity
        //_ui->mapView->centerOn((Entity *)selection);
        //_ui->mapView->SetCenter((Entity *)selection);
        break;
        case Selectable::EMPTY:


        /*if(static_cast<TileSelectable *>(selection)->getSize() == 1)*/ //_selectionDockWidget->ExpandSelectionTree();
      //  selection->installSelectionViewCustomDelegates(_selectionDockWidget->getSelectionTreeWidget());
       //_selectionDockWidget->SetSelectionTreeWidget();

        //_selectionDockWidget->SetSelectionWidget(NULL);
        //crap pants?
         break;
    };
}

void ESToolkit::on_actionUndo_triggered(){
    if(!_assets.getCurrentArea()) return;
    if(_assets.getCurrentArea()->getUndoStackCount() > 0) {
        _assets.getCurrentArea()->getUndoStack().undo();
    }
}

void ESToolkit::on_actionRedo_triggered() {
    if(!_assets.getCurrentArea())  {
        Debug::Log(Debug::DBG_WARNING, "Action Redo triggered, but there was no Area! Aborting function call...");
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
    //_assets.getEngine()->getLevelSys()->addArea(newArea);
    _assets.setCurrentArea(newArea);
    _assets.setCurrentSelection(newArea);
}

void ESToolkit::OnProjectRemoved() {
    toggleProjectActions(false);
    _assetDockWidget->clearTreeView();
    _assetDockWidget->setEnabled(false);
    _entityDockWidget->setEnabled(false);
    _layerDockWidget->ToggleDefaultItem(false);
    _layerDockWidget->setEnabled(false);
    _sheetDockWidget->setEnabled(false);
    _historyDockWidget->setEnabled(false);
    _selectionDockWidget->setWidget(nullptr);
    _selectionDockWidget->setEnabled(false);
    _ui->mapView->deleteSelectionItemGroup();
    _ui->mapView->setEnabled(false);
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
                //Debug::Log(Debug::DBG_WARNING, "InvokeEngine(DREAMCAST) failed! Invalid Settings.");
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

void ESToolkit::onInvokeEngineStopped(void) {
    const bool blocked = _ui->actionInvoke->blockSignals(true);
    _ui->actionInvoke->setChecked(false);
    _ui->actionInvoke->blockSignals(blocked);
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

void ESToolkit::UpdateDockWidgetChecked(int widget) {
    switch(widget) {
        case 0:
            _ui->actionAsset_View->setChecked(false);
            break;
        case 1:
            _ui->actionEntity_View->setChecked(false);
            break;
        case 2:
            _ui->actionLayer_View->setChecked(false);
            break;
        case 3:
            _ui->actionSheet_View->setChecked(false);
            break;
        case 4:
            _ui->actionHistory_View->setChecked(false);
            break;
        case 5:
            _ui->actionSelection_View->setChecked(false);
            break;
        default:
            break;
    }
}

void ESToolkit::GiveMapViewFocus()
{
    QWidget::activateWindow();
    _ui->mapView->setFocus();
}

// #JARROD - 03/29/12
// Added this function to remove issue of panning/painting at the same time on start-up
void ESToolkit::ExitPanMode()
{
   _ui->mapView->setDragMode(QGraphicsView::NoDrag);
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
   // static bool inited = false;
   // static LuaTableStackModel* lmodel;
#if 0
    if(!inited) {
        lua_pushglobaltable(*_mainState);
        lua_getfield(*_mainState, -1, "tablet");
        const int ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
        lua_pop(*_mainState, 1);

        //ref = new OOLUA::Lua_table_ref;
        //ref->pull(*_mainState);
        //lua_pop(*_mainState, 1);

        //LuaTable ltable = LuaTable::fromLuaExpression(luaExpr);
        elysian::LuaManager::rPrintByRef(ref, 99, "");
        lmodel = new LuaTableStackModel(ref);
        lmodel->populateModel();
        inited = true;
    }

    //_luaExplorerDialog->setLuaTableStackModel(_assets.getCurArea());
    _luaExplorerDialog->setModel(lmodel);
    _luaExplorerDialog->show();

    RowsJoinerProxy* proxy = new RowsJoinerProxy(nullptr);
    proxy->insertSourceModel(lmodel);
    proxy->insertSourceModel(_assets.getCurrentArea());

    _selectionDockWidget->SetSelectionModel(proxy);

    lua_pushglobaltable(*_mainState);
    lua_getfield(*_mainState, -1, "serialize");helpengine
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, lmodel->_luaRef);
    lua_pushstring(*_mainState, "");
    lua_pushboolean(*_mainState, 0);
    LuaManager::_protectedCall(3, 1, nullptr);
    elysian::Debug::printf(elysian::Debug::DBG_VERBOSE, lua_tostring(*_mainState, -1));
    lua_pop(*_mainState, 1);

#else



#endif


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


    if(_assets.getEngine()) {
        int ref = LUA_NOREF;
        LuaManager::getGlobal("testInjectTable");
        //rPrintByIndex(-1, 99, "RAWREF");

        ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);

        rPrintByRef(ref, 99, "STARTREF");


        LuaTableStackModel* model = new LuaTableStackModel(ref);
        model->populateModel();
        _selectionDockWidget->SetSelectionTreeWidget();
        _selectionDockWidget->SetSelectionModel(model);
    }
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

void ESToolkit::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_1:
        _assets.setLayerVisible(Assets::LAYER_TILE1, !_assets.getLayerVisible(Assets::LAYER_TILE1));
        _layerDockWidget->setTopLevelItemCheckState(3, _assets.getLayerVisible(Assets::LAYER_TILE1));
        break;
    case Qt::Key_2:
        _assets.setLayerVisible(Assets::LAYER_TILE2, !_assets.getLayerVisible(Assets::LAYER_TILE2));
        _layerDockWidget->setTopLevelItemCheckState(2, _assets.getLayerVisible(Assets::LAYER_TILE2));
        break;
    case Qt::Key_3:
        _assets.setLayerVisible(Assets::LAYER_TILE3, !_assets.getLayerVisible(Assets::LAYER_TILE3));
        _layerDockWidget->setTopLevelItemCheckState(1, _assets.getLayerVisible(Assets::LAYER_TILE3));
        break;
    case Qt::Key_Space:
        on_actionSheet_View_triggered(!_sheetDockWidget->isVisible());
        break;
    }
    Area::updateScene();
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
    QSettings settings;
    QString savedSettings = settings.value("settings", "").toString();
    QString projectDir = settings.value("previousProject", "").toString();
    openProjectDir(projectDir);
}

void ESToolkit::on_actionInvoke_triggered(bool checked)
{
    if(checked && (_ui->actionWindows->isChecked() || _ui->actionMac_OS_X->isChecked() || _ui->actionNix->isChecked())) {
        _engineGLView->play();
    } else if(!checked) {
        _engineGLView->stop();
    }
}

void ESToolkit::onEntityTreeWidgetLayerChange(void) {
    OnActiveLayerChanged(1); //force entity layer
    _layerDockWidget->setCurrentLayer(Assets::LAYER_ENTITY);
}


void ESToolkit::on_actionAdvance_Frame_triggered()
{
#if 0 //random debug reest. :D
    static bool once = false;
    static GYVector4 color[4] = {
        { 10.0f, 0.0f, 0.0f, 1.5f },
        { 0.0f, 0.0f, 10.0f, 1.5f },
        { 0.0f, 10.0f, 0.0f, 1.5f },
        { 0.0f, 10.0f, 10.0f, 1.5f }

    };

    if(!once) {
        Entity* entity;

        entity = _assets.getCurArea()->findEntity("Julien");
        qDebug() << "JULIEN COLLIDER" << entity->getCollidable();

        for(unsigned i = 0; i < 1; ++i) {
            //tity = new Entity;
            //entity->getTransform().setPos(900.0f*i, 900.0f*i, 100.0f);
            auto* src = entity->addLightSource();
            src->setLightType(GY_LIGHT_POINT);
            src->setShadowCaster(true);
            auto* attrib = new elysian::LightAttrib;
            attrib->setDiffuseColor(color[i]);
            attrib->setSpecularColor(color[i]);
            //attrib->setAmbientColor(color[i]);
            src->setAttrib(attrib);

            _assets.getCurArea()->addEntity(entity);
        }

        once = true;
    }
#endif

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

void ESToolkit::_setMapViewResolution(const MAP_VIEW_RES res) {
    static const unsigned sizes[MAP_VIEW_RES_MAX][2][2] = {
        { { 640,  480  }, { 640,  480 } },
        { { 1280, 720  }, { 1280, 720 } },
        { { 9999, 9999 }, { 0,    0  }  }
    };

    _mapViewRes = res;
    _ui->mapView->setMaximumSize(sizes[res][0][0], sizes[res][0][1]);
    _ui->mapView->setMinimumSize(sizes[res][1][0], sizes[res][1][1]);
    _ui->mapView->adjustSize();

    _ui->action480p->blockSignals(true);
    _ui->action720p->blockSignals(true);
    _ui->actionDynamic->blockSignals(true);

    _ui->action480p->setEnabled(true);
    _ui->action720p->setEnabled(true);
    _ui->actionDynamic->setEnabled(true);

    _ui->action480p->setChecked(false);
    _ui->action720p->setChecked(false);
    _ui->actionDynamic->setChecked(false);

    switch(res) {
    case MAP_VIEW_RES_480P:
        _ui->action480p->setChecked(true);
        _ui->action480p->setEnabled(false);
        break;
    case MAP_VIEW_RES_720P:
        _ui->action720p->setChecked(true);
        _ui->action720p->setEnabled(false);
        break;
    case MAP_VIEW_RES_DYNAMIC:
    default:
        _ui->actionDynamic->setChecked(true);
        _ui->actionDynamic->setEnabled(false);
        break;
    }

    _ui->action480p->blockSignals(false);
    _ui->action720p->blockSignals(false);
    _ui->actionDynamic->blockSignals(false);

}

void ESToolkit::on_actionReload_Shaders_triggered(void) {
    gyShadersReloadAll();
}

void ESToolkit::setMainTextureUniform(const int uniform) {
    gyVidDbgTexMapUniform(uniform);

    _ui->actionTilesheet->blockSignals(true);
    _ui->actionNormalsheet->blockSignals(true);
    _ui->actionSpecularsheet->blockSignals(true);
    _ui->actionTilesheet->setEnabled(true);
    _ui->actionNormalsheet->setEnabled(true);
    _ui->actionSpecularsheet->setEnabled(true);
    _ui->actionTilesheet->setChecked(false);
    _ui->actionNormalsheet->setChecked(false);
    _ui->actionSpecularsheet->setChecked(false);


    switch(uniform) {
    case 0:
        _ui->actionTilesheet->setEnabled(false);
        _ui->actionTilesheet->setChecked(true);
        break;
    case 1:
        _ui->actionNormalsheet->setEnabled(false);
        _ui->actionNormalsheet->setChecked(true);
        break;
    case 2:
        _ui->actionSpecularsheet->setEnabled(false);
        _ui->actionSpecularsheet->setChecked(true);
        break;

    }


    _ui->actionTilesheet->blockSignals(false);
    _ui->actionNormalsheet->blockSignals(false);
    _ui->actionSpecularsheet->blockSignals(false);
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

void ESToolkit::_onEngineInitialized(void) {
    _assets.getEngine()->getMapSys()->setView(_ui->mapView);
    _tkLuaInitialize();
    _assets.getEngine()->getLightSys()->setShadowPassEnabled(_engineParams.shadowPassEnabled);
    _assets.getEngine()->getLightSys()->setEnabled(_engineParams.lightingEnabled);
    _engineGLView->setCameraManualOnPlay(_engineParams.manualCameraEnabled);
    _assets.getEngine()->getAudioSys()->setEnabled(_engineParams.audioEnabled);

    //Override
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

void ESToolkit::_tkLuaInitialize(void) {


}

void ESToolkit::select(const int luaRef) {
    auto* self = _instance;

    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, luaRef);
    int newRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);

    LuaTableStackModel* model = new LuaTableStackModel(newRef);
    model->populateModel();
    self->_selectionDockWidget->SetSelectionTreeWidget();
    self->_selectionDockWidget->SetSelectionModel(model);
    self->_selectionDockWidget->ExpandSelectionTree();
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

bool ESToolkit::verifySingleArea(const QString verifyPath) {
    QFileInfo   pathInfo(verifyPath);
    QString     areaPath = pathInfo.absoluteFilePath();
    QString     levelPath;
    QString     projectPath;

    qDebug() << "VERIFYING" << verifyPath;
    qDebug() << "ABSOLUTE" << areaPath;

    if(!pathInfo.exists() || !pathInfo.isDir()) {
        qDebug() << "PATH DOES NOT EXIST";
        return false;
    }

    QDir curDir(areaPath);
    do {
        if(!curDir.cdUp()) {
            qDebug() << "Could not go up one level.";
        }

    } while(!QDir(curDir.filePath("area")).exists());

    levelPath = curDir.absolutePath();
    qDebug() << "Level path:" << levelPath;

    do {
        if(!curDir.cdUp()) {
            qDebug() << "Could not go up one level.";
        }

    } while(!QDir(curDir.filePath("level")).exists());

    projectPath = curDir.absolutePath();
    qDebug() << "Project path: " << projectPath;

    openProjectDir(projectPath);
    QDir::setCurrent(projectPath);
    _assets.setCurrentLevel(levelPath.split("/").last());
    _assets.setCurrentArea(areaPath.split("/").last());

    QApplication::quit();
    return true;
}



void ESToolkit::on_actionClear_triggered()
{
    Debug::clear();
}

void ESToolkit::on_actionLibGyro_toggled(bool arg1)
{
    _ui->actionLibGyroVerbose->setChecked(arg1);
    _ui->actionLibGyroWarning->setChecked(arg1);
    _ui->actionLibGyroError->setChecked(arg1);
}

void ESToolkit::on_actionEngine_toggled(bool arg1)
{
    _ui->actionEngineVerbose->setChecked(arg1);
    _ui->actionEngineWarning->setChecked(arg1);
    _ui->actionEngineError->setChecked(arg1);
}

void ESToolkit::on_actionToolkit_toggled(bool arg1)
{
    _ui->actionToolkitVerbose->setChecked(arg1);
    _ui->actionToolkitWarning->setChecked(arg1);
    _ui->actionToolkitError->setChecked(arg1);}

void ESToolkit::on_actionLua_toggled(bool arg1)
{
    _ui->actionLuaVerbose->setChecked(arg1);
    _ui->actionLuaWarning->setChecked(arg1);
    _ui->actionLuaError->setChecked(arg1);
}


void ESToolkit::on_actionQt_toggled(bool arg1)
{
    _ui->actionQtVerbose->setChecked(arg1);
    _ui->actionQtWarning->setChecked(arg1);
    _ui->actionQtError->setChecked(arg1);
}

void ESToolkit::on_actionCommands_toggled(bool arg1)
{
    _ui->actionCommandsVerbose->setChecked(arg1);
    _ui->actionCommandsWarning->setChecked(arg1);
    _ui->actionCommandsError->setChecked(arg1);
}

void ESToolkit::on_actionOpenGL_toggled(bool arg1)
{
    _ui->actionOpenGLVerbose->setChecked(arg1);
    _ui->actionOpenGLWarning->setChecked(arg1);
    _ui->actionOpenGLError->setChecked(arg1);
}

void ESToolkit::on_actionLibGyroVerbose_toggled(bool arg1)
{
    Debug::setFilter(Debug::LIBGYRO, Debug::DBG_VERBOSE, arg1);

}

void ESToolkit::on_actionLibGyroWarning_toggled(bool arg1)
{
    Debug::setFilter(Debug::LIBGYRO, Debug::DBG_WARNING, arg1);
}

void ESToolkit::on_actionLibGyroError_toggled(bool arg1)
{
    Debug::setFilter(Debug::LIBGYRO, Debug::DBG_ERROR, arg1);
}

void ESToolkit::on_actionEngineVerbose_toggled(bool arg1)
{
    Debug::setFilter(Debug::ENGINE, Debug::DBG_VERBOSE, arg1);
}

void ESToolkit::on_actionEngineWarning_toggled(bool arg1)
{
    Debug::setFilter(Debug::ENGINE, Debug::DBG_WARNING, arg1);
}

void ESToolkit::on_actionEngineError_toggled(bool arg1)
{
    Debug::setFilter(Debug::ENGINE, Debug::DBG_ERROR, arg1);
}

void ESToolkit::on_actionToolkitVerbose_toggled(bool arg1)
{
    Debug::setFilter(Debug::TOOLKIT, Debug::DBG_VERBOSE, arg1);
}

void ESToolkit::on_actionToolkitWarning_toggled(bool arg1)
{
    Debug::setFilter(Debug::TOOLKIT, Debug::DBG_WARNING, arg1);
}

void ESToolkit::on_actionToolkitError_toggled(bool arg1)
{
    Debug::setFilter(Debug::TOOLKIT, Debug::DBG_ERROR, arg1);
}

void ESToolkit::on_actionQtVerbose_toggled(bool arg1)
{
    Debug::setFilter(Debug::SOURCE_QT, Debug::DBG_VERBOSE, arg1);
}

void ESToolkit::on_actionQtWarning_toggled(bool arg1)
{
    Debug::setFilter(Debug::SOURCE_QT, Debug::DBG_WARNING, arg1);
}

void ESToolkit::on_actionQtError_toggled(bool arg1)
{
    Debug::setFilter(Debug::SOURCE_QT, Debug::DBG_ERROR, arg1);
}

void ESToolkit::on_actionOpenGLVerbose_toggled(bool arg1)
{
    Debug::setFilter(Debug::OPENGL, Debug::DBG_VERBOSE, arg1);
}

void ESToolkit::on_actionOpenGLWarning_toggled(bool arg1)
{
    Debug::setFilter(Debug::OPENGL, Debug::DBG_WARNING, arg1);
}

void ESToolkit::on_actionOpenGLError_toggled(bool arg1)
{
    Debug::setFilter(Debug::OPENGL, Debug::DBG_ERROR, arg1);
}

void ESToolkit::on_actionLuaVerbose_toggled(bool arg1)
{
    Debug::setFilter(Debug::LUA, Debug::DBG_VERBOSE, arg1);
}

void ESToolkit::on_actionLuaWarning_toggled(bool arg1)
{
    Debug::setFilter(Debug::LUA, Debug::DBG_WARNING, arg1);
}

void ESToolkit::on_actionLuaError_toggled(bool arg1)
{
    Debug::setFilter(Debug::LUA, Debug::DBG_ERROR, arg1);
}

void ESToolkit::on_actionCommandsVerbose_toggled(bool arg1)
{
    Debug::setFilter(Debug::COMMAND, Debug::DBG_VERBOSE, arg1);
}

void ESToolkit::on_actionCommandsWarning_toggled(bool arg1)
{
    Debug::setFilter(Debug::COMMAND, Debug::DBG_WARNING, arg1);
}

void ESToolkit::on_actionCommandsError_toggled(bool arg1)
{
    Debug::setFilter(Debug::COMMAND, Debug::DBG_ERROR, arg1);
}

void ESToolkit::on_actionReset_Stack_Depth_triggered()
{
    Debug::resetStackDepth();
}

void ESToolkit::showGitlabBugReportTab(void) const {
    _gitlabDialog->setTab(GitlabDialog::TAB_CREATE_REPORT);
    _gitlabDialog->show();
}

void ESToolkit::on_actionHidden_Levels_toggled(bool arg1)
{
    _engineParams.showHiddenLevels = arg1;
    if(_assets.getCurrentProject()) {
        _assets.getCurrentProject()->setIndexHiddenLevels(arg1);
    }
}

void ESToolkit::setIndexHiddenLevels(const bool val) {
    on_actionHidden_Levels_toggled(val);
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
