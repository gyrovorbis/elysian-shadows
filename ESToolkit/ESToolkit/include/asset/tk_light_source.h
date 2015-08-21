#ifndef TK_LIGHT_SOURCE_H
#define TK_LIGHT_SOURCE_H

#include <component/elysian_light_source_base.hpp>

class TkLightSource: public elysian::LightSourceBase {
public:
    TkLightSource(TkEntity& parent);
    TkLightSource(void) = default;

    virtual bool    validate(QString &) const;
    virtual QMenu*  createContextMenu(void);

    static void     initializeDelegates(void);
};




// ======= INLINEZ ======

inline TkLightSource::TkLightSource(TkEntity &parent): elysian::LightSourceBase(parent) {}


#endif
