#ifndef ELYSIAN_SPRITE_ATTRIB_HPP
#define ELYSIAN_SPRITE_ATTRIB_HPP


#ifdef ELYSIAN_ENGINE
#   include <primitive/elysian_sprite_attrib_base.hpp>
namespace elysian {
    typedef SpriteAttribBase SpriteAttrib;
}
#else
#   include <AssetIO/tk_sprite_attrib.h>
namespace elysian {
    typedef TkSpriteAttrib SpriteAttrib;
}
typedef TkSpriteAttrib SpriteAttrib;

#endif


#endif // ELYSIAN_SPRITE_ATTRIB_HPP

