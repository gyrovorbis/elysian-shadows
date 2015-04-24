#ifndef TK_ENTITY_H
#define TK_ENTITY_H

#include <treenode.h>
#include <elysian_entity_base.hpp>
#include <luaTableStackModel.h>
#include <elysian_system.hpp>

class QMenu;
class Sprite;
class TkEntity;
class LuaTableStackModel;

namespace elysian {
    class CollidableAttrib;
    class SpriteAttrib;
    class Material;
}

class TkEntity:
        //public LuaTableStackModel,
        public elysian::EntityBase,
		public TreeNode,
        public AssetManager,
        public elysian::System
{
    friend class ESToolkit;
    friend class elysian::EntityBase;
    Q_OBJECT

public:
    enum SELECTION_STATE { SELECTION_NONE, SELECTION_SELECTING, SELECTION_SELECTED };

protected:

    static bool                         _staticInit;
    static elysian::CollidableAttrib*   _selColliderAttrib;

    mutable LuaTableStackModel*  _luaModel              = nullptr;
    bool                         _selectable;
    elysian::SpriteAttrib*       _selSpriteAttrib;
    elysian::Collidable*         _selectionCollider;
    elysian::Renderable*         _selectionRenderable;
    SELECTION_STATE              _selectionState        = SELECTION_NONE;
    bool                         _saveFlag              = false;

    void                         _renderRelative(void);

public:

    using                       elysian::EntityBase::render;
    using                       elysian::EntityBase::move;
    using                       elysian::EntityBase::setName;
    using                       elysian::EntityBase::setTransform;

                                TkEntity(void);
                                TkEntity(const bool selectable);
                                TkEntity(const OOLUA::Lua_table_ref valTable);
    virtual                     ~TkEntity(void) override;

    void                        setName(const QString name);
    void                        setName(char const*const name);
    void                        setSelectionState(const SELECTION_STATE state);
    SELECTION_STATE             getSelectionState(void) const;
    void                        populateModel(void);
    QMenu*                      createContextMenu(void);
    bool                        validate(QString& str) const;
    bool                        getSaveFlag(void) const;
    void                        setSaveFlag(const bool val);

	//Selectable
    virtual auto                getType(void) const -> TYPE override;
    virtual auto                getTreeWidgetItemType(void) -> ITEM_TYPE override;
    virtual LuaTableStackModel* getModel(void) const override;

    //elysian::EntityBase
    virtual bool                activate(void) override;
    virtual bool                deactivate(void) override;

    //QTreeWidgetItem
    virtual void                setData(int column, int role, const QVariant &value) override;

public Q_SLOTS:
    void                        onEntityParented(Entity*const child, Entity*const parent);

Q_SIGNALS:

    void                        entityParented(Entity*const child, Entity*const parent);

};














// ======== INLINEZ =======





inline void TkEntity::setName(char const * const name) {
    setName(QString(name));
}

inline void TkEntity::setName(const QString name) {
    ((elysian::EntityBase*)this)->setName(name.toStdString().c_str());
    elysian::EntityBase::setName(name.toStdString().c_str());
    setNodeName(name);
}

inline auto TkEntity::getSelectionState(void) const -> SELECTION_STATE { return _selectionState; }


inline  auto TkEntity::getType(void) const -> TYPE { return Selectable::ENTITY; }
inline  auto TkEntity::getTreeWidgetItemType(void) -> ITEM_TYPE { return ITEM_ENTITY; }


#endif // ENTITY_H
