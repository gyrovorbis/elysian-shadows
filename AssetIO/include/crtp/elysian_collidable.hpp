#ifndef ELYSIAN_COLLIDABLE_HPP
#define ELYSIAN_COLLIDABLE_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_collidable_base.hpp>
namespace elysian {
    typedef CollidableBase          Collidable;
    typedef CollidableAttribBase    CollidableAttrib
}
#else
#   include <tk_collidable.h>
namespace elysian {
    typedef TkCollidableAttrib  CollidableAttrib;
    typedef TkCollidable        Collidable;
}
typedef TkCollidableAttrib  CollidableAttrib;
typedef TkCollidable        Collidable;

#endif





#endif
