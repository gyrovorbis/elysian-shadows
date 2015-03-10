#ifndef SHEETMANAGER_H
#define SHEETMANAGER_H

#include <QUndoStack>
#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QGraphicsScene>
#include "assetManager.h"
#include "sheetManagerAssetManager.h"
#include "sheetManagerAssets.h"
#include "ui_sheetmanager.h"

#define SHEET_MANAGER_RESERVED_TILE_IMAGE ":/sheetManager/images/reservedTile.png"

class QTabBar;
class QMenu;
class QActionGroup;
class QToolButton;
class SheetManagerFileDockWidget;
class TileDockWidget;
class HistoryDockWidget;
class QGraphicsScene;
class TerrainImage;
class QIcon;
class SheetManagerTileSelection;
class SourceView;
class QFileSystemWatcher;

namespace Ui {
    class SheetManager;
}

struct SourceSheetEntry {
	~SourceSheetEntry(void) { delete icon; }
	QString filename;
	unsigned width, height;
	unsigned tileWidth, tileHeight;
	QIcon *icon;
	QVector<QPixmap> imgs;
	QGraphicsScene scene;
};

class SheetManager : public QMainWindow, public AssetManager, public SheetManagerAssetManager {
    Q_OBJECT
public:

	explicit SheetManager(QWidget *parent=0);
	virtual ~SheetManager(void);

	void swapSheets(void);
	void initialize(const bool gl, const bool autoScroll);

	//Returns false if CANCELED. Returns true if saved or discarded.
	bool saveOrDiscard(bool canCancel=true);
	//Useful helper function. This should really be moved to some utility Pixmap/Tile class eventually...
	static QPixmap reorientPixmap(const QPixmap &pixmap, const GY_RECT_TEX_ORIENT orient);

protected:
	//virtual void closeEvent(QCloseEvent *const event);
	virtual void closeEvent(QCloseEvent *event);
/*Q_SIGNALS:
	void activeLayerChanged(const int index);*/

private Q_SLOTS:
	bool _openSourceSheet(const QString filename);

	void changeActiveLevel(QAction *action);
	void onLevelAdded(Project * const project, Level * const level);
	void onLevelRemoved(Project * const project, Level * const level);
	void onLevelRenamed(const QString oldName, const QString newName);
	void onSourceOtherTabChanged(const int index);
	void onSourceOtherTabClose(const int index);
	void onSelectionChanged(SheetManagerTileSelection *const sel);
    void onFileChanged(const QString& path);
	void focusDestView(void);
    void onUndo_triggered();
    void onRedo_triggered();

	//QT Designer
	void on_actionCut_triggered(void);
	void on_actionPaste_triggered(void);
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionEnable_OpenGL_triggered();
	void on_actionToggle_AutoScroll_triggered();
	void on_actionDefault_Transform_triggered();
	void on_actionReset_Layout_triggered();

private:

	SheetManagerAssets _assets;

	Ui::SheetManager *_ui;

	//Dock widgets
	SheetManagerFileDockWidget *_fileDockWidget;
    HistoryDockWidget *_historyDockWidget;

	//Source widgets
	QTabBar *_srcLevelTabBar;
	QTabBar *_srcOtherTabBar;
	QMenu *_srcLevelMenu;
	QActionGroup *_srcLevelGroup;
	QToolButton *_srcLevelDropdown;
	QVector<QAction *> _srcLevelActions;

	//Destination widgets
	QMenu *_destLevelMenu;
	QActionGroup *_destLevelGroup;
	QToolButton *_destLevelDropdown;
	QVector<QAction *> _destLevelActions;

	//Source sheet storage
	//SourceSheetEntry *_levSheet; //static entry for level tile/objectsheets
	QMap<QString, SourceSheetEntry *> _srcSheets; //entries for other images
	QVector<QString> _srcSheetTabLookup;

	//Dest sheet storage
	QGraphicsScene *_destSheetScene;
	QGraphicsRectItem *_destSceneBgRect;

    //Monitor open files for modification
    QFileSystemWatcher *_watcher;

    bool _ignoreSheetTabChange;

	static SourceSheetEntry *_createSourceSheetEntry(const QString filename, const SourceView *const view);
	bool _closeSourceSheet(const QString);
	void _destViewForceUpdate(void);
	bool _save(void);

};

#endif // SHEETMANAGER_H
