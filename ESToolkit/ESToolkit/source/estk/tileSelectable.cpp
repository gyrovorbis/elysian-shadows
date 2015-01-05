#include <QComboBox>
#include <QMetaEnum>

#include "tileSelectable.h"
#include "tileSelectableSelectionViewDelegate.h"
#include "tkArea.h"
#include "elysian_level.hpp"
#include "tileImage.h"
#include "propertyStandardItem.h"

// ===== STATIC MEMBERS =====

TileSelectableSelectionViewDelegate* TileSelectable::_selectionViewDelegate = nullptr;

// ===== INSTANCE MEMBERS =====

QIcon TileSelectable::updateColliderIcon(const QString string) const {
    return ElysianTileWrapper::getColliderIconFromString(string);
}


void TileSelectable::_populateTileIndicesList(void) {
    for(unsigned y = 0; y < _height; ++y) {
        for(unsigned x = 0; x < _width; ++x) {
            bool alreadyUsed = false;

            //Compare it against running list to prevent tile duplicates
            for(const auto& it : _tileIndices) {
                if(it == _entry[y][x].getIndex()) {
                    alreadyUsed = true;
                    break;
                }
            }
            if(alreadyUsed) continue; //Don't readd duplicates
            _tileIndices.push_back(_entry[y][x].getIndex());
        }
    }


    _groupColGet = [&](uint16_t index) {
        return _level->getElysianTileWrapper(index)->getColliderString();
    };
    _groupColSet = [&](uint16_t index, QString str) {
        _level->getElysianTileWrapper(index)->setColliderString(str);
    };

    _groupStandOnGet = [&](uint16_t index) {
        return _level->getElysianTile(index)->getStandOn();
    };
    _groupStandOnSet = [&](uint16_t index, bool val) {
        _level->getElysianTile(index)->setStandOn(val);
    };

    _groupTransGet = [&](uint16_t index) {
        return _level->getElysianTile(index)->getTransparency();
    };
    _groupTransSet = [&](uint16_t index, bool val) {
        _level->getElysianTile(index)->setTransparency(val);
    };

    _groupNormalGet = [&](uint16_t index) {
        return _level->getElysianTileWrapper(index)->getNormalMapEnabled();
    };
    _groupNormalSet = [&](uint16_t index, bool val) {
        _level->getElysianTileWrapper(index)->setNormalMapEnabled(val);
    };

    _groupSpecularGet = [&](uint16_t index) {
        return _level->getElysianTileWrapper(index)->getSpecularMapEnabled();
    };
    _groupSpecularSet = [&](uint16_t index, bool val) {
        _level->getElysianTileWrapper(index)->setSpecularMapEnabled(val);
    };

}

TileSelectable::TileSelectable(const QList<QGraphicsItem *> &items, Area *const area, Level *const lev, const Assets::LAYER_TYPE layer):
	TileSelection<TileEntry>::TileSelection(items),
	_layer(layer),
	_level(lev),
	_area(area)
{
    //Q_ASSERT(0); //shouldn't be using this constructor
	if(!_selectionViewDelegate) _selectionViewDelegate = new TileSelectableSelectionViewDelegate;
    _populateTileIndicesList();
    //updateSceneSelection();
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
    _populateTileIndicesList();
	//updateSceneSelection(region.topLeft().x(), region.topLeft().y());
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

	clearStack();
	insertHeaders();

    //Add group attributes if we're > 1
    if(_tileIndices.size() > 1) {
        QStandardItem *item = new QStandardItem;
        item->setData(_tileIndices.size(), Qt::DisplayRole);
        item->setFlags(Qt::ItemIsEnabled);
        pushLabel("Tile Group", item);

        item = _groupCol = new PropertyStandardItem<QString, TileSelectable>(this,
                                                                        &TileSelectable::getColliderString,
                                                                        &TileSelectable::setColliderString,
                                                                        UndoItem::TILE_GROUP_COLLIDER,
                                                                        nullptr,
                                                                        &TileSelectable::updateColliderIcon);

        item->setIcon(ElysianTileWrapper::getColliderIconFromString(getColliderString()));
        addProperty("collider", item);


        item = _groupStandOn = new PropertyStandardItem<bool, TileSelectable>(this,
                                                                            &TileSelectable::getStandOn,
                                                                            &TileSelectable::setStandOn);
        item->setEditable(false);
        item->setCheckState(getStandOn()? Qt::Checked : Qt::Unchecked);
        item->setCheckable(true);
        static_cast<PropertyStandardItem<bool, TileSelectable>*>(item)->unignoreCheck();
        addProperty("Stand on", item);

        item = _groupTrans = new PropertyStandardItem<bool, TileSelectable>(this,
                                                                            &TileSelectable::getTransparency,
                                                                            &TileSelectable::setTransparency);
        item->setEditable(false);
        item->setCheckState(getTransparency()? Qt::Checked : Qt::Unchecked);
        item->setCheckable(true);
        static_cast<PropertyStandardItem<bool, TileSelectable>*>(item)->unignoreCheck();
        addProperty("Transparency", item);

        item = _groupNormal = new PropertyStandardItem<bool, TileSelectable>(this,
                                                                            &TileSelectable::getNormalMapEnabled,
                                                                            &TileSelectable::setNormalMapEnabled);
        item->setEditable(false);
        item->setCheckState(getNormalMapEnabled()? Qt::Checked : Qt::Unchecked);
        item->setCheckable(true);
        static_cast<PropertyStandardItem<bool, TileSelectable>*>(item)->unignoreCheck();
        addProperty("Normal Map", item);

        item = _groupSpecular = new PropertyStandardItem<bool, TileSelectable>(this,
                                                                            &TileSelectable::getSpecularMapEnabled,
                                                                            &TileSelectable::setSpecularMapEnabled);
        item->setEditable(false);
        item->setCheckState(getSpecularMapEnabled()? Qt::Checked : Qt::Unchecked);
        item->setCheckable(true);
        static_cast<PropertyStandardItem<bool, TileSelectable>*>(item)->unignoreCheck();
        addProperty("Specular Map", item);

    }

    for(const auto it : _tileIndices) {

			//ADD TILE ATTRIBUTES (BOTH TERRAIN AND OBJECTS)
            const unsigned int tileIndex = it;
            elysian::Tile *curTile = &_level->getElysianLevel()->tileset[tileIndex];
            auto*           curTileWrapper = _level->getElysianTileWrapper(tileIndex);
			Q_ASSERT(curTile);
			//Add tile # and icon header
			QStandardItem *item = new QStandardItem;
            item->setData(tileIndex, Qt::DisplayRole);
			item->setFlags(Qt::ItemIsEnabled);
            item->setIcon(_level->getTileImage(tileIndex)->getIcon());
			pushLabel("Tile", item);

				//Add collider type property
                item = new PropertyStandardItem<QString, ElysianTileWrapper, true>(curTileWrapper,
																				   &ElysianTileWrapper::getColliderString,
																				   &ElysianTileWrapper::setColliderString,
																				   UndoItem::TILE_COLLIDER,
																				   &_area->getUndoStack(),
																				   &ElysianTileWrapper::updateColliderIcon);
				item->setIcon(ElysianTileWrapper::getColliderIconFromEnum((ElysianTileWrapper::COLLIDER_TYPE)curTile->getColliderType()));
                if(tileIndex == 0) item->setEditable(false);
                if(_groupCol) _groupCol->addChildItem(static_cast<ESTkStandardItem*>(item));
                addProperty("collider", item);

				//addProperty("col int", new PropertyStandardItem<unsigned, elysian::Tile>(curTile, &elysian::Tile::getColliderType, &elysian::Tile::setColliderType));

                item = new PropertyStandardItem<bool, elysian::Tile, true>(curTile,
                                                                                &elysian::Tile::getAnimated,
                                                                                &elysian::Tile::setAnimated,
																				UndoItem::TILE_ANIMATED,
																				&_area->getUndoStack());
                item->setEditable(false);
                item->setCheckState(curTile->getAnimated()? Qt::Checked : Qt::Unchecked);
                if(tileIndex != 0) {
                    item->setCheckable(true);
                    static_cast<PropertyStandardItem<bool, elysian::Tile, true>*>(item)->unignoreCheck();
                }

				pushLabel("Animation", item);

                    item = new PropertyStandardItem<uint16_t, elysian::Tile, true>(curTile,
                                                                                        &elysian::Tile::getNextIndex,
                                                                                        &elysian::Tile::setNextIndex,
																						UndoItem::TILE_ANIM_NEXT_INDEX,
																						&_area->getUndoStack());
                    if(tileIndex == 0) item->setEditable(false);
					addProperty("next index", item);

                    item = new PropertyStandardItem<float, elysian::Tile, true>(curTile,
                                                                                        &elysian::Tile::getAnimTime,
                                                                                        &elysian::Tile::setAnimTime,
																						UndoItem::TILE_ANIM_TIME,
																						&_area->getUndoStack());
                    if(tileIndex == 0) item->setEditable(false);
					addProperty("time", item);

				popLabel();

                item = new PropertyStandardItem<bool, elysian::Tile, true>(curTile,
                                                                                    &elysian::Tile::getStandOn,
                                                                                    &elysian::Tile::setStandOn,
                                                                                    UndoItem::TILE_STAND_ON,
                                                                                    &_area->getUndoStack());
                item->setEditable(false);
                item->setCheckState(curTile->getStandOn()? Qt::Checked : Qt::Unchecked);
                if(tileIndex != 0) {
                    item->setCheckable(true);
                    static_cast<PropertyStandardItem<bool, elysian::Tile, true>*>(item)->unignoreCheck();
                }
                if(_groupStandOn) _groupStandOn->addChildItem(static_cast<ESTkStandardItem*>(item));
                addProperty("Stand on", item);

                item = new PropertyStandardItem<bool, elysian::Tile, true>(curTile,
                                                                                    &elysian::Tile::getTransparency,
                                                                                    &elysian::Tile::setTransparency,
                                                                                    UndoItem::TILE_TRANSPARENCY,
                                                                                    &_area->getUndoStack());
                item->setEditable(false);
                item->setCheckState(curTile->getTransparency()? Qt::Checked : Qt::Unchecked);
                if(tileIndex != 0) {
                    item->setCheckable(true);
                    static_cast<PropertyStandardItem<bool, elysian::Tile, true>*>(item)->unignoreCheck();
                }
                if(_groupTrans) _groupTrans->addChildItem(static_cast<ESTkStandardItem*>(item));
                addProperty("Transparency", item);


                item = new PropertyStandardItem<bool, ElysianTileWrapper, true>(curTileWrapper,
                                                                                    &ElysianTileWrapper::getNormalMapEnabled,
                                                                                    &ElysianTileWrapper::setNormalMapEnabled,
                                                                                    UndoItem::TILE_NORMAL_MAP,
                                                                                    &_area->getUndoStack());
                item->setEditable(false);
                item->setCheckState(curTileWrapper->getNormalMapEnabled()? Qt::Checked : Qt::Unchecked);
                if(tileIndex != 0) {
                    item->setCheckable(true);
                    static_cast<PropertyStandardItem<bool, ElysianTileWrapper, true>*>(item)->unignoreCheck();
                }
                if(_groupNormal) _groupNormal->addChildItem(static_cast<ESTkStandardItem*>(item));
                addProperty("Normal Map", item);

                item = new PropertyStandardItem<bool, ElysianTileWrapper, true>(curTileWrapper,
                                                                                    &ElysianTileWrapper::getSpecularMapEnabled,
                                                                                    &ElysianTileWrapper::setSpecularMapEnabled,
                                                                                    UndoItem::TILE_SPECULAR_MAP,
                                                                                    &_area->getUndoStack());
                item->setEditable(false);
                item->setCheckState(curTileWrapper->getSpecularMapEnabled()? Qt::Checked : Qt::Unchecked);
                if(tileIndex != 0) {
                    item->setCheckable(true);
                    static_cast<PropertyStandardItem<bool, ElysianTileWrapper, true>*>(item)->unignoreCheck();
                }
                if(_groupSpecular) _groupSpecular->addChildItem(static_cast<ESTkStandardItem*>(item));
                addProperty("Specular Map", item);

			popLabel();
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
            if(++elev > 31) elev = 31; //Cap the max elevation
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
            _entry[y][x].setElev(_height-1-y);
            _entry[y][x].setPlane(GY_RECT_PLANE_FRONT);
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

void TileSelectable::toggleNormalFace(void) {
    for(unsigned y = 0; y < _height; ++y) {
        for(unsigned x = 0; x < _width; ++x) {
            if(!_entry[y][x].value) continue; //Do NOT give attributes to tile 0!
            _entry[y][x].setPlane(_planeType);
        }
    }

    _planeType = (GY_RECT_PLANE_TYPE)((int)(_planeType+1));
    if(_planeType == GY_RECT_PLANE_MAX) _planeType = GY_RECT_PLANE_BOTTOM;
}


QString TileSelectable::getColliderString(void) const {
    return _getTileGroupProperty<QString>(_groupColGet, "COLLIDER_NONE");
}

void TileSelectable::setColliderString(const QString string) {
    _area->getUndoStack().push(new TileSelectableUndoItem<QString>(_tileIndices,
                                                          string,
                                                          _groupColGet,
                                                          _groupColSet,
                                                          _groupCol,
                                                          UndoItem::TILE_GROUP_COLLIDER));
}

bool TileSelectable::getStandOn(void) const {
    return _getTileGroupProperty<bool>(_groupStandOnGet, false);
}

void TileSelectable::setStandOn(const bool val) {
    _area->getUndoStack().push(new TileSelectableUndoItem<bool>(_tileIndices,
                                                          val,
                                                          _groupStandOnGet,
                                                          _groupStandOnSet,
                                                          _groupStandOn,
                                                          UndoItem::TILE_GROUP_STAND_ON));
}

bool TileSelectable::getTransparency(void) const {
    return _getTileGroupProperty<bool>(_groupTransGet, false);
}

void TileSelectable::setTransparency(const bool val) {
    _area->getUndoStack().push(new TileSelectableUndoItem<bool>(_tileIndices,
                                                          val,
                                                          _groupTransGet,
                                                          _groupTransSet,
                                                          _groupTrans,
                                                          UndoItem::TILE_GROUP_TRANSPARENCY));
}

bool TileSelectable::getNormalMapEnabled(void) const {
    return _getTileGroupProperty<bool>(_groupNormalGet, false);
}

void TileSelectable::setNormalMapEnabled(const bool val) {
    _area->getUndoStack().push(new TileSelectableUndoItem<bool>(_tileIndices,
                                                          val,
                                                          _groupNormalGet,
                                                          _groupNormalSet,
                                                          _groupNormal,
                                                          UndoItem::TILE_GROUP_NORMAL_MAP));
}

bool TileSelectable::getSpecularMapEnabled(void) const {
    return _getTileGroupProperty<bool>(_groupSpecularGet, false);
}

void TileSelectable::setSpecularMapEnabled(const bool val) {
    _area->getUndoStack().push(new TileSelectableUndoItem<bool>(_tileIndices,
                                                          val,
                                                          _groupSpecularGet,
                                                          _groupSpecularSet,
                                                          _groupSpecular,
                                                          UndoItem::TILE_GROUP_SPECULAR_MAP));
}
/*
void TileSelectable::updateSceneSelection(const unsigned sceneX, const unsigned sceneY) {
//	return;
    if(sceneX == _sceneX && sceneY == _sceneY) return;

#if 0
    //Refresh old selection region to clear selection from VBO
    if(_prevSceneX != -1 && _prevSceneY != -1)
    for(unsigned y = prevSceneStartY; y <= prevSceneEndY; ++y) {
        for(unsigned x = prevSceneStartX; x <= prevSceneEndX; ++x) {
            //Don't bother if tile is part of new selection
            if(x >= sceneX && x <= sceneEndX && y >= sceneY && y <= sceneEndY)
                continue;
            _area->setTileEntry(_layer, x, y, _area->getTileEntry(_layer,w x, y));
        }
    }


    for(unsigned y = 0; y < _height; ++y) {
        for(unsigned x = 0; x < _width; ++x) {
            if(_inSelection[y][x]) {
                _area->setVboEntry(&_entry[y][x], _layer, x+sceneX, y+sceneY);
            }
        }
    }
#endif

   clearSceneSelection();

   _sceneX             = (sceneX+16)/32;
   _sceneY             = (sceneY+16)/32;

    for(unsigned y = 0; y < _height; ++y) {
        for(unsigned x = 0; x < _width; ++x) {
            if(_inSelection[y][x]) {
                if(_area->isValidTileEntry(x+_sceneX, y+_sceneY)) {
                    _prevEntry[y][x] = _area->getTileEntry(_layer, x+_sceneX, y+_sceneY);
                    _area->setTileEntry(_layer, x+_sceneX, y+_sceneY, _entry[y][x]);
                }
            }
        }
    }
}

void TileSelectable::clearSceneSelection(void) {
    _prevSceneX = _sceneX;
    _prevSceneY = _sceneY;
    _sceneX = -1;
    _sceneY = -1;
    if(_prevSceneX != -1 && _prevSceneY != -1) {
        for(unsigned y = 0; y < _height; ++y) {
            for(unsigned x = 0; x < _width; ++x) {
                 if(_area->isValidTileEntry(x+_prevSceneX, y+_prevSceneY))
                if(_inSelection[y][x]) {
                    _area->setTileEntry(_layer, x+_prevSceneX, y+_prevSceneY, _prevEntry[y][x]);
                }
            }
        }
    }
}


void TileSelectable::clearPrevSceneSelection(void) {
    for(unsigned y = 0; y < _height; ++y) {
        for(unsigned x = 0; x < _width; ++x) {
            if(_inSelection[y][x]) {
                _prevEntry[y][x] = _entry[y][x];
            }
        }
    }
}
*/

