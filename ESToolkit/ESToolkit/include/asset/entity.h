#ifndef ENTITY_H
#define ENTITY_H

#include <QGraphicsRectItem>
#include "treenode.h"
#include "assetManager.h"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "rowsJoinerProxyModel.h"

class Sprite;
class Component;
class Renderable;
class Collidable;
class Animation;
class Warp;
class Item;
class Character;
class Friendly;
class Enemy;
class TkEntity;
class LuaTableStackModel;
typedef TkEntity Entity;

class TkEntity:
		public RowsJoinerProxy,
	/*	public QWidget, */
		public TreeNode,
		public QGraphicsRectItem,
		public elysian::EntityBase
{
    friend class ESToolkit;
    Q_OBJECT

protected:
	LuaTableStackModel*			_entityModel	= nullptr;
	int							_luaRef			= LUA_NOREF;

private:

    /* COMPONENTS */
/*	Renderable*		_renderable;
	Animation *		_animation;
	Collidable*		_collidable;
	Warp*			_warp;
	Item*			_item;
	Character*		_character;
	Friendly*		_friendly;
	Enemy*			_enemy;
*/
    //QList<Behavior *> _behavior;

    /*  WIDGETS for BASE ENTITY attributes:
      name, transform
      PROBABLY want to redo this in designer?
    */
	//QToolBox *_compToolBox;
    QWidget *_entityTab;
	//QVBoxLayout *_mainLayout, *_entityBaseLayout;
	//QGridLayout *_entityLayout;
	//QLabel *_nameLabel, *_transLabel, *_posLabel, *_sizeLabel, *_scaleLabel, *_orientLabel;
	//QLabel *_xLabel, *_yLabel, *_zLabel;
    QLineEdit *_nameEdit, *_posXEdit, *_posYEdit, *_posZEdit, *_sizeXEdit, *_sizeYEdit,
        *_scaleXEdit, *_scaleYEdit, *_orientZEdit;
	//QCheckBox *_snapsToGrid;

    bool _gridSnap;
    //void InitializeUI(void);
    //void PopulateWidgets(void);

public slots:

public:

    using elysian::EntityBase::render;
    using elysian::EntityBase::move;
    using elysian::EntityBase::setName;

			TkEntity(void);
	virtual ~TkEntity(void) override;

    void setName(const QString name);
	void populateModel(void);

	//Selectable
	virtual Selectable::TYPE getType(void) const { return Selectable::ENTITY; }
	virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_ENTITY; }

	//Should "technically" be decoupled. See .cpp for comment.
    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual bool activate(void) override { return elysian::EntityBase::activate(); }
    virtual bool deactivate(void) override { return elysian::EntityBase::deactivate(); }

    elysian::Entity* getElysianEntity(void);
#if 0
    Renderable *getRenderable(void) const;
	Animation *getAnimation(void) const;
	Collidable *getCollidable(void) const;
	Warp *getWarp(void) const;
	Item *getItem(void) const;
	Character *getCharacter(void) const;
	Friendly *getFriendly(void) const;
	Enemy *getEnemy(void) const;
#endif

signals:
    void ComponentAttached(Entity *const entity, Component *const comp);
    void ComponentRemoved(Entity *const entity, Component *const comp);

};

inline elysian::Entity* Entity::getElysianEntity(void) { return this; }

inline void Entity::setName(const QString name) {
    ((elysian::EntityBase*)this)->setName(name.toStdString().c_str());
    elysian::EntityBase::setName(name.toStdString().c_str());
    setNodeName(name);
}


#endif // ENTITY_H
