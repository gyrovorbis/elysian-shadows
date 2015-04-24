#ifndef ELYSIAN_ENTITY_HPP
#define ELYSIAN_ENTITY_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_entity_base.hpp>
namespace elysian {
    typedef EntityBase Entity;
}
#else
#   include <tkEntity.h>
namespace elysian {
    typedef TkEntity Entity;
}
typedef TkEntity Entity;

#endif


#endif // ELYSIAN_ENTITY_HPP
