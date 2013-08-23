#ifndef UNDO_ITEM_H
#define UNDO_ITEM_H

#include <QString>
#include <QUndoCommand>

class UndoItem: public QUndoCommand {
public:
	//GLOBAL LIST OF ALL UNDO TYPES IN THE TOOLKIT!
	enum TYPE {
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
