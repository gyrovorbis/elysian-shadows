#ifndef TILE_SELECTABLE_H
#define TILE_SELECTABLE_H

#include <QStyledItemDelegate>
#include "tileSelection.h"
#include "elysian_tile.hpp"

class TileSelectable;
class TileSelectableSelectionViewDelegate;

class TileSelectable:
        public TileSelection<TileEntry>,
        public Selectable,
        public PropertyStackModel
{
private:

	Assets::LAYER_TYPE _layer;
	Level *const _level;
	Area *const _area;

	//Shared delegate instance for displaying all TileSelectables
	static TileSelectableSelectionViewDelegate *_selectionViewDelegate;

public:

	//Main constructor--based on QGraphicsItems that have been selected within the scene
	TileSelectable(const QList<QGraphicsItem *> &items, Area *const area, Level *const lev, const Assets::LAYER_TYPE layer);
	//Copy constructor
	TileSelectable(const TileSelectable &rval);
	//MEGA reesty constructor to optimize creation from MapView
	TileSelectable(const QRect &region, bool selectionArray[][ELYSIAN_AREA_MAP_WIDTH], Area *const area, Level *const lev, const Assets::LAYER_TYPE layer);

    virtual void populateModel(void) override;
    virtual void installSelectionViewCustomDelegates(QAbstractItemView *const view) override;
	Assets::LAYER_TYPE getTileLayer(void) const { return _layer; }
    virtual Selectable::TYPE getType(void) const override { return Selectable::TILE; }

	//Elevation
	void elevateUp(void);
	void elevateDown(void);
	void elevateFromBase(void);
	void toggleZTransitional(void);
};

#endif // TILESELECTABLE_H
