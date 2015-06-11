#ifndef ELYSIAN_LIGHT_SOURCE_HPP
#define ELYSIAN_LIGHT_SOURCE_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_light_source.hpp>
namespace elysian {
    typedef LightSourceBase LightSource;
}
#else
#   include <tk_light_source.h>
namespace elysian {
    typedef TkLightSource LightSource;
}
typedef TkLightSource LightSource;

#endif

#endif // ELYSIAN_LIGHT_SOURCE_HPP

