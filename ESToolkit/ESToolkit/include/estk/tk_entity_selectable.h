#ifndef ENTITY_SELECTABLE_H
#define ENTITY_SELECTABLE_H

#include <tk_selectable.h>
#include <tk_rows_joiner_proxy_model.h>
#include <tk_asset_manager.h>
#include <QVector>

class TkEntity;

class EntitySelectable: public Selectable, public RowsJoinerProxy, public AssetManager {
private:
    QVector<TkEntity*>          _entities;
    TkEntity*                   _baseEntity = nullptr;

public:
                                EntitySelectable(TkEntity*const entity = nullptr);
    void                        addEntity(TkEntity*const entity);
    void                        setBaseEntity(TkEntity*const entity);
    TkEntity*                   getBaseEntity(void) const;
    void                        removeEntities(QVector<TkEntity*>& entities);
    void                        removeEntity(TkEntity*const entity);
    virtual TYPE                getType(void) const override;
    const QVector<TkEntity*>&   getEntities(void) const;
    bool                        isInSelection(const TkEntity*const entity) const;

    void                        translate(const float x, const float y);
    void                        setPosition(const float x, const float y);

    virtual void                setCurrentSelection(const bool val, const Selectable*const other) override;
    virtual QAbstractItemModel* getModel(void) const override;

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




#endif // ENTITY_SELECTABLE_H
