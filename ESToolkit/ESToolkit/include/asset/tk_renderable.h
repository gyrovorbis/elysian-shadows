#ifndef TKRENDERABLE_H
#define TKRENDERABLE_H

#include <component/elysian_renderable_base.hpp>
#include <estk/tk_lua_wrappers.h>

class TkRenderable: public elysian::RenderableBase {
friend class TkEntity;
friend class elysian::EntityBase;
protected:
                    TkRenderable(Entity& parent);
    virtual         ~TkRenderable(void) = default;

public:
                    TkRenderable(void) = default;

    virtual bool    validate(QString& string) const override;
    virtual QMenu*  createContextMenu(void) override;

    static void     initializeDelegates(void);

};




// ========== INLINEZ ================
inline TkRenderable::TkRenderable(Entity &parent): elysian::RenderableBase(parent) {}

#endif // TKRENDERABLE_H

