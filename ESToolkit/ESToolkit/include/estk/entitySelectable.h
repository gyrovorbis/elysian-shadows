#ifndef ENTITY_SELECTABLE_H
#define ENTITY_SELECTABLE_H

#include "selectable.h"
#include "rowsJoinerProxyModel.h"
#include <QVector>

class TkEntity;

class EntitySelectable: public Selectable, public RowsJoinerProxy {
private:
	QVector<TkEntity*> _entities;

public:
						EntitySelectable(TkEntity*const entity = nullptr);
	void				addEntity(TkEntity*const entity);
	void				removeEntity(TkEntity*const entity);
	virtual TYPE		getType(void) const override;
	QVector<TkEntity*>& getEntities(void);

    void                translate(const float x, const float y);

    virtual void        setCurrentSelection(const bool val) override;

};

#endif // ENTITY_SELECTABLE_H
