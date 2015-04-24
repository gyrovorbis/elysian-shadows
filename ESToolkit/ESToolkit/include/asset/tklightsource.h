#ifndef TK_LIGHT_SOURCE_H
#define TK_LIGHT_SOURCE_H

#include <elysian_light_source_base.hpp>

class TkLightSource: public elysian::LightSourceBase {
public:
    TkLightSource(TkEntity& parent);
    TkLightSource(void) = default;
};

inline TkLightSource::TkLightSource(TkEntity &parent): elysian::LightSourceBase(parent) {}


#endif
