#include <QComboBox>
#include <QMetaEnum>

#include "tileSelectable.h"
#include "tileSelectableSelectionViewDelegate.h"
#include "area.h"
#include "level.h"
#include "tileImage.h"
#include "propertyStandardItem.h"

// ===== STATIC MEMBERS =====

TileSelectableSelectionViewDelegate *TileSelectable::_selectionViewDelegate = NULL;

// ===== INSTANCE MEMBERS =====

TileSelectable::TileSelectable(const QList<QGraphicsItem *> &items, Area *const area, Level *const lev, const Assets::LAYER_TYPE layer):
	TileSelection<TileEntry>::TileSelection(items),
	_layer(layer),
	_level(lev),
	_area(area)
{
	if(!_selectionViewDelegate) _selectionViewDelegate = new TileSelectableSelectionViewDelegate;
}


TileSelectable::TileSelectable(const QRect &region, bool selectionArray[][ELYSIAN_AREA_MAP_WIDTH], Area *const area, Level *const lev, const Assets::LAYER_TYPE layer):
	_layer(layer),
	_level(lev),
	_area(area)
{
	_width = region.width();
	_height = region.height();

	_allocArrays(_entry, _inSelection, _width, _height);

	for(unsigned j = 0; j < _height; ++j) {
		for(unsigned i = 0; i < _width; ++i) {
			//uint16_t tileValue = area->getTileValue(QPoint(region.topLeft().x()+i, region.topLeft().y()+j), layer);
			_entry[j][i] = area->getTileEntry(QPoint(region.topLeft().x()+i, region.topLeft().y()+j), layer);
			_inSelection[j][i] = selectionArray[region.topLeft().y()+j][region.topLeft().x()+i];
			//_index[j][i] = ELYSIAN_MAP_INDEX_FROM_VALUE(tileValue);
			//_orient[j][i] = ELYSIAN_MAP_ORIENT_FROM_VALUE(tileValue);

		}
	}

	if(!_selectionViewDelegate) _selectionViewDelegate = new TileSelectableSelectionViewDelegate;
}

TileSelectable::TileSelectable(const TileSelectable &rval):
	TileSelection<TileEntry>::TileSelection(rval),
	Selectable(),
	PropertyStackModel(),
	_layer(rval._layer),
	_level(rval._level),
	_area(rval._area)
{}

void TileSelectable::populateModel(void) {
	QList<uint16_t> used;

	clearStack();
	insertHeaders();

	//Iteratore over every selected tile
	for(unsigned y = 0; y < _height; ++y) {
		for(unsigned x = 0; x < _width; ++x) {
			bool alreadyUsed = false;

			//Compare it against running list to prevent tile duplicates
			for(const auto& it : used) {
				if(it == _entry[y][x].getIndex()) {
					alreadyUsed = true;
					break;
				}
			}
			if(alreadyUsed) continue; //Don't readd duplicates
			used.push_back(_entry[y][x].getIndex());

			//ADD TILE ATTRIBUTES (BOTH TERRAIN AND OBJECTS)
			elysian::Tile *curTile = &_level->getElysianLevel()->tileset[_entry[y][x].getIndex()];
			Q_ASSERT(curTile);
			//Add tile # and icon header
			QStandardItem *item = new QStandardItem;
			item->setData(_entry[y][x].getIndex(), Qt::DisplayRole);
			item->setFlags(Qt::ItemIsEnabled);
			item->setIcon(_level->getTileImage(_entry[y][x].getIndex())->getIcon());
			pushLabel("Tile", item);

				//Add collider type property
				item = new PropertyStandardItem<QString, ElysianTileWrapper, true>(static_cast<ElysianTileWrapper *>(curTile),
																				   &ElysianTileWrapper::getColliderString,
																				   &ElysianTileWrapper::setColliderString,
																				   UndoItem::TILE_COLLIDER,
																				   &_area->getUndoStack(),
																				   &ElysianTileWrapper::updateColliderIcon);
				item->setIcon(ElysianTileWrapper::getColliderIconFromEnum((ElysianTileWrapper::COLLIDER_TYPE)curTile->getColliderType()));
                if(_entry[y][x].getIndex() == 0) item->setEditable(false);
                addProperty("collider", item);

				//addProperty("col int", new PropertyStandardItem<unsigned, elysian::Tile>(curTile, &elysian::Tile::getColliderType, &elysian::Tile::setColliderType));

				item = new PropertyStandardItem<bool, ElysianTileWrapper, true>(static_cast<ElysianTileWrapper*>(curTile),
																				&ElysianTileWrapper::getAnimated,
																				&ElysianTileWrapper::setAnimated,
																				UndoItem::TILE_ANIMATED,
																				&_area->getUndoStack());
                item->setEditable(false);
                item->setCheckState(curTile->getAnimated()? Qt::Checked : Qt::Unchecked);
                if(_entry[y][x].getIndex() != 0) {
                    item->setCheckable(true);
                    static_cast<PropertyStandardItem<bool, ElysianTileWrapper, true>*>(item)->unignoreCheck();
                }

				pushLabel("Animation", item);

					item = new PropertyStandardItem<uint16_t, ElysianTileWrapper, true>(static_cast<ElysianTileWrapper*>(curTile),
																						&ElysianTileWrapper::getNextIndex,
																						&ElysianTileWrapper::setNextIndex,
																						UndoItem::TILE_ANIM_NEXT_INDEX,
																						&_area->getUndoStack());
                    if(_entry[y][x].getIndex() == 0) item->setEditable(false);
					addProperty("next index", item);

					item = new PropertyStandardItem<float, ElysianTileWrapper, true>(static_cast<ElysianTileWrapper*>(curTile),
																						&ElysianTileWrapper::getAnimTime,
																						&ElysianTileWrapper::setAnimTime,
																						UndoItem::TILE_ANIM_TIME,
																						&_area->getUndoStack());
                    if(_entry[y][x].getIndex() == 0) item->setEditable(false);
					addProperty("time", item);

				popLabel();
			popLabel();
		}
	}
}

void TileSelectable::installSelectionViewCustomDelegates(QAbstractItemView *const view) {
	view->setItemDelegate(_selectionViewDelegate);
}

void TileSelectable::elevateUp(void) {
	for(unsigned y = 0; y < _height; ++y) {
		for(unsigned x = 0; x < _width; ++x) {
			if(!_entry[y][x].value) continue; //Do NOT give attributes to tile 0!
			int elev = _entry[y][x].getElev();
			if(++elev > 15) elev = 15; //Cap the max elevation
			_entry[y][x].setElev(elev);
		}
	}
}

void TileSelectable::elevateDown(void) {
	for(unsigned y = 0; y < _height; ++y) {
		for(unsigned x = 0; x < _width; ++x) {
			if(!_entry[y][x].value) continue; //Do NOT give attributes to tile 0!
			int elev = _entry[y][x].getElev();
			if(--elev < 0) elev = 0; //Cap the min elevation
			_entry[y][x].setElev(elev);
		}
	}
}

void TileSelectable::elevateFromBase(void) {
	for(unsigned y = 0; y < _height; ++y) {
		for(unsigned x = 0; x < _width; ++x) {
			if(!_entry[y][x].value) continue; //Do NOT give attributes to tile 0!
			_entry[y][x].setElev(_height-y);
		}
	}
}

void TileSelectable::toggleZTransitional(void) {
	for(unsigned y = 0; y < _height; ++y) {
		for(unsigned x = 0; x < _width; ++x) {
			if(!_entry[y][x].value) continue; //Do NOT give attributes to tile 0!
			_entry[y][x].setZTran(!_entry[y][x].getZTran());
		}
	}
}
