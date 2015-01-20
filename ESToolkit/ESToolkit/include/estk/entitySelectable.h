#ifndef ENTITY_SELECTABLE_H
#define ENTITY_SELECTABLE_H

#include "selectable.h"
#include "rowsJoinerProxyModel.h"
#include "assetManager.h"
#include <QVector>

class TkEntity;

class EntitySelectable: public Selectable, public RowsJoinerProxy, public AssetManager {
private:
    QVector<TkEntity*>          _entities;

public:
                                EntitySelectable(TkEntity*const entity = nullptr);
    void                        addEntity(TkEntity*const entity);
    void                        removeEntities(QVector<TkEntity*>& entities);
    void                        removeEntity(TkEntity*const entity);
    virtual TYPE                getType(void) const override;
    const QVector<TkEntity*>&   getEntities(void) const;

    void                        translate(const float x, const float y);

    virtual void                setCurrentSelection(const bool val, const Selectable*const other) override;

};




//======== INLINEZ ========

inline void EntitySelectable::removeEntities(QVector<TkEntity *> &entities) {
    for(auto it = entities.begin(); it != entities.end(); ++it)
        removeEntity(*it);
}


#endif // ENTITY_SELECTABLE_H
