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

namespace elysian {
    class CollidableAttrib;
    class SpriteAttrib;
    class Material;
}

class TkEntity:
		public RowsJoinerProxy,
	/*	public QWidget, */
		public TreeNode,
		public QGraphicsRectItem,
		public elysian::EntityBase
{
    friend class ESToolkit;
    friend class elysian::EntityBase;
    Q_OBJECT

public:
    enum SELECTION_STATE { SELECTION_NONE, SELECTION_SELECTING, SELECTION_SELECTED };

protected:
	LuaTableStackModel*			_entityModel	= nullptr;
	int							_luaRef			= LUA_NOREF;

    static bool                         _staticInit;
    static elysian::CollidableAttrib*   _selColliderAttrib;

    bool                                _selectable;
    elysian::SpriteAttrib*              _selSpriteAttrib;
    elysian::Collidable*                _selectionCollider;
    elysian::Renderable*                _selectionRenderable;
    SELECTION_STATE                     _selectionState = SELECTION_NONE;

    void _renderRelative(void);

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
    using elysian::EntityBase::setTransform;
    //using elysian::LuaTableProxy<elysian::EntityBase, Entity>::fromTable;

            TkEntity();
            TkEntity(const bool selectable);
	virtual ~TkEntity(void) override;

    void setName(const QString name);
    void setName(char const*const name);
    void setSelectionState(const SELECTION_STATE state);
    SELECTION_STATE getSelectionState(void) const;
	void populateModel(void);

	//Selectable
	virtual Selectable::TYPE	getType(void) const { return Selectable::ENTITY; }
	virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_ENTITY; }

	//Should "technically" be decoupled. See .cpp for comment.
    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual bool activate(void) override;
    virtual bool deactivate(void) override;

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

inline elysian::Entity* TkEntity::getElysianEntity(void) { return this; }

inline void TkEntity::setName(char const * const name) {
    setName(QString(name));
}

inline void TkEntity::setName(const QString name) {
    ((elysian::EntityBase*)this)->setName(name.toStdString().c_str());
    elysian::EntityBase::setName(name.toStdString().c_str());
    setNodeName(name);
}

inline auto TkEntity::getSelectionState(void) const -> SELECTION_STATE { return _selectionState; }


#endif // ENTITY_H
