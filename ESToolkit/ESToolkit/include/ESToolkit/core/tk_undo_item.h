#ifndef UNDO_ITEM_H
#define UNDO_ITEM_H

#include <QString>
#include <QUndoCommand>

class UndoItem: public QUndoCommand {
public:
	//GLOBAL LIST OF ALL UNDO TYPES IN THE TOOLKIT!
	enum TYPE {
        UNDO_NONE,
		PLACE_TILE,
		PLACE_TILE_SM,
		AREA_RENAME,
		AREA_STRING,
		AREA_WIDTH,
		AREA_HEIGHT,
		TILE_COLLIDER,
		TILE_ANIMATED,
		TILE_ANIM_NEXT_INDEX,
		TILE_ANIM_TIME,
        TILE_STAND_ON,
        TILE_TRANSPARENCY,
        TILE_NORMAL_MAP,
        TILE_SPECULAR_MAP,
        TILE_GROUP_COLLIDER,
        TILE_GROUP_STAND_ON,
        TILE_GROUP_TRANSPARENCY,
        TILE_GROUP_NORMAL_MAP,
        TILE_GROUP_SPECULAR_MAP,
        ENTITY_DELETE,
        LUA_TABLE_FIELD_ITEM,
		TYPE_SIZE
	};
	//EVERY NEW TYPE MUST ALSO HAVE A CORRESPONDING QSTRING NAME!
	static const QString text[TYPE_SIZE];

	UndoItem(const TYPE type): _id(type) {
		setText(text[type]);
	}

	virtual int id(void) const {
		return (int)_id;
	}

private:
	const TYPE _id;

};


#endif // UNDOITEM_H
