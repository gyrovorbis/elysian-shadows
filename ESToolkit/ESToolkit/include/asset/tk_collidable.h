#ifndef TKCOLLIDABLE_H
#define TKCOLLIDABLE_H

#include <component/elysian_collidable_base.hpp>

class TkCollidableAttrib: public elysian::CollidableAttribBase {
public:

    static void initializeDelegates(void);

};


class TkCollidable: public elysian::CollidableBase {
friend class TkEntity;
friend class elysian::EntityBase;
protected:
                    TkCollidable(Entity& parent, const TkCollidable*const prefab=nullptr);
    virtual         ~TkCollidable(void) = default;

public:
                    TkCollidable(void) = default;

    virtual bool    validate(QString& string) const override;
    virtual QMenu*  createContextMenu(void) override;
    virtual bool    canRemoveFrom(const Entity &) const override;

};




// ========== INLINEZ ================
inline TkCollidable::TkCollidable(Entity &parent, const TkCollidable*const prefab): elysian::CollidableBase(parent, prefab) {}


#endif // TKCOLLIDABLE_H

