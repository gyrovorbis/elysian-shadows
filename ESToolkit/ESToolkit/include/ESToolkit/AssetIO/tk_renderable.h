#ifndef TKRENDERABLE_H
#define TKRENDERABLE_H

#include <AssetIO/component/elysian_renderable_base.hpp>

class TkRenderable: public elysian::RenderableBase {
friend class TkEntity;
friend class elysian::EntityBase;
protected:
                    TkRenderable(Entity& parent, TkRenderable*const prefab=nullptr);
    virtual         ~TkRenderable(void) = default;

public:
                    TkRenderable(void) = default;

    virtual bool    validate(QString& string) const override;
    virtual QMenu*  createContextMenu(void) override;

    static void     initializeDelegates(void);

};




// ========== INLINEZ ================
inline TkRenderable::TkRenderable(Entity &parent, TkRenderable*const prefab): elysian::RenderableBase(parent, prefab) {}

#endif // TKRENDERABLE_H

