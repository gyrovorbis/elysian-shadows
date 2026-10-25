/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef SELECTABLE_H
#define SELECTABLE_H

//include <crtp/elysian_lua_variant.hpp>

class QAbstractItemView;
class QAbstractItemModel;

namespace elysian {
    struct TileEntry;
}

class EntitySelectable;
class LuaTableSelectable;
//class TkEntity;
//typedef TkEntity Entity;
template<typename T>
class TileSelection;
class Project;
class TkLevel;
typedef TkLevel Level;
class TkArea;
typedef TkArea Area;

/*
  Abstract base class for Selectables.
  Anything that is selectable must also implement a
  QAbstractItemModel for the Selection View Widget's
  TreeView.
*/

/*
  NEEDS TO INHEIRIT FROM PROPERTYSTACKVIEW, BUT CANNOT
  DO SO UNTIL ENTITY STOPS BEING A FUCKING WIDGET, BECAUSE
  OF QOBJECT DUPLICATION!
 */

class Selectable/*: public PropertyStackModel*/ {
public:
    enum TYPE { EMPTY, PROJECT, LEVEL, AREA, TILE, ENTITY, LUA_TABLE };

	union Union {
        TileSelection<elysian::TileEntry>*  terrain;
        Selectable*                         base;
        LuaTableSelectable*                 luaTable;
        EntitySelectable*                   entity;
        Area*                               area;
        Level*                              level;
        Project*                            project;
	};

                                Selectable(void) = default;
    virtual                     ~Selectable(void) = default;
    virtual TYPE                getType(void) const=0;
    virtual void                installSelectionViewCustomDelegates(QAbstractItemView *const /*view*/);
    virtual void                setCurrentSelection(const bool /*val*/, const Selectable*const other=nullptr);
    virtual bool                isTemporary(void) const;
    virtual QAbstractItemModel* getModel(void) const = 0;
   // virtual LuaVariant          getLuaRef(void) const { return LuaVariant(); }
};








//===== INLINEZ ========

inline bool Selectable::isTemporary(void) const { return false; }
inline void Selectable::setCurrentSelection(const bool, const Selectable * const) {}
inline void Selectable::installSelectionViewCustomDelegates(QAbstractItemView * const) { }



#endif // SELECTABLE_H
