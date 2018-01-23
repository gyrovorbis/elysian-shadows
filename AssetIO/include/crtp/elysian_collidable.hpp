#ifndef ELYSIAN_COLLIDABLE_HPP
#define ELYSIAN_COLLIDABLE_HPP

#ifdef ELYSIAN_ENGINE
#   include <component/elysian_collidable_base.hpp>
namespace elysian {
    typedef CollidableBase          Collidable;
    typedef CollidableAttribBase    CollidableAttrib;
}
#else
#   include <AssetIO/tk_collidable.h>
namespace elysian {
    typedef TkCollidableAttrib  CollidableAttrib;
    typedef TkCollidable        Collidable;
}
typedef TkCollidableAttrib  CollidableAttrib;
typedef TkCollidable        Collidable;

#endif





#endif
