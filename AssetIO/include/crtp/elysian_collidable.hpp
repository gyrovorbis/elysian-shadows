#ifndef ELYSIAN_COLLIDABLE_HPP
#define ELYSIAN_COLLIDABLE_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_collidable_base.hpp>
namespace elysian {
    typedef CollidableBase Collidable;
}
#else
#   include <tk_collidable.h>
namespace elysian {
    typedef TkCollidable Collidable;
}
typedef TkCollidable Collidable;

#endif





#endif
