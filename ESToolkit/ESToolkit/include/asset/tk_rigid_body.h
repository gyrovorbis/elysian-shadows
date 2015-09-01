#ifndef TKRIGIDBODY_H
#define TKRIGIDBODY_H

#include <component/elysian_rigid_body_base.hpp>

class TkRigidBody: public elysian::RigidBodyBase {
public:
    using                       LuaTableProxy<elysian::RigidBodyBase, RigidBody, ForceTarget>::initializeDelegates;

    TkRigidBody(TkEntity& parent, const TkRigidBody*const prefab=nullptr);
    TkRigidBody(void) = default;

    virtual bool    validate(QString &string) const override;
    virtual QMenu*  createContextMenu(void) override;
    virtual bool    canAddTo(const Entity& entity) const override;
    virtual bool    canRemoveFrom(const Entity& entity) const override;

};





// ========= INLINEZ ========

inline TkRigidBody::TkRigidBody(TkEntity &parent, const TkRigidBody*const prefab): elysian::RigidBodyBase(parent, prefab) {}

#endif // TKRIGIDBODY_H

