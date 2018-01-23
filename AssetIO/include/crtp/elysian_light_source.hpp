#ifndef ELYSIAN_LIGHT_SOURCE_HPP
#define ELYSIAN_LIGHT_SOURCE_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_light_source_base.hpp>
namespace elysian {
    typedef LightSourceBase LightSource;
}
#else
#   include <AssetIO/tk_light_source.h>
namespace elysian {
    typedef TkLightSource LightSource;
}
typedef TkLightSource LightSource;

#endif

#endif // ELYSIAN_LIGHT_SOURCE_HPP

