#ifndef TKRIGIDBODY_H
#define TKRIGIDBODY_H

#include <AssetIO/component/elysian_rigid_body_base.hpp>

class TkRigidBody: public elysian::RigidBodyBase {
public:
    using                       LuaTableProxy<elysian::RigidBodyBase, RigidBody, ForceTarget>::initializeDelegates;
    using                       LuaTableProxy<elysian::RigidBodyBase, RigidBody, ForceTarget>::initializeVirtualTableDelegates;

                    TkRigidBody(TkEntity& parent, TkRigidBody*const prefab=nullptr);
                    TkRigidBody(void) = default;

    virtual bool    validate(QString &string) const override;
    virtual QMenu*  createContextMenu(void) override;
    virtual bool    canAddTo(const Entity& entity) const override;
    virtual bool    canRemoveFrom(const Entity& entity) const override;

};





// ========= INLINEZ ========

inline TkRigidBody::TkRigidBody(TkEntity &parent, TkRigidBody*const prefab): elysian::RigidBodyBase(parent, prefab) {}

#endif // TKRIGIDBODY_H

