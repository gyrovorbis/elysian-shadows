#ifndef TK_ENTITY_H
#define TK_ENTITY_H

#include <treenode.h>
#include <assetManager.h>
#include <elysian_entity_base.hpp>

class Sprite;
class Component;
class Renderable;
class Collidable;
class Warp;
class Item;
class Character;
class Friendly;
class Enemy;
class TkEntity;
class LuaTableStackModel;
ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(Animation)

namespace elysian {
    class CollidableAttrib;
    class SpriteAttrib;
    class Material;
}

class TkEntity:
        public elysian::EntityBase,
        public TreeNode,
        public AssetManager
{
    friend class ESToolkit;
    friend class elysian::EntityBase;
    Q_OBJECT

public:
    enum SELECTION_STATE { SELECTION_NONE, SELECTION_SELECTING, SELECTION_SELECTED };

protected:
    mutable LuaTableStackModel*          _luaModel   = nullptr;

    static bool                         _staticInit;
    static elysian::CollidableAttrib*   _selColliderAttrib;

    bool                                _selectable;
    elysian::SpriteAttrib*              _selSpriteAttrib;
    elysian::Collidable*                _selectionCollider;
    elysian::Renderable*                _selectionRenderable;
    SELECTION_STATE                     _selectionState = SELECTION_NONE;

    void _renderRelative(void);

public Q_SLOTS:
    void onEntityParented(Entity*const child, Entity*const parent);

public:

    using elysian::EntityBase::render;
    using elysian::EntityBase::move;
    using elysian::EntityBase::setName;
    using elysian::EntityBase::setTransform;
    //using elysian::LuaTableProxy<elysian::EntityBase, Entity>::fromTable;

            TkEntity(void);
            TkEntity(const bool selectable);
            TkEntity(const OOLUA::Lua_table_ref valTable);
    virtual ~TkEntity(void) override;

    void setName(const QString name);
    void setName(char const*const name);
    void setSelectionState(const SELECTION_STATE state);
    SELECTION_STATE getSelectionState(void) const;
    void populateModel(void);

    //Selectable
    virtual Selectable::TYPE	getType(void) const { return Selectable::ENTITY; }
    virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_ENTITY; }
    virtual QAbstractItemModel* getModel(void) const override;

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

Q_SIGNALS:

    void entityParented(Entity*const child, Entity*const parent);

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
