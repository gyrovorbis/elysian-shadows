#ifndef TKCOMPONENT_H
#define TKCOMPONENT_H

#include <AssetIO/component/elysian_component_base.hpp>

class QString;
class QMenu;

namespace elysian {
    class EntityBase;
}
class TkEntity;
typedef TkEntity Entity;

class TkComponent: public elysian::ComponentBase {
    friend class elysian::EntityBase;
    friend Entity;
protected:
    virtual         ~TkComponent(void) = default;
                    TkComponent(Entity& parent);
                    TkComponent(void) = default;

public:
                    //GET RID OF THESE DEFAULT!!!!
    virtual bool    validate(QString& /*str*/) const {return true;}//= 0;
    virtual bool    canAddTo(const Entity& /*entity*/) const{return true;}// = 0;
    virtual bool    canRemoveFrom(const Entity& /*entity*/) const{return true;}// = 0;
    virtual TYPE    getType(void) const { return elysian::ComponentBase::MAX_TYPE; }
    virtual QMenu*  createContextMenu(void) { return nullptr; }


};






//============== INLINEZ ===============

inline TkComponent::TkComponent(Entity &parent): elysian::ComponentBase(parent) {}

#endif // TKCOMPONENT_H

