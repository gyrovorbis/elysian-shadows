#ifndef ENTITY_SELECTABLE_H
#define ENTITY_SELECTABLE_H

#include <tk_selectable.h>
#include <tk_rows_joiner_proxy_model.h>
#include <tk_asset_manager.h>
#include <estk/tk_lua_manager.h>
#include <QVector>

class TkEntity;
class TkArea;
class LuaTableStackModel;

class EntitySelectable: public Selectable, public TkLuaManager, public AssetManager {
public:
    struct EntityEntry {
                    EntityEntry(void)       = default;
                    EntityEntry(TkEntity*const ent);
        TkEntity*   operator->(void);
        TkEntity*   operator->(void) const;
                    operator TkEntity*(void) const;
                    operator const TkEntity*(void) const;
        friend bool operator==(EntityEntry entry, TkEntity* ent);


        TkEntity*   entity      = nullptr;
        bool        wasActive   = false;
    };

private:

    QVector<EntityEntry>            _entities;
    TkEntity*                       _baseEntity     = nullptr;
    mutable LuaTableStackModel*     _luaModel       = nullptr;
    int                             _luaTableRef    = LUA_REFNIL;


public:
                                EntitySelectable(void);
                                EntitySelectable(TkEntity*const entity);
                                EntitySelectable(EntitySelectable* selectable);
    virtual                     ~EntitySelectable(void) override;

    void                        addEntity(const EntityEntry entry);
    void                        setBaseEntity(TkEntity*const entity);
    TkEntity*                   getBaseEntity(void) const;
    void                        removeEntities(QVector<TkEntity*>& entities);
    void                        removeEntity(TkEntity*const entity);
    virtual TYPE                getType(void) const override;
    //const QVector<TkEntity*>&   getEntities(void) const;
    bool                        isInSelection(const TkEntity*const entity) const;
    int                         getLuaTableRef(void) const;

    void                        translate(const float x, const float y);
    void                        setPosition(const float x, const float y);

    void                        deactivate(void);
    void                        activate(TkArea* area);

    virtual void                setCurrentSelection(const bool val, const Selectable*const other) override;
    virtual QAbstractItemModel* getModel(void) const override;

    QVector<EntityEntry>::iterator begin(void);
    QVector<EntityEntry>::iterator end(void);

};




//======== INLINEZ ========

inline void EntitySelectable::removeEntities(QVector<TkEntity *> &entities) {
    for(auto it = entities.begin(); it != entities.end(); ++it) {
        if(*it == _baseEntity) _baseEntity = nullptr;
        removeEntity(*it);
    }
}

inline TkEntity* EntitySelectable::getBaseEntity(void) const {
    if(_baseEntity) return _baseEntity;
    else {
        return _entities.size()? _entities[0] : nullptr;
    }
}

inline void EntitySelectable::setBaseEntity(TkEntity*const entity) {
    _baseEntity = entity;
}

inline bool EntitySelectable::isInSelection(const TkEntity*const entity) const {
    for(auto it: _entities) {
        if(it == entity) return true;
    }
    return false;
}

inline EntitySelectable::EntityEntry::EntityEntry(TkEntity*const ent): entity(ent) {}

inline TkEntity* EntitySelectable::EntityEntry::operator->(void) {
    return entity;
}

inline TkEntity* EntitySelectable::EntityEntry::operator->(void) const {
    return entity;
}

inline bool operator==(EntitySelectable::EntityEntry entry, TkEntity* ent) {
    return entry.entity == ent;
}

inline EntitySelectable::EntityEntry::operator TkEntity*(void) const { return entity; }
inline EntitySelectable::EntityEntry::operator const TkEntity*(void) const { return const_cast<const TkEntity*>(entity); }

inline QVector<EntitySelectable::EntityEntry>::iterator EntitySelectable::begin(void) { return _entities.begin(); }
inline QVector<EntitySelectable::EntityEntry>::iterator EntitySelectable::end(void) { return _entities.end(); }

inline int EntitySelectable::getLuaTableRef(void) const { return _luaTableRef; }

#endif // ENTITY_SELECTABLE_H
