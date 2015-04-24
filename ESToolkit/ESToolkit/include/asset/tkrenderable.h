#ifndef TKRENDERABLE_H
#define TKRENDERABLE_H

#include <elysian_renderable_base.hpp>

class TkRenderable: public elysian::RenderableBase {
friend class TkEntity;
friend class elysian::EntityBase;
protected:
                    TkRenderable(Entity& parent);
    virtual         ~TkRenderable(void) = default;

public:
                    TkRenderable(void) = default;

    virtual bool    validate(QString& string) const override;

};




// ========== INLINEZ ================
inline TkRenderable::TkRenderable(Entity &parent): elysian::RenderableBase(parent) {}

#endif // TKRENDERABLE_H

