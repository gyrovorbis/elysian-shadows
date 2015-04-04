#include "elysian_area.hpp"
#include <QMenu>
#include <QActionGroup>
#include <QToolButton>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTransform>
#include <QFileDialog>
#include <QStatusBar>
#include <QShortcut>
#include <QFileSystemWatcher>
#include <QPushButton>
#define QT_NO_OPENGL_ES_2
#include <QOpenGLWidget>

#include "elysian_debug.hpp"
#include "sheetManager.h"
#include "ui_sheetmanager.h"
#include "sheetManagerFileDockWidget.h"
#include "historyDockWidget.h"
#include "project.h"
#include "elysian_level.hpp"
#include "tileSheetUtilities.h"
#include "sheetManagerSourceTileGraphicsItem.h"
#include "tileImage.h"
#include "sheetManagerDestTileGraphicsItem.h"
#include "project.h"
#include "checkeredAlphaRectGraphicsItem.h"
#include "glUtilities.h"

//=====STATIC=====
QPixmap SheetManager::reorientPixmap(const QPixmap &pixmap, const GY_RECT_TEX_ORIENT orient) {
	QTransform transform;

	switch(orient) {
	case GY_RECT_TEX_ORIENT_NORMAL:
		return pixmap;
	case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
		transform.scale(-1.0f, 1.0f);
		return pixmap.transformed(transform);
	case GY_RECT_TEX_ORIENT_FLIP_VERT:
		transform.scale(1.0f, -1.0f);
		return pixmap.transformed(transform);
	case GY_RECT_TEX_ORIENT_ROT_90:
		transform.rotate(-90.0f);
		return pixmap.transformed(transform);
	case GY_RECT_TEX_ORIENT_ROT_180:
		transform.rotate(180.0f);
		return pixmap.transformed(transform);
	case GY_RECT_TEX_ORIENT_ROT_270:
		transform.rotate(-270.0f);
		return pixmap.transformed(transform);
	case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
		transform.scale(-1.0f, 1.0f);
		transform.rotate(-90.0f);
		return pixmap.transformed(transform);
	case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
		transform.scale(-1.0f, 1.0f);
		transform.rotate(-270.0f);
		return pixmap.transformed(transform);
	default:
		Q_ASSERT(0);
		return QPixmap();
	}
}


//=====INSTANCE=====

SheetManager::SheetManager(QWidget *parent):
	QMainWindow(parent),
	_ui(new Ui::SheetManager),
	_ignoreSheetTabChange(true)
{
	_ui->setupUi(this);
	_ui->_destView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	_ui->_srcView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

	// initialize FileDockWidget
	_fileDockWidget = new SheetManagerFileDockWidget(this);
	addDockWidget(Qt::LeftDockWidgetArea, _fileDockWidget);
	connect(_fileDockWidget, SIGNAL(imageSelected(QString)), this, SLOT(_openSourceSheet(QString)));

	// initialize HistoryDockWidget
	_historyDockWidget = new HistoryDockWidget(this);
	//_historyDockWidget->SetActiveStack(&_undoStack);
	addDockWidget(Qt::LeftDockWidgetArea, _historyDockWidget);

	// add source level selection dropdown
	_srcLevelMenu = new QMenu(this);
	_srcLevelGroup = new QActionGroup(_srcLevelMenu);
	_srcLevelGroup->setExclusive(true);
	_srcLevelDropdown = new QToolButton;
	_srcLevelDropdown->setToolTip("Select Level...");
	_srcLevelDropdown->setMenu(_srcLevelMenu);
	_srcLevelDropdown->setToolButtonStyle(Qt::ToolButtonTextOnly);
	_srcLevelDropdown->setPopupMode(QToolButton::MenuButtonPopup);
	_srcLevelDropdown->setText("OTHER");
	_ui->_srcVerticalLayout->insertWidget(0, _srcLevelDropdown, 0, Qt::AlignLeft);

	// add source tabs for sheet images
	//_srcLevelTabBar = new QTabBar(_ui->centralwidget);
	_srcOtherTabBar = new QTabBar(_ui->centralwidget);
	_srcOtherTabBar->setTabsClosable(true);
	//_srcOtherTabBar->setMaximumWidth(440);
	//_srcOtherTabBar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	_ui->_srcVerticalLayout->insertWidget(1, _srcOtherTabBar, 0, Qt::AlignLeft);

	// add dest level selection dropdown
	_destLevelMenu = new QMenu(this);
	_destLevelGroup = new QActionGroup(_destLevelMenu);
	_destLevelGroup->setExclusive(true);
	_destLevelDropdown = new QToolButton;
	_destLevelDropdown->setToolTip("Select Level...");
	_destLevelDropdown->setMenu(_destLevelMenu);
	_destLevelDropdown->setToolButtonStyle(Qt::ToolButtonTextOnly);
	_destLevelDropdown->setPopupMode(QToolButton::MenuButtonPopup);
	_destLevelDropdown->setText("none");
	_ui->_destVerticalLayout->insertWidget(0, _destLevelDropdown, 0, Qt::AlignLeft);

	SheetManagerAssetManager::_assets = &this->_assets;

	// initialize destination scene
	_destSheetScene = new QGraphicsScene;
	for(unsigned j = 0; j < 32; ++j) {
		for(unsigned i = 0; i < 16; ++i) {
			if(i == 0 && j == 0)  {
				QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap(SHEET_MANAGER_RESERVED_TILE_IMAGE));
				item->setPos(-16, -16);
				item->setZValue(5.0f);
				_destSheetScene->addItem(item);
			}
			_destSheetScene->addItem(new SheetManagerDestTileGraphicsItem(i, j, _ui->_destView));
		}
	}
	_destSceneBgRect = new CheckeredAlphaRectGraphicsItem;
	_destSceneBgRect->setRect(QRect(-16, -16, 32*16, 32*32));
	_destSceneBgRect->setZValue(0.0f);
	_destSheetScene->addItem(_destSceneBgRect);
	_ui->_destView->setScene(_destSheetScene);

	// Initialize status bar
	QLabel *label = new QLabel;
	label->setPixmap(QPixmap(":/statusbar/images/absPosStatusIcon.png"));
	statusBar()->addWidget(label, 0);
	statusBar()->addWidget(_ui->_destView->getSceneMousePosLabel(), 1);

	label = new QLabel;
	label->setPixmap(QPixmap(":/statusbar/images/tilePosStatusIcon.png"));
	statusBar()->addWidget(label, 0);
	statusBar()->addWidget(_ui->_destView->getSceneMouseTileLabel(), 1);

	label = new QLabel;
	label->setPixmap(QPixmap(":/statusbar/images/selectionSizeStatusIcon.png"));
	statusBar()->addWidget(label, 0);
	statusBar()->addWidget(_ui->_destView->getSceneSelectionSizeLabel(), 1);

	label = new QLabel;
	label->setPixmap(QPixmap(":/statusbar/images/zoomStatusIcon.png"));
	statusBar()->addWidget(label, 0);
	statusBar()->addWidget(_ui->_destView->getZoomLabel(), 2);

	_watcher = new QFileSystemWatcher(this);

	//Keyboard shortcuts
	QShortcut *shortcut = new QShortcut(QKeySequence(tr("Ctrl+Z", "Undo")),
							  this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(onUndo_triggered()));
	shortcut = new QShortcut(QKeySequence(tr("Ctrl+Y", "Redo")),
							  this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(onRedo_triggered()));

	connect(_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(onFileChanged(const QString&)));
	connect(AssetManager::getAssets(), SIGNAL(levelAdded(Project *const, Level  *const)), this, SLOT(onLevelAdded(Project * const, Level * const)));
	connect(AssetManager::getAssets(), SIGNAL(levelRemoved(Project*const,Level*const)), this, SLOT(onLevelRemoved(Project*const,Level*const)));
	connect(AssetManager::getAssets(), SIGNAL(levelRenamed(QString,QString)), this, SLOT(onLevelRenamed(QString,QString)));
	connect(_srcOtherTabBar, SIGNAL(currentChanged(int)), this, SLOT(onSourceOtherTabChanged(int)));
	connect(_srcOtherTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(onSourceOtherTabClose(int)));
	connect(_destLevelMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeActiveLevel(QAction*)));
	connect(getSMAssets(), SIGNAL(onCurrentSelectionChanged(SheetManagerTileSelection*const)), this, SLOT(onSelectionChanged(SheetManagerTileSelection*const)));
	connect(_ui->_srcView, SIGNAL(releaseFocus()), this, SLOT(focusDestView()));

	if(!glUtilities::glSupported()) {
		_ui->actionEnable_OpenGL->setDisabled(true);
	}

	_ui->_srcView->setFocus();
}

SheetManager::~SheetManager(void) {
	delete _ui;
	for(QMap<QString, SourceSheetEntry *>::iterator i = _srcSheets.begin();
			i != _srcSheets.end(); ++i) delete i.value();
}

void SheetManager::initialize(const bool gl, const bool autoScroll) {
	Assets &assets = *AssetManager::getAssets();

    if(_destLevelDropdown->text() == assets.getCurrentLevel()->getNodeName() || _assets.getUndoStack()->count()) return;

	//_assets.setCurrentLevel(assets.getCurrentLevel());
	//_assets.setCurrentTileType(assets.getCurrentLayerTileType());
	_historyDockWidget->SetActiveStack(_assets.getUndoStack());
	_ignoreSheetTabChange = true;

    _destLevelDropdown->setText(assets.getCurrentLevel()->getNodeName());

	_ui->actionEnable_OpenGL->setChecked(gl);
	on_actionEnable_OpenGL_triggered();
	_ui->actionToggle_AutoScroll->setChecked(autoScroll);
	on_actionToggle_AutoScroll_triggered();


	for(int l = 0; l < _destLevelActions.size(); ++l) {
        if(_destLevelActions[l]->text() == assets.getCurrentLevel()->getNodeName()) {
			_destLevelActions[l]->setChecked(true);
			changeActiveLevel(_destLevelActions[l]);
		}
	}
}


void SheetManager::closeEvent(QCloseEvent *) {
	QMessageBox msgBox;
	msgBox.setText("Save modified sheet? Changes will not be saved to disk without applying them.");
	QPushButton *discardButton	= msgBox.addButton(QMessageBox::Discard);
	/*QPushButton *saveButton	=*/ msgBox.addButton(tr("Save without applying"), QMessageBox::ActionRole);
	QPushButton *applyButton	= msgBox.addButton(tr("Save and apply"), QMessageBox::ActionRole);

	if(_assets.getUndoStack()->isClean()) return;

	msgBox.exec();

	if(msgBox.clickedButton() == static_cast<QAbstractButton*>(applyButton)) {
		_save();
	} else if(msgBox.clickedButton() == static_cast<QAbstractButton*>(discardButton)) {
		//Without this, we can't lay the same selection in the previous location, even though it may have been undone
		_ui->_destView->setSelectionChanged(true);
		for(int i = _assets.getUndoStack()->index(); i >= 0; --i)
			_assets.getUndoStack()->undo();
	}

}


void SheetManager::focusDestView(void) {
	_ui->_destView->setFocus();
}

void SheetManager::_destViewForceUpdate(void) {
	_destSheetScene->removeItem(_destSceneBgRect);
	_destSheetScene->addItem(_destSceneBgRect);
	_ui->_destView->scene()->update();
	_ui->_destView->update();
}

void SheetManager::swapSheets(void) {
	//_assets.setCurrentTileType((_assets.getCurrentTileType() == Assets::TILE_TERRAIN)? Assets::TILE_OBJECT : Assets::TILE_TERRAIN);
	_historyDockWidget->SetActiveStack(_assets.getUndoStack());

	_destViewForceUpdate();
}

void SheetManager::changeActiveLevel(QAction *action) {
	if(!saveOrDiscard(true)) {
		//Change the level back to the previous one if the save was canceled.
		for(int l = 0; l < _destLevelActions.size(); ++l) {
			if(_destLevelActions[l]->text() == _destLevelDropdown->text()) {
				_destLevelActions[l]->setChecked(true);
			}
		}
		return;
	}

	_assets.setCurrentLevel(AssetManager::getAssets()->getCurrentProject()->getLevel(action->text()));
	if(!_assets.getCurrentLevel()->isOpen()) _assets.getCurrentLevel()->open();
	Q_ASSERT(_assets.getCurrentLevel());
	action->setChecked(true);
	_destLevelDropdown->setText(action->text());
	_destViewForceUpdate();

	//Populate Destination tile entries
//	for(int terType = Assets::TILE_TERRAIN; terType <= Assets::TILE_OBJECT; ++terType) {
		SheetManagerAssets::DestSheetEntry &destEntry = _assets.getDestSheetEntry();

		//destEntry.pixmaps.clear();
		//for(unsigned i = 0; i < 256; ++i) destEntry.pixmaps.push_back(_assets.getCurrentLevel()->getTileImage((Assets::TILE_TYPE)terType, i)->getPixmap());
		//for(unsigned i = 0; i < 256; ++i) destEntry.orients[i] = GY_RECT_TEX_ORIENT_NORMAL;

		for(unsigned i = 0; i < 512; ++i) {
			destEntry.tileEntry[i].setPixmap(_assets.getCurrentLevel()->getTileImage(i)->getPixmap());
			destEntry.tileEntry[i].setOrient(GY_RECT_TEX_ORIENT_NORMAL);
		}
//	}
}

void SheetManager::onLevelAdded(Project *const /*project*/, Level *const level) {
	//destination
    QAction *levelAction = new QAction(level->getNodeName(), this);
	levelAction->setCheckable(true);
	_destLevelMenu->addAction(levelAction);
	_destLevelGroup->addAction(levelAction);
	_destLevelActions.push_back(levelAction);
	//source
    levelAction = new QAction(level->getNodeName(), this);
	levelAction->setCheckable(true);
	_srcLevelMenu->addAction(levelAction);
	_srcLevelGroup->addAction(levelAction);
	_srcLevelActions.push_back(levelAction);
}

void SheetManager::onLevelRemoved(Project *const /*project*/, Level *const level) {
	//destination
	for(int i = 0; i < _destLevelActions.size(); ++i) {
        if(_destLevelActions[i]->text() == level->getNodeName()) {
			delete _destLevelActions[i];
			_destLevelActions.remove(i);
			break;
		}
	}
	//source
	for(int i = 0; i < _srcLevelActions.size(); ++i) {
        if(_srcLevelActions[i]->text() == level->getNodeName()) {
			delete _srcLevelActions[i];
			_srcLevelActions.remove(i);
			break;
		}
	}
}

void SheetManager::onLevelRenamed(const QString /*oldName*/, const QString /*newName*/) {
	//Clear all actions
	for(int i = 0; i < _destLevelActions.size(); ++i) {
		delete _destLevelActions[i];
		_destLevelActions.remove(i);
	}
	for(int i = 0; i < _srcLevelActions.size(); ++i) {
		delete _srcLevelActions[i];
		_srcLevelActions.remove(i);
	}

	//repopulate actions
	unsigned levIndex = 0;
	Level* lev = nullptr;
	while((lev = AssetManager::getAssets()->getCurrentProject()->getLevelByIndex(levIndex++))) {
		onLevelAdded(nullptr, lev);
	}
}


void SheetManager::onSourceOtherTabChanged(const int index) {
	if(index != -1) {
		_openSourceSheet(_srcSheetTabLookup[index]);
	}
}

void SheetManager::onSourceOtherTabClose(const int index) {
	_closeSourceSheet(_srcSheetTabLookup[index]);
}

void SheetManager::onSelectionChanged(SheetManagerTileSelection *const /*sel*/) {
	_ui->_destView->createSelectionItemGroup();
}

SourceSheetEntry *SheetManager::_createSourceSheetEntry(const QString filename, const SourceView *const view) {
	SourceSheetEntry *entry;
	unsigned sheetWidth, sheetHeight;

	Debug::Logf(Debug::VERBOSE, "SheetManager::_createSourceSheetEntry(%s)", filename.toStdString().c_str());
    Debug::push();

	if(!TileSheetUtilities::GetImageSize(filename, sheetWidth, sheetHeight)) {
        entry = nullptr;
	}
	else {
		if(sheetWidth%32 != 0 || sheetHeight%32 != 0) {
			QMessageBox::critical(0, tr("Error!"), tr("Your sheet dimensions are not a multiple of 32! You can still use the image, but the edges will be padded."));
		}

		//Create the entry
		entry = new SourceSheetEntry;
		entry->filename = filename;
		entry->width = sheetWidth;
		entry->height = sheetHeight;
	\
		//Make damn sure this can actually handle non multiples of 32 and just round to next tile!
		if(!TileSheetUtilities::DivideTileSheet(filename, sheetWidth, sheetHeight, 32, 32, entry->imgs)) {
			delete entry;
			entry = NULL;
		}
		else {

			entry->tileWidth = (sheetWidth%32)? sheetWidth/32+1 : sheetWidth / 32;
			entry->tileHeight = (sheetHeight%32)? sheetHeight/32+1 : sheetHeight / 32;

			//Safety check. Better ALWAYS be true!
			Q_ASSERT((entry->tileWidth * entry->tileHeight) == (unsigned)entry->imgs.size());

			entry->icon = new QIcon(filename);

			for(unsigned j = 0; j < entry->tileHeight; ++j) {
				for(unsigned i = 0; i < entry->tileWidth; ++i) {
					entry->scene.addItem(new SheetManagerSourceTileGraphicsItem(i, j, entry, view));
				}
			}
			QGraphicsRectItem *bgRect = new CheckeredAlphaRectGraphicsItem;
			bgRect->setRect(QRect(-16, -16, entry->tileWidth*32/*+entry->tileWidth*/, entry->tileHeight*32/*+entry->tileHeight*/));
			bgRect->setZValue(0.0f);
			entry->scene.addItem(bgRect);
		}
	}

    Debug::pop();
	return entry;
}

bool SheetManager::_openSourceSheet(const QString filename /*, bool forceReload=false*/) {
	//CHECK FOR LEVEL SHEET VS IMAGE?

	SourceSheetEntry *entry;
	QMap<QString, SourceSheetEntry *>::const_iterator i = _srcSheets.find(filename);

	//Sheet needs to be created
	if(i == _srcSheets.end()) {
		entry = _createSourceSheetEntry(filename, _ui->_srcView);
		if(!entry) {
			//qDebug() << "Shit has REESTED";
			return false; //couldn't create entry
		}

		_srcSheets.insert(filename, entry);
		_srcSheetTabLookup.push_back(filename);

		QString tabName = filename.split("/").last();
		_srcOtherTabBar->addTab(*entry->icon, tabName);
		_srcOtherTabBar->setCurrentIndex(_srcSheetTabLookup.size()-1);

		//add to filesystem monitor
		_watcher->addPath(filename);

	} else {
		entry = i.value();
		for(int i = 0; i < _srcSheetTabLookup.size(); ++i)
			if(_srcSheetTabLookup[i] == filename) _srcOtherTabBar->setCurrentIndex(i);
	}

	_ui->_srcView->setScene(&entry->scene);

	_assets.setCurrentSourceSheetEntry(entry);
	return true;
}

bool SheetManager::_closeSourceSheet(const QString filename) {
	QMap<QString, SourceSheetEntry *>::iterator i = _srcSheets.find(filename);
	SourceSheetEntry *entry;

	//Never existed...
	if(i == _srcSheets.end()) {
		return false;
	}

	entry = i.value();
	_srcSheets.remove(i.key());

	//remove from filesystem watcher
	_watcher->removePath(filename);

	if(entry == _assets.getCurrentSourceSheetEntry()) {
		_ui->_srcView->setScene(NULL);
		_assets.setCurrentSourceSheetEntry(NULL);
	}

	for(unsigned j = 0; j < (unsigned)_srcSheetTabLookup.size(); ++j) {
		if(_srcSheetTabLookup[j] == filename) {
			_srcSheetTabLookup.erase(_srcSheetTabLookup.begin() + j);
			_srcOtherTabBar->removeTab(j);
			break;
		}
	}

	delete entry;
	return true;
}

bool SheetManager::saveOrDiscard(bool canCancel) {
	//Check if we want to save changes to current level sheets.
	QMessageBox::StandardButton button;
	QMessageBox::StandardButtons flags = (QMessageBox::Discard | QMessageBox::Save);
	if(canCancel) flags |= QMessageBox::Cancel;

	if(_assets.getUndoStack()->count() > 0) {
		button = QMessageBox::question(this, "Save Changes?", "Do you want to save changes made to the tile sheet?", flags);
		switch(button) {
		case QMessageBox::Cancel:
			return false;
		case QMessageBox::Discard:
			//Without this, we can't lay the same selection in the previous location, even though it may have been undone
			_ui->_destView->setSelectionChanged(true);
			_assets.getUndoStack()->clear();
			break;
		case QMessageBox::Save:
			_save();
			break;
		default:
			Q_ASSERT(0);
		}

	}

	return true;
}

bool SheetManager::_save(void) {

	Debug::Logf(Debug::VERBOSE, "SheetManager::_save(%s)", _destLevelDropdown->text().toStdString().c_str());
    Debug::push();

	Level *const level = AssetManager::getAssets()->getCurrentProject()->getLevel(_destLevelDropdown->text());
	SheetManagerAssets::DestSheetEntry &tileDestEntry = _assets.getDestSheetEntry();
	bool retVal = false;


	for(unsigned i = 0; i < 512; ++i) level->getTileImage(i)->setPixmap(reorientPixmap(tileDestEntry.tileEntry[i].getPixmap(), tileDestEntry.tileEntry[i].getOrient()));
	retVal = level->saveTileSheet();
    retVal &= level->reloadSheets();
	//Without this, we can't lay the same selection in the previous location, even though it may have been undone
	_ui->_destView->setSelectionChanged(true);
    _assets.getUndoStack()->setClean();


    if(!retVal) Debug::Logf(Debug::CRITICAL, "Could not reload sheet!");
    Debug::pop();
	return retVal;
}



//========QT Designer Q_SLOTS========


void SheetManager::on_actionCut_triggered(void) {
	_ui->_destView->selectionCut();
}

void SheetManager::on_actionPaste_triggered(void) {
	_ui->_destView->selectionPaste();
}

void SheetManager::on_actionOpen_triggered() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Browse source image"), QString(), tr("Images (*.png *.jpeg *.jpg *.bmp *.gif)"));

	if(filename == "") {
		return;
	}

	_openSourceSheet(filename);
}

void SheetManager::on_actionSave_triggered() {
	_save();
}

void SheetManager::on_actionEnable_OpenGL_triggered() {
	if(!glUtilities::glSupported()) return;

	//Cheating and only allowing GL for dest view, because of how shittily Qt handles QGLWidgets without
	//a current scene. It artifacts out the ass and looks terrible.
	//_ui->_srcView->setViewport((_ui->actionEnable_OpenGL->isChecked())? new QGLWidget : new QWidget);
    _ui->_destView->setViewport((_ui->actionEnable_OpenGL->isChecked())? new QOpenGLWidget : new QWidget);
	if(_ui->_srcView->scene()) _ui->_srcView->scene()->update();
	//if(_ui->_destView->scene()) _ui->_destView->scene()->update();
}


void SheetManager::on_actionToggle_AutoScroll_triggered() {
	_ui->_srcView->autoScrollToggle(_ui->actionToggle_AutoScroll->isChecked());
	_ui->_destView->autoScrollToggle(_ui->actionToggle_AutoScroll->isChecked());
}

void SheetManager::on_actionDefault_Transform_triggered() {
	_ui->_srcView->resetTransform();
	_ui->_destView->resetTransform();
	//Force recalculating of sceneRect, in case selection reest fucks it up
	_ui->_destView->setSceneRect(QRect());
}

void SheetManager::on_actionReset_Layout_triggered() {
	if(dockWidgetArea(_fileDockWidget) != Qt::LeftDockWidgetArea || _fileDockWidget->isFloating() || !_fileDockWidget->isVisible()) {
		_fileDockWidget->setFloating(false);
		removeDockWidget(_fileDockWidget);
		addDockWidget(Qt::LeftDockWidgetArea, _fileDockWidget);
		_fileDockWidget->setVisible(true);
	}

	if(dockWidgetArea(_historyDockWidget) != Qt::LeftDockWidgetArea || _historyDockWidget->isFloating() || !_historyDockWidget->isVisible()) {
		_historyDockWidget->setFloating(false);
		removeDockWidget(_historyDockWidget);
		addDockWidget(Qt::LeftDockWidgetArea, _historyDockWidget);
		_historyDockWidget->setVisible(true);
	}
}

void SheetManager::onUndo_triggered(void) {
	_assets.getUndoStack()->undo();
}

void SheetManager::onRedo_triggered(void) {
	_assets.getUndoStack()->redo();
}

void SheetManager::onFileChanged(const QString &path) {
	QMessageBox msgBox;
	msgBox.setText(QString(path) + QString(" has been modified outside of Sheet Manager. Reload image?"));
	QPushButton *reloadButton	= msgBox.addButton(tr("Reload"), QMessageBox::ActionRole);
	QPushButton *cancelButton	= msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);

	msgBox.exec();

	if(msgBox.clickedButton() == static_cast<QAbstractButton*>(reloadButton)) {
		_closeSourceSheet(path);
		_openSourceSheet(path);

	} else if(msgBox.clickedButton() == static_cast<QAbstractButton*>(cancelButton)) {
		//don't do shit
	}

}
