#include <QGLWidget>
#include <QSizePolicy>
#include <QToolButton>
#include <QCloseEvent>
#include <QSettings>
#include <QToolButton>
#include <QActionGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QDebug>

#include "area.h"
#include "debug.h"
#include "level.h"
#include "assets.h"
#include "entity.h"
#include "project.h"
#include "estoolkit.h"
#include "ui_estoolkit.h"
#include "assetManager.h"
#include "engineDialog.h"
#include "tileSelectable.h"
#include "elysian_asset.hpp"
#include "elysian_behavior.hpp"
#include "sheetManager.h"
#include "engineSettingsDialog.h"
#include "sheetViewTileGraphicsItem.h"
#include "glUtilities.h"

#include "assetDockWidget.h"
#include "layerDockWidget.h"
#include "sheetDockWidget.h"
#include "entityDockWidget.h"
#include "historyDockWidget.h"
#include "selectionDockWidget.h"

ESToolkit::ESToolkit(QWidget *parent):
	QMainWindow(parent),
	_ui(new Ui::ESToolkit),
	_ignoreAreaTabChange(false)
{

	// VERY IMPROTANT GLOBAL STATE/STATIC INITIALIZATION
	Debug::getInstance();
	AssetManager::_assets = &_assets;
	elysian::Behavior::staticConstructor();

	const QString verString = QString("ESTkv")+QString(ESTOOLKIT_VERSION)+QString(" \"Audacious Cuntwhisp\" Release");
	const char* platString =
#ifdef __APPLE__
	"OSX";
#elif defined(WIN32)
	"WINDOWS";
#else
	"LINUX";
#endif

	Debug::Logf(Debug::VERBOSE, "Initializing %s [%s]", verString.toStdString().c_str(), platString);

	_ui->setupUi(this);
	setWindowTitle(verString);

	//Instantiate external dialogs/tools
	_sheetManager = new SheetManager(/*this*/);
	_engineDialog = new EngineDialog(/*this*/);
	_engineSettingsDialog = new EngineSettingsDialog(/*this*/);
	connect(_engineSettingsDialog, SIGNAL(dcIpChanged(QString)), _engineDialog, SLOT(OnDreamcastIpChanged(QString)));
	connect(_engineSettingsDialog, SIGNAL(dcMacChanged(QString)), _engineDialog, SLOT(OnDreamcastMacChanged(QString)));
	connect(_engineSettingsDialog, SIGNAL(sudoPswdChanged(QString)), _engineDialog, SLOT(OnSudoPswdChanged(QString)));
	_engineSettingsDialog->restoreSettings();

	_areaTabBar = new QTabBar(this);
	QVBoxLayout *boxLayout = qobject_cast<QVBoxLayout*>(_ui->centralWidget->layout());
	Q_ASSERT(boxLayout);
	boxLayout->insertWidget(0, _areaTabBar, 0, Qt::AlignLeft);

	// set action groups here
	m_platformActionGroup = new QActionGroup(_ui->menuSelect_Platform);
	m_platformActionGroup->addAction(_ui->actionNix);
	m_platformActionGroup->addAction(_ui->actionWindows);
	m_platformActionGroup->addAction(_ui->actionMac_OS_X);
	m_platformActionGroup->addAction(_ui->actionDreamcast);
	m_platformActionGroup->addAction(_ui->actionIPhone_iPad);
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
	//_entityDockWidget = new EntityDockWidget(this);
	//addDockWidget(Qt::LeftDockWidgetArea, _entityDockWidget);

	// create/initialize LAYER dock and widget
	_layerDockWidget = new LayerDockWidget(this);
	addDockWidget(Qt::LeftDockWidgetArea, _layerDockWidget);

	// create/initialize SHEET dock and widget
	_sheetDockWidget = new SheetDockWidget(this);
	addDockWidget(Qt::BottomDockWidgetArea, _sheetDockWidget);

	// create/initialize SELECTION dock and widget
	_selectionDockWidget = new SelectionDockWidget(this);
	addDockWidget(Qt::RightDockWidgetArea, _selectionDockWidget);

	// create/initialize HISTORY dock and widget
	_historyDockWidget = new HistoryDockWidget(this);
	_historyDockWidget->SetActiveGroup(&_undoGroup);
	addDockWidget(Qt::RightDockWidgetArea, _historyDockWidget);

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
	_ui->mapView->setScene(Area::editorScene);
	_ui->mapView->setFocus();
	//setFocusPolicy(Qt::ClickFocus);

	//CONNECT ASSET SIGNALS/SLOTS TO MAIN UI
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
	//connect(_entityDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
	connect(_sheetDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
	connect(_sheetDockWidget, SIGNAL(ReleaseFocus()), this, SLOT(GiveMapViewFocus()));
	connect(_sheetDockWidget, SIGNAL(ExitPanMode()), this, SLOT(ExitPanMode()));
	connect(_historyDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));
	connect(_selectionDockWidget, SIGNAL(UncheckMe(int)), this, SLOT(UpdateDockWidgetChecked(int)));

	glUtilities::printGLSupport();

	ReadSettings();

	setMouseTracking(true);
}

ESToolkit::~ESToolkit(void) {
	_assets.setCurrentArea(NULL);
	_assets.setCurrentProject(NULL);
	_assets.setCurrentLevel(NULL);
	_areaTabBar->disconnect();
	_assetDockWidget->disconnect();
	_assets.disconnect();
	delete _ui;
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

			QMessageBox quitAlert(QMessageBox::Question, "Save Project?", "Would you like to save changes to your project before you quit?", QMessageBox::Save | QMessageBox::Cancel | QMessageBox::No);
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

void ESToolkit::ToogleProjectActions(bool enabled)
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
	m_activePlatform->setEnabled(enabled);
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
   // settings.setValue("entityDock", _entityDockWidget->isVisible());
	settings.setValue("layerDock", _layerDockWidget->isVisible());
	settings.setValue("sheetDock", _sheetDockWidget->isVisible());
	settings.setValue("selectionDock", _selectionDockWidget->isVisible());
	settings.setValue("glEnabled", _ui->actionEnable_OpenGL->isChecked());
	settings.setValue("autoScrollEnabled", _ui->actionToggle_AutoScroll->isChecked());
	settings.setValue("sheetViewHorizontal", _ui->actionHorizontal_SheetView->isChecked());
	settings.setValue("engineResolution", _engineDialog->getResolution());
	unsigned x, y;
	_engineDialog->getStartPos(x, y);
	settings.setValue("engineStartPosX", x);
	settings.setValue("engineStartPosY", y);
}

void ESToolkit::ReadSettings()
{
	ToogleProjectActions(true);
	_assetDockWidget->setEnabled(false);
  //  _entityDockWidget->setEnabled(false);
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

	_assets.setCurrentProject(NULL);

	QSettings settings;
	QString savedSettings = settings.value("settings", "").toString();
	QByteArray geo = settings.value("geometry", QByteArray()).toByteArray();
	QByteArray state = settings.value("state", QByteArray()).toByteArray();
	//QString dir = settings.value("projectDir", QString()).toString();
	bool sheetViewHorizontal = settings.value("sheetViewHorizontal", true).toBool();
	bool glEnabled = settings.value("glEnabled", true).toBool();
	bool autoScrollEnabled = settings.value("autoScrollEnabled", true).toBool();
	_engineDialog->setResolution(settings.value("engineResolution", 0).toUInt());
	unsigned x = settings.value("engineStartPosX", 0).toUInt();
	unsigned y = settings.value("engineStartPosY", 0).toUInt();
	_engineDialog->setStartPos(x, y);

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
	on_actionHorizontal_SheetView_triggered(sheetViewHorizontal);


	ToogleProjectActions(false);

}

void ESToolkit::OnAreaRenamed(const QString /*oldName*/, const QString /*newName*/) {
	_ignoreAreaTabChange = true;

	while(_areaTabBar->count()) _areaTabBar->removeTab(0);
	for(unsigned int i = 0; i < _assets.getCurrentLevel()->getAreaAmount(); ++i) {
		_areaTabBar->addTab(_assets.getCurrentLevel()->getAreaByIndex(i)->getName());
	}

	for(int i = 0; i < _areaTabBar->count(); ++i) {
		if(_areaTabBar->tabText(i) == _assets.getCurrentArea()->getName()) _areaTabBar->setCurrentIndex(i);
	}

	_ignoreAreaTabChange = false;

}

void ESToolkit::OnAddArea(Project *const /*project*/, Level *const level, Area *const area) {

	_undoGroup.addStack(&area->getUndoStack());

	if(level == _assets.getCurrentLevel()) { //Don't bother updating the tabs otherwise
		_ignoreAreaTabChange = true;
		_areaTabBar->addTab(area->getName());
		_ignoreAreaTabChange = false;
	}
}

void ESToolkit::OnRemoveArea(Project *const /*project*/, Level *const level, Area *const area) {
	_undoGroup.removeStack(&area->getUndoStack());

	if(level == _assets.getCurrentLevel()) { //Don't bother updating the tabs otherwise
		for(int i = 0; i < _areaTabBar->count(); ++i) {
			if(_areaTabBar->tabText(i) == area->getName()) {
				_ignoreAreaTabChange = true;
				_areaTabBar->removeTab(i);
				_ignoreAreaTabChange = false;
				return;
			}
		}
	}
}

void ESToolkit::OnTabChanged(int index) {
	if(_ignoreAreaTabChange) return;
	if(_assets.getCurrentArea() && _assets.getCurrentArea()->getName() == _areaTabBar->tabText(index)) return;
	_assets.setCurrentArea(_areaTabBar->tabText(index));
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
		Area::inactiveTransparency = visible;
		break;
	case 7:
		TileEntry::renderElev = visible;
		break;
	case 8:
		TileEntry::renderIndeces = visible;
		Level::updateScenes();
		break;
	case 9:
		TileEntry::renderZTran = visible;
		break;
	case 10:
		TileEntry::renderFlipRot = visible;
		break;
	default:
		break;
	}

	_layerDockWidget->setCurrentLayer(_assets.getCurrentLayer());
	Area::updateScene();
}

void ESToolkit::OnActiveLayerChanged(int index) {
   switch(index) {
	case 1:
		//_layerDockWidget->SetCurrentLayerIndex();
		OnLayerChanged(Assets::LAYER_LIGHT);
		break;
	case 2:
		OnLayerChanged(Assets::LAYER_TILE3);
		break;
	case 3:
		OnLayerChanged(Assets::LAYER_TILE2);
		break;
	case 4:
		OnLayerChanged(Assets::LAYER_TILE1);
		break;
	default:
	   //Fucking LayerTreeWidget just set the current item to an item
	   //that shouldn't be selectable... set it back.
	   QTimer::singleShot(10, this, SLOT(updateLayerTreeWidget()));
		break;
	}
	Area::updateScene();
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


void ESToolkit::OnAreaChanged(Area *const area) {
	if(area) {
		_ui->actionAreaAddEntity->setDisabled(false);
		_ui->actionAreaRemove->setDisabled(false);
		_ui->menuArea->setDisabled(false);
		_ui->actionFileSaveCurrentArea->setDisabled(false);
		_undoGroup.setActiveStack(&area->getUndoStack());
		_ui->mapView->setEnabled(true);
	}
	else {
		_ui->actionAreaAddEntity->setDisabled(true);
		_ui->actionAreaRemove->setDisabled(true);
		_ui->menuArea->setDisabled(true);
		_ui->actionFileSaveCurrentArea->setDisabled(true);
		_ui->mapView->setEnabled(false);
	}

	//Update tabs
	if(area) {
		for(int i = 0; i < _areaTabBar->count(); ++i) {
			if(_areaTabBar->tabText(i) == area->getName()) _areaTabBar->setCurrentIndex(i);
		}
	}
	_ui->mapView->deleteSelectionItemGroup();
	//auto boundingRect = Area::editorScene->itemsBoundingRect();
	//boundingRect.setTop(boundingRect.getTop()-16.0f);
	//boundingRect.setLeft(boundingRect.getLeft()-16.0f);
	//boundingRect.setBottom(boundingRect.getBottom()+16.0f);
	//boundingRect.setRight(boundingRect.getRight()+16.0f);
	//Area::editorScene->setSceneRect();
	_ui->mapView->scene()->update();
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

	}
	else {
		_ui->actionLevelReloadSheets->setDisabled(true);
		_ui->actionLevelRemove->setDisabled(true);
		_ui->menuLevel->setEnabled(false);
		_ui->actionFileSaveCurrentLevel->setDisabled(true);
		_ui->actionFileNewArea->setEnabled(false);
		_sheetDockWidget->setViewEnabled(false);
	}

	//Update tabs
	if(level) {

		_ignoreAreaTabChange = true;
		while(_areaTabBar->count()) _areaTabBar->removeTab(0);

		for(unsigned int i = 0; i < _assets.getCurrentLevel()->getAreaAmount(); ++i) {
			_areaTabBar->addTab(_assets.getCurrentLevel()->getAreaByIndex(i)->getName());
		}
		//_areaTabBar->setCurrentIndex(0);
		_ignoreAreaTabChange = false;
	}
	else {
		while(_areaTabBar->count()) _areaTabBar->removeTab(0);
	}

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

void ESToolkit::on_actionFileOpenProject_triggered(){

	QString dir =  QFileDialog::getExistingDirectory(this, tr("Locate Project Directory"), "./", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(dir=="") {
		return;
	}

	Project *proj = _assets.loadProject(dir);

	// ENABLE ALL UI VIEWS
	if(proj != NULL) {
		ToogleProjectActions(true);
		_assetDockWidget->setEnabled(true);
	   // _entityDockWidget->setEnabled(true);
		_layerDockWidget->ToggleDefaultItem(true);
		_layerDockWidget->setEnabled(true);
		_sheetDockWidget->CenterSheetView(0, 0);
		_sheetDockWidget->setEnabled(true);
		_selectionDockWidget->setEnabled(true);
		_historyDockWidget->setEnabled(true);
		//_ui->mapView->setEnabled(true);
	}

	_assets.setCurrentProject(proj);
	_assets.setCurrentSelection(_assets.getCurrentArea());
	//_ui->mapView->setFocus();

}

void ESToolkit::on_actionFileCloseProject_triggered()
{
	//emit CloseProject
	_assets.removeProject(_assets.getCurrentProject()->getName());
}

void ESToolkit::on_actionFileSaveCurrentProject_triggered() {
	//check failure!
	Debug::Log(Debug::VERBOSE, "ESToolkit::on_actionFileSaveCurrentProject_triggered");
	Debug::Push();
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
	_assets.setCurrentSelection(_assets.getCurrentArea()->newEntity());
}

void ESToolkit::on_actionDebugViewLog_triggered()
{
	Debug::getInstance().show();
}

void ESToolkit::on_actionEntityRemove_triggered() {
	if(QMessageBox::critical(NULL, tr("Warning!"), tr("Are you SURE that you want to permanently delete the current entity?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		return;
	}

	_assets.getCurrentArea()->deleteEntity((Entity *)_assets.getCurrentSelection());
}

void ESToolkit::on_actionEntityAttachRenderable_triggered() {
	((Entity *)_assets.getCurrentSelection())->AttachRenderable();
}

void ESToolkit::on_actionEntityAttachAnimation_triggered() {
	((Entity *)_assets.getCurrentSelection())->AttachAnimation();
}

void ESToolkit::on_actionEntityAttachCollidable_triggered() {
	((Entity *)_assets.getCurrentSelection())->AttachCollidable();
}

void ESToolkit::on_actionEntityAttachWarp_triggered() {
	((Entity *)_assets.getCurrentSelection())->AttachWarp();
}

void ESToolkit::on_actionEntityAttachItem_triggered() {
   ((Entity *)_assets.getCurrentSelection())->AttachItem();
}

void ESToolkit::on_actionEntityAttachCharacter_triggered() {
	((Entity *)_assets.getCurrentSelection())->AttachCharacter();
}

void ESToolkit::on_actionEntityAttachFriendly_triggered() {
	((Entity *)_assets.getCurrentSelection())->AttachFriendly();
}

void ESToolkit::on_actionEntityAttachEnemy_triggered() {
   ((Entity *)_assets.getCurrentSelection())->AttachEnemy();
}

void ESToolkit::on_actionEntityRemoveRenderable_triggered() {
	((Entity *)_assets.getCurrentSelection())->RemoveRenderable();
}

void ESToolkit::on_actionEntityRemoveAnimation_triggered() {
	((Entity *)_assets.getCurrentSelection())->RemoveAnimation();
}

void ESToolkit::on_actionEntityRemoveCollidable_triggered() {
	((Entity *)_assets.getCurrentSelection())->RemoveCollidable();
}

void ESToolkit::on_actionEntityRemoveWarp_triggered() {
	((Entity *)_assets.getCurrentSelection())->RemoveWarp();
}

void ESToolkit::on_actionEntityRemoveItem_triggered() {
	((Entity *)_assets.getCurrentSelection())->RemoveItem();
}

void ESToolkit::on_actionEntityRemoveCharacter_triggered() {
	((Entity *)_assets.getCurrentSelection())->RemoveCharacter();
}

void ESToolkit::on_actionEntityRemoveFriendly_triggered() {
	((Entity *)_assets.getCurrentSelection())->RemoveFriendly();
}

void ESToolkit::on_actionEntityRemoveEnemy_triggered() {
	((Entity *)_assets.getCurrentSelection())->RemoveEnemy();
}

void ESToolkit::on_actionAreaRemove_triggered() {
	if(QMessageBox::critical(NULL, tr("Warning!"), tr("Are you SURE that you want to permanently delete the current area?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		return;
	}

	QString name = _assets.getCurrentArea()->getName();
	if(_assets.getCurrentSelection() == _assets.getCurrentArea()) _selectionDockWidget->SetSelectionModel(NULL);
	_assets.setCurrentArea(NULL);
	_assets.getCurrentLevel()->removeArea(name);
	_assets.setCurrentArea(_assets.getCurrentLevel()->getFirstArea());
}


void ESToolkit::on_actionLevelRemove_triggered() {
	if(QMessageBox::critical(NULL, tr("Warning!"), tr("Are you SURE that you want to permanently delete the current level?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		return;
	}

	QString name = _assets.getCurrentLevel()->getName();
	if(_assets.getCurrentSelection() == _assets.getCurrentLevel()) _selectionDockWidget->SetSelectionModel(NULL);
	_assets.setCurrentLevel(NULL);
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
	_selectionDockWidget->SetSelectionModel(NULL);
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
		static_cast<Area *>(selection)->populateModel();
		_selectionDockWidget->SetSelectionModel(static_cast<Area *>(selection));
		_selectionDockWidget->ExpandSelectionTree();
		break;
	case Selectable::TILE:
		 _ui->mapView->createSelectionItemGroup();
		 static_cast<TileSelectable *>(selection)->populateModel();
		 _selectionDockWidget->SetSelectionModel(static_cast<TileSelectable *>(selection));
		 if(static_cast<TileSelectable *>(selection)->getSize() == 1) _selectionDockWidget->ExpandSelectionTree();
		 selection->installSelectionViewCustomDelegates(_selectionDockWidget->getSelectionTreeWidget());
		_selectionDockWidget->SetSelectionTreeWidget();
		break;
	case Selectable::ENTITY:
		//Update Menu Bar
		SyncEntityMenuBar(static_cast<Entity *>(selection));

		//Update Entity View
		_selectionDockWidget->SetSelectionWidget((QWidget *)(Entity *)selection);

		//center on entity
		_ui->mapView->centerOn((Entity *)selection);
		//_ui->mapView->SetCenter((Entity *)selection);
		break;
		case Selectable::EMPTY:
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
		Debug::Log(Debug::CRITICAL, "Action Redo triggered, but there was no Area! Aborting function call...");
		return;
	}
	_assets.getCurrentArea()->getUndoStack().redo();
}

void ESToolkit::on_actionFileNewLevel_triggered() {

	//QMessageBox::critical(this, tr("Error!"), tr("This feature won't be implemented until ESTkV1.5.2. Do it manually. ;)"));
	Level *newLev = _assets.getCurrentProject()->newLevel();
	_assets.setCurrentLevel(newLev);
	_assets.setCurrentSelection(newLev);
}

void ESToolkit::on_actionFileNewArea_triggered() {
	Area *newArea = _assets.getCurrentLevel()->newArea();
	_assets.setCurrentArea(newArea);
	_assets.setCurrentSelection(newArea);
}

void ESToolkit::OnProjectRemoved() {
	ToogleProjectActions(false);
	_assetDockWidget->ClearTreeView();
	_assetDockWidget->setEnabled(false);
  //  _entityDockWidget->setEnabled(false);
	_layerDockWidget->ToggleDefaultItem(false);
	_layerDockWidget->setEnabled(false);
	_sheetDockWidget->setEnabled(false);
	_historyDockWidget->setEnabled(false);
	_selectionDockWidget->setWidget(NULL);
	_selectionDockWidget->setEnabled(false);
	_ui->mapView->deleteSelectionItemGroup();
	_ui->mapView->setEnabled(false);
}

void ESToolkit::on_actionEnable_OpenGL_triggered() {
	if(!glUtilities::glSupported()) return;

	_ui->mapView->setViewport((_ui->actionEnable_OpenGL->isChecked())? new QGLWidget : new QWidget);
	_ui->mapView->setMouseTracking(true);
	_ui->mapView->viewport()->setMouseTracking(true);
	_sheetDockWidget->getSourceView()->setViewport((_ui->actionEnable_OpenGL->isChecked())? new QGLWidget : new QWidget);
	_sheetDockWidget->getSourceView()->setMouseTracking(true);
	_sheetDockWidget->getSourceView()->viewport()->setMouseTracking(true);
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

	_assets.getCurrentArea()->save();
	_engineDialog->show();

	if(_ui->actionWindows->isChecked() || _ui->actionMac_OS_X->isChecked() || _ui->actionNix->isChecked())
		_engineDialog->Invoke(EngineDialog::ENGINE_PC);
	else if(_ui->actionDreamcast->isChecked()) {
		if(!_engineDialog->Invoke(EngineDialog::ENGINE_DC)) {
			_engineDialog->close();
			_engineSettingsDialog->show();
			_engineSettingsDialog->setFocus();
			//Debug::Log(Debug::CRITICAL, "InvokeEngine(DREAMCAST) failed! Invalid Settings.");
		}
	}
	else if(_ui->actionIPhone_iPad->isChecked())
		_engineDialog->Invoke(EngineDialog::ENGINE_iOS);
}

void ESToolkit::on_actionAsset_View_triggered(bool checked)
{
	_assetDockWidget->setVisible(checked);
	_ui->actionAsset_View->setChecked(checked);
}

void ESToolkit::on_actionEntity_View_triggered(bool checked)
{
   // _entityDockWidget->setVisible(checked);
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
/*
	if(dockWidgetArea(_entityDockWidget) != Qt::LeftDockWidgetArea || _entityDockWidget->isFloating() || !_entityDockWidget->isVisible()) {
		_entityDockWidget->setFloating(false);
		removeDockWidget(_entityDockWidget);
		addDockWidget(Qt::LeftDockWidgetArea, _entityDockWidget);
		_entityDockWidget->setVisible(true);
		_ui->actionEntity_View->setChecked(true);
	}*/

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

void ESToolkit::on_actionPaste_triggered()
{
	_ui->mapView->selectionPaste();
}

void ESToolkit::on_actionDefault_Transform_triggered()
{
	_ui->mapView->defaultTransform();

}

void ESToolkit::on_actionToggle_AutoScroll_triggered()
{
	_ui->mapView->autoScrollToggle(_ui->actionToggle_AutoScroll->isChecked());
	_sheetDockWidget->getSourceView()->autoScrollToggle(_ui->actionToggle_AutoScroll->isChecked());
}

void ESToolkit::on_actionActionSecret_triggered()
{
	if(_assets.getCurLevel()) {
		_sheetManager->initialize(_ui->actionEnable_OpenGL->isChecked(), _ui->actionToggle_AutoScroll->isChecked());
		_sheetManager->setFocus();
		_sheetManager->showMaximized();
	}
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
	//Area::editorScene->setSceneRect(Area::editorScene->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
	_ui->mapView->scale(0.1f, 0.1f);
	QImage image(Area::editorScene->sceneRect().size().toSize(), QImage::Format_ARGB32);              // Create the image with the exact size of the shrunk scene
	image.fill(Qt::transparent);                                                                      // Start all pixels transparent

	QPainter painter(&image);
	Area::editorScene->render(&painter);

	QDir dir("");
	if(!dir.exists("screenshots")) dir.mkdir("screenshots");
	QString fileBase("screenshots/estkScreenshot");
	unsigned num = 1;

	while(QFile::exists(fileBase+QString::number(num)+QString(".png"))) ++num;
	image.save(fileBase+QString::number(num)+QString(".png"));

	_ui->mapView->scale(10.0f, 10.0f);
}
