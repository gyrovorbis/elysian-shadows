#include <ESToolkit/ui//tk_estoolkit.h>
#include "ui_tk_estoolkit.h"
#include "ui_tk_sheet_dock_widget.h"

#include <QSettings>
#include <QStatusBar>
#include <QMessageBox>
#include <QToolButton>

#include <QESTkUtils/tk_gl_utilities.h>
#include <ESToolkit/ui/dock/asset/tk_asset_dock_widget.h>
#include <ESToolkit/ui/dock//tk_entity_dock_widget.h>
#include <ESToolkit/ui/dock/tk_layer_dock_widget.h>
#include <ESToolkit/ui/dock/tk_sheet_dock_widget.h>
#include <ESToolkit/sheet_manager/tk_sheet_manager.h>
#include <ESToolkit/ui/dialog/tk_engine_dialog.h>
#include <ESToolkit/ui/dialog/tk_engine_settings_dialog.h>
#include <QESTkUtils/ui/widget/tk_gitlab_dialog.h>
#include <QESTkUtils/ui/widget/tk_documentation_browser.h>
#include <ESToolkit/ui/dock/tk_history_dock_widget.h>
#include <ESToolkit/ui/dock/tk_camera_dock_widget.h>
#include <ESToolkit/ui/dock/tk_selection_dock_widget.h>
#include <AssetIO/tk_level.h>
#include <ESToolkit/core/tk_project.h>
#include <AssetIO/tk_area.h>
#include <ESToolkit/ui/widget/tk_entity_tree_widget.h>
#include <system/elysian_engine.hpp>
#include <ESToolkit/ui/widget/tk_selection_scene.h>
#include <ESGamma/tk_map_system.h>
#include <ESToolkit/ui/widget/tk_engine_gl_scene_widget.h>
#include <ESGamma/tk_input_system.h>
#include <ESToolkit/core/tk_entity_selectable.h>
#include <ESToolkit/core/tk_lua_table_selectable.h>
#include <system/elysian_audio_system.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_area_proxy.hpp>
#include <proxy/elysian_level_proxy.hpp>
#include <gyro_vmu_main_window.hpp>
#include <ui_gyro_vmu_main_window.h>


#ifdef ESTK_UNIT_TESTS
#include <QtTest>
#endif

ESToolkit* ESToolkit::_instance = nullptr;

ESToolkit::ESToolkit(QWidget *parent):
    QMainWindow(parent),
    _ui(new Ui::ESToolkit)
{
    _instance = this;
    gyDebugInit();
    Debug::getInstance();
    AssetManager::_assets = &_assets;

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

    // another QMainWindow you want to embed
#if 0
        _evmuWindow = new VMUMainWindow(this);	// no parent
        //ubMainWindow->setWindowFlags(subMainWindow->windowFlags() & ~Qt::Window);
        //evmuWindow->setWindowFlags(Qt::Widget);
       _evmuWindow->setFocusPolicy(Qt::ClickFocus);

        //subMainWindow->setUnifiedTitleAndToolBarOnMac(true); _vmuDockWidget = new QDockWidget(nullptr);
       _vmuDockWidget = new QDockWidget(this);
        _vmuDockWidget->setWindowTitle("VMU View");
        _evmuWindow->menuBar()->setNativeMenuBar(false);


       _vmuDockWidget->setWidget(_evmuWindow);
//dock->setMouseTracking(false);
       //evmuWindow->setParent(dock) ;	// here

        addDockWidget(Qt::RightDockWidgetArea, _vmuDockWidget);
#endif

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
    connect(_selectionDockWidget, SIGNAL(uncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
    connect(_engineDialog, SIGNAL(stopped()), this, SLOT(onInvokeEngineStopped()));


    auto* altTabAction = new QAction(this);
    /* Since CMD+Tab is already used for swapping windows on OSX AND
     * ALT+Tab does the same on Windows, this shortcut has to be treated
     * differently between Win/Mac. Linux should work like Window.
     */
#ifdef Q_OS_MAC
    altTabAction->setShortcut(QKeySequence("Alt+Tab"));
#else
    altTabAction->setShortcut(QKeySequence("Ctrl+Tab"));
#endif
    connect(altTabAction, SIGNAL(triggered()), this, SLOT(onAltTabShortcutTriggered()));
    addAction(altTabAction);

    //glUtilities::printGLSupport();
    ReadSettings();

    setMouseTracking(true);
}
#if 0
bool ESToolkit::event(QEvent *event) {
    switch(event->type()) {
    case QEvent::Shortcut: {
        auto* shortcutEvent = static_cast<QShortcutEvent*>(event);
        if(shortcutEvent->isAmbiguous()) {
            if(_evmuWindow && _evmuWindow->hasFocus()) {
                shortcutEvent->accept();
                return true;
            }

        }
        break;
    }


    default: break;
    }
    return QMainWindow::event(event);
}
#endif


ESToolkit::~ESToolkit(void) {
    _ui->mapView->setEnabled(false);
    _assets.setCurrentArea(nullptr);
    _assets.setCurrentProject(nullptr);
    _assets.setCurrentLevel(nullptr);
    _areaTabBar->disconnect();
    _assetDockWidget->disconnect();
    _assets.disconnect();
    delete _assets.getEngine();

    delete _helpDockWidget;
    delete _gitlabDialog;
    delete _sheetManager;
    delete _engineDialog;
    delete _engineSettingsDialog;
    delete _ui;

    _libGyroUninitialize();
}

void ESToolkit::_libGyroInitialize(void) {
    /* The File subsystem needs to be initialized before anything else, ideally */
    _debug = &elysian::Debug::getInstance();
    char verString[50];

    _debug->parseArguments();
    strcpy(verString, "ESGamma v");
    strcat(verString, GYRO_LIB_VERSION);

    GYSysInitParams sysParams = {new std::function<void()>([&](void) {
                        _gitlabDialog->show();
                  }),
                    [](void* /*ptr*/) {
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
                }

            };
    gySysInit(&sysParams);
    //gyVidInit(&vidParams);
    gyAudInit();
    //gyTimerInit();
    _libGyroInitialized = true;

}

void ESToolkit::_libGyroUninitialize(void) {
    if(_libGyroInitialized) {
        gyAudUninit();
        _libGyroInitialized = false;
    }
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
#ifdef ESTOOLKIT_SETTINGS_FILE
    QSettings settings(_outputDir.absoluteFilePath(ESTOOLKIT_SETTINGS_FILE), QSettings::IniFormat);
#else
    QSettings settings;
#endif
    DbgEvent event("Saving user settings [%s].", Q_CSTR(settings.fileName()));

    settings.setValue("settings", "Did Set");
    settings.setValue("geometry", saveGeometry());
    setWindowState(Qt::WindowMaximized);
    settings.setValue("state", saveState());
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
    settings.setValue("bloomEnabled", _engineParams.bloomEnabled);
    settings.setValue("manualCameraEnabled", _engineParams.manualCameraEnabled);
    bool audioEnabled = _engineParams.audioEnabled;
    settings.setValue("audioEnabled", audioEnabled);
    unsigned x, y;
    _engineDialog->getStartPos(x, y);
    settings.setValue("engineStartPosX", x);
    settings.setValue("engineStartPosY", y);
    settings.setValue("previousProject", _assets.getProjectDir());
    if(!_assets.getProjectDir().isEmpty()) {
        settings.setValue("previousProject", _assets.getProjectDir());
        if(_assets.getCurrentLevel()) {
            settings.setValue("previousLevel", _assets.getCurrentLevel()->getPath());
            if(_assets.getCurrentArea()) {
                settings.setValue("previousArea", _assets.getCurrentArea()->getPath());
            } else {
                settings.setValue("previousArea", _prevAreaPath);
            }
        } else {
            settings.setValue("previousLevel", _prevLevelPath);
        }
    } else {
        settings.setValue("previousProject", _prevProjectPath);
    }
    if(_gitlabDialog->isAuthenticated()) {
        settings.setValue("gitLabHostName", _gitlabDialog->getHostName());
        settings.setValue("gitLabPrivateToken", _gitlabDialog->getPrivateToken());
        settings.setValue("gitLabProjectId", _gitlabDialog->getProjectId());
    }

    for(unsigned i = 0; i < TkDebug::SOURCE_MAX; ++i) {
        for(unsigned j = 0; j < TkDebug::LOG_OFF; ++j) {
            char buff[30];
            sprintf(buff, "debugFilter_%d_%d", i, j);
            settings.setValue(buff, Debug::isFilterEnabled((TkDebug::TK_SOURCE)i, (TkDebug::SEVERITY)j));
        }
    }

    settings.setValue("showHiddenLevels", _assets.getCurrentProject()? _assets.getCurrentProject()->getIndexHiddenLevels() : false);

}

void ESToolkit::ReadSettings()
{
#ifdef ESTOOLKIT_SETTINGS_FILE
    QSettings settings(_outputDir.absoluteFilePath(ESTOOLKIT_SETTINGS_FILE), QSettings::IniFormat);
#else
    QSettings settings;
#endif

    DbgEvent event("Loading user settings [%s].", Q_CSTR(settings.fileName()));
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


    QString savedSettings       = settings.value("settings", "").toString();
    QByteArray geo              = settings.value("geometry", QByteArray()).toByteArray();
    QByteArray state            = settings.value("state", QByteArray()).toByteArray();
    bool sheetViewHorizontal    = settings.value("sheetViewHorizontal", true).toBool();
    bool glEnabled              = settings.value("glEnabled", true).toBool();
    bool autoScrollEnabled      = settings.value("autoScrollEnabled", true).toBool();
    _engineDialog->setResolution(settings.value("engineResolution", 0).toUInt());
    bool renderMapBg            = settings.value("renderMapBackground", false).toBool();
    unsigned x                  = settings.value("engineStartPosX", 0).toUInt();
    unsigned y                  = settings.value("engineStartPosY", 0).toUInt();
    bool showHiddenEntities     = settings.value("showHiddenEntities", false).toBool();
    bool shadowPassEnabled      = settings.value("shadowPassEnabled", true).toBool();
    bool bloomEnabled           = settings.value("bloomEnabled", true).toBool();
    bool manualCamera           = settings.value("manualCameraEnabled", false).toBool();
    bool audioEnabled           = settings.value("audioEnabled", true).toBool();
    bool lightingEnabled        = settings.value("lightingEnabled", true).toBool();
    bool showHiddenLevels       = settings.value("showHiddenLevels", false).toBool();
    _prevProjectPath            = settings.value("previousProject", "").toString();
    _prevLevelPath              = settings.value("previousLevel", "").toString();
    _prevAreaPath               = settings.value("previousArea", "").toString();

    _engineDialog->setStartPos(x, y);
    MAP_VIEW_RES mapViewRes     = (MAP_VIEW_RES)settings.value("mapViewRes", MAP_VIEW_RES_DYNAMIC).toInt();
    _setMapViewResolution(mapViewRes);

    for(unsigned source = 0; source < Debug::SOURCE_MAX; ++source) {
        for(unsigned severity = 0; severity < Debug::LOG_OFF; ++severity) {
            char buff[30];
            sprintf(buff, "debugFilter_%d_%d", source, severity);
            const bool enabled = settings.value(buff, true).toBool();
            Debug::setFilter((Debug::TK_SOURCE)source, (Debug::SEVERITY)severity, enabled);
        }
    }

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
    _ui->actionBloom->setChecked(bloomEnabled);
    _ui->actionManual_Camera->setChecked(manualCamera);
    _ui->actionEnable_Audio->setChecked(audioEnabled);
    _ui->actionHidden_Levels->setChecked(showHiddenLevels);
    if(!settings.value("gitLabPrivateToken").isNull()) {
        _gitlabDialog->setHostName(settings.value("gitLabHostName").toString());
        _gitlabDialog->setPrivateToken(settings.value("gitLabPrivateToken").toString());
        //_gitlabDialog->authenticate(true);
        _gitlabDialog->setProjectId(settings.value("gitLabProjectId").toInt());
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
  //  static int i = 0;

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
    if(_assets.getCurrentLayer() == Assets::LAYER_ENTITY && _assets.getCurrentLayer() != newLayer) _assets.setCurrentSelection(nullptr);

    _assets.setCurrentLayer(newLayer);

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

        connect(viewport, SIGNAL(engineUpdated()), _ui->mapView, SLOT(onEngineUpdated()));
        viewport->setMouseTracking(true);
        _engineGLView->setMapView(_ui->mapView);
        _engineGLView->makeCurrent();
        _engineGLView->update();
        _engineGLView->show();
        _engineGLView->updateGL();
        _ui->mapView->setViewport(viewport);


    GYInputInitParams inputParams = {
        static_cast<QObject*>(_ui->mapView),
        static_cast<QGLWidget*>(_engineGLView),
        4
    };

    success &= elysian::InputSystem::initialize(&inputParams);

    _engineGLView->makeCurrent();
    success &= glalVidGlewInit();

    _engineGLView->updateGL();

    _engineGLView->startRenderTimer();

    Project *proj = _assets.loadProject(QDir::currentPath());
    success &= (proj != nullptr);

    if(!proj) {
        QString invalidNotice("There's no fucking way that was a valid project directory, cockwheeze.");
        if(isVisible()) QMessageBox::critical(this, "You dun fucked up!", invalidNotice);
        else Debug::printf(Debug::LOG_ERROR, "%s", Q_CSTR(invalidNotice));
        success = false;
        return false;
    }

    _assets.setCurrentProject(proj);
    _assets.setCurrentSelection(_assets.getCurrentArea());

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


void ESToolkit::OnSelectionChanged(const Selectable::TYPE type, Selectable *const selection) {



    _ui->mapView->deleteSelectionItemGroup();
    /* Must be cleared after every selection change so that the model isn't pointing
      to a deleted TerrainSelection */
    _selectionDockWidget->setSelectionModel(nullptr);
    _selectionDockWidget->restoreSelectionTreeViewDefaultItemDelegate();

    switch(type) {
    case Selectable::PROJECT:
        break;
    case Selectable::LEVEL:
        _selectionDockWidget->setSelectionTreeView();
        static_cast<Level *>(selection)->populateModel();
        _selectionDockWidget->setSelectionModel(static_cast<Level *>(selection));
        _selectionDockWidget->expandSelectionTree();
        break;
    case Selectable::AREA:
        _selectionDockWidget->setSelectionTreeView();

        _selectionDockWidget->setSelectionModel(selection->getModel());
        _selectionDockWidget->expandSelectionTree();
        break;
    case Selectable::TILE:
        if(_assets.getCurrentLayer() != Assets::LAYER_TILE1 &&
                _assets.getCurrentLayer() != Assets::LAYER_TILE2 &&
                _assets.getCurrentLayer() != Assets::LAYER_TILE3) {
            _assets.setCurrentLayer(Assets::LAYER_TILE1);
        }
         _ui->mapView->createSelectionItemGroup();
         static_cast<TileSelectable *>(selection)->populateModel();
         _selectionDockWidget->setSelectionModel(static_cast<TileSelectable *>(selection));
         _selectionDockWidget->expandSelectionTree();
         selection->installSelectionViewCustomDelegates(_selectionDockWidget->getSelectionTreeView());
        _selectionDockWidget->setSelectionTreeView();
        break;
    case Selectable::ENTITY:
        _assets.setCurrentLayer(Assets::LAYER_ENTITY);
        //Update Entity View
        _selectionDockWidget->setSelectionTreeView();
        _selectionDockWidget->setSelectionModel(static_cast<EntitySelectable*>(selection)->getModel());
        _selectionDockWidget->restoreSelectionTreeViewDefaultItemDelegate();
        _selectionDockWidget->expandSelectionTree(2);
        break;

    case Selectable::LUA_TABLE:

        _selectionDockWidget->setSelectionTreeView();
        _selectionDockWidget->setSelectionModel(selection->getModel());
        _selectionDockWidget->expandSelectionTree();

        break;
    case Selectable::EMPTY:

         break;
    };
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

void ESToolkit::onInvokeEngineStopped(void) {
    const bool blocked = _ui->actionInvoke->blockSignals(true);
    _ui->actionInvoke->setChecked(false);
    _ui->actionInvoke->blockSignals(blocked);
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

void ESToolkit::onEntityTreeWidgetLayerChange(void) {
    OnActiveLayerChanged(1); //force entity layer
    _layerDockWidget->setCurrentLayer(Assets::LAYER_ENTITY);
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



void ESToolkit::_onEngineInitialized(void) {
    _assets.getEngine()->getMapSys()->setView(_ui->mapView);
    _assets.getEngine()->getLightSys()->setShadowPassEnabled(_engineParams.shadowPassEnabled);
    _assets.getEngine()->getLightSys()->setEnabled(_engineParams.lightingEnabled);
    _engineGLView->setCameraManualOnPlay(_engineParams.manualCameraEnabled);
    _assets.getEngine()->getAudioSys()->setEnabled(_engineParams.audioEnabled);
    _assets.getEngine()->getLightSys()->setBloomEnabled(_engineParams.bloomEnabled);

    //Override
}

void ESToolkit::select(const LuaVariant& ref) {
    auto* self = _instance;
    Selectable* selection;

    switch(ref.getType()) { //Check whether it's any of the supported Selectable types!
    case LUA_TUSERDATA:
        if(strcmp(ref.getTypeString(), "Entity") == 0) {
            selection = new EntitySelectable(ref.getValue<Entity*>());
            break;
        } else if(strcmp(ref.getTypeString(), "Area") == 0) {
            selection = ref.getValue<Area*>();
            break;
        } else if(strcmp(ref.getTypeString(), "Level") == 0) { //Level isn't really implemented via proxies yet...
            selection = ref.getValue<Level*>();
            break;
        }
    case LUA_TTABLE:
    default:
        selection = new LuaTableSelectable(ref);
    }

    self->_assets.setCurrentSelection(selection);

}

int ESToolkit::pushSelection(lua_State * const vm) {
    Selectable* selection = _instance->_assets.getCurrentSelection();

    switch(_instance->_assets.getCurrentSelectionType()) {
    case Selectable::ENTITY:
        static_cast<EntitySelectable*>(selection)->getTableRef().push();
        return 1;
    //case Selectable::AREA:
        //LuaVariant::fromRef(static_cast<Area*>(selection)->getLuaTableRef()).push();
        //return 1;
   // case Selectable::LEVEL:
   //     OOLUA::push(vm, static_cast<Level*>(selection)->ge);
      //  return 1;
    case Selectable::LUA_TABLE:
        static_cast<LuaTableSelectable*>(selection)->getTableRef().push();
        return 1;
    default:
        lua_pushnil(vm);
        return 0;
    }
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


void ESToolkit::showGitlabBugReportTab(void) const {
    _gitlabDialog->setTab(GitlabDialog::TAB_CREATE_REPORT);
    _gitlabDialog->show();
}

void ESToolkit::setIndexHiddenLevels(const bool val) {
    on_actionHidden_Levels_toggled(val);
}

void ESToolkit::on_actionToggle_Fullscreen_toggled(bool arg1)
{
    if(arg1) {
        _ui->menuBar->hide();
        _ui->mainToolBar->hide();
        _areaTabBar->hide();
        statusBar()->hide();
        showFullScreen();
    } else {
        statusBar()->show();
        _areaTabBar->show();
        _ui->mainToolBar->show();
        _ui->menuBar->show();
        showNormal();
    }

}

void ESToolkit::onAltTabShortcutTriggered(void) {
    const auto count = _areaTabBar->count();
    if(!count) return;

    auto prevIndex = _areaTabBar->currentIndex();
    auto newIndex = prevIndex + 1;
    if(newIndex >= count) newIndex = 0;

    if(prevIndex != newIndex) {
        _areaTabBar->setCurrentIndex(newIndex);
    }
}

void ESToolkit::searchDocs(QString query) {
    _instance->_helpDockWidget->openSearch(query);
}

void ESToolkit::openDoc(QString page, QString section) {
    _instance->_helpDockWidget->openPage(page, section);
}


void ESToolkit::on_actionTexture_Dump_triggered()
{
    gyVidTexWalkthrough();
}
