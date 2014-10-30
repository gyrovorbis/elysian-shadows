#ifndef SELECTABLE_H
#define SELECTABLE_H

//#include "property_stack_model.h"
class QAbstractItemView;

namespace elysian {
    struct TileEntry;
}

class TkEntity;
typedef TkEntity Entity;
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
	enum TYPE { EMPTY, PROJECT, LEVEL, AREA, TILE, ENTITY };

	union Union {
		Selectable *base;
		TileSelection<elysian::TileEntry> *terrain;
		Entity *entity;
		Area *area;
		Level *level;
		Project *project;
	};

	Selectable(void);
	virtual ~Selectable(void) {}
	virtual TYPE getType(void) const=0;
	virtual void installSelectionViewCustomDelegates(QAbstractItemView *const /*view*/) {}
};




#endif // SELECTABLE_H
