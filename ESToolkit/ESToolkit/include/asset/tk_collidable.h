#ifndef TKCOLLIDABLE_H
#define TKCOLLIDABLE_H

#include <component/elysian_collidable_base.hpp>

class TkCollidable: public elysian::CollidableBase {
friend class TkEntity;
friend class elysian::EntityBase;
protected:
                    TkCollidable(Entity& parent);
    virtual         ~TkCollidable(void) = default;

public:
                    TkCollidable(void) = default;

    virtual bool    validate(QString& string) const override;
    virtual QMenu*  createContextMenu(void) override;

};




// ========== INLINEZ ================
inline TkCollidable::TkCollidable(Entity &parent): elysian::CollidableBase(parent) {}


#endif // TKCOLLIDABLE_H

