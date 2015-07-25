#ifndef TKRIGIDBODY_H
#define TKRIGIDBODY_H

#include <component/elysian_rigid_body_base.hpp>

class TkRigidBody: public elysian::RigidBodyBase {
public:
    TkRigidBody(TkEntity& parent);
    TkRigidBody(void) = default;

    virtual bool    validate(QString &string) const override;
    virtual QMenu*  createContextMenu(void) override;
    virtual bool    canAddTo(const Entity &entity) const override;

};





// ========= INLINEZ ========

inline TkRigidBody::TkRigidBody(TkEntity &parent): elysian::RigidBodyBase(parent) {}

#endif // TKRIGIDBODY_H

