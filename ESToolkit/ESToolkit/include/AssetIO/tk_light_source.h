#ifndef TK_LIGHT_SOURCE_H
#define TK_LIGHT_SOURCE_H

#include <component/elysian_light_source_base.hpp>

class TkLightSource: public elysian::LightSourceBase {
public:
    TkLightSource(void) = default;
    TkLightSource(TkEntity& parent, TkLightSource*const prefab=nullptr);

    virtual bool    validate(QString &) const;
    virtual QMenu*  createContextMenu(void);

    static void     initializeDelegates(void);
};




// ======= INLINEZ ======

inline TkLightSource::TkLightSource(TkEntity &parent, TkLightSource*const prefab): elysian::LightSourceBase(parent, prefab) {}


#endif
