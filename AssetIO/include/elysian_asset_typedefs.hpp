#ifndef ELYSIAN_ASSET_TYPEDEFS_HPP
#define ELYSIAN_ASSET_TYPEDEFS_HPP

#ifdef ELYSIAN_ENGINE
#   define ELYSIAN_ASSET_TYPEDEF(type) \
        namespace elysian { \
			class type##Base; \
			typedef ##type##Base type; \
        }

#   define ELYSIAN_ASSET_HEADER(gamma, tk) gamma
#else
#   define ELYSIAN_ASSET_TYPEDEF(type) \
        class Tk##type; \
        namespace elysian { \
            typedef Tk##type type; \
        }
#   define ELYSIAN_ASSET_HEADER(gamma, tk) tk
#endif



//ELYSIAN_ASSET_HEADER("elysian_level.hpp", "level.h")


//ELYSIAN_ASSET_TYPEDEF(Level)
//ELYSIAN_ASSET_TYPEDEF(Area)
//ELYSIAN_ASSET_TYPEDEF(Entity)




#endif // ELYSIAN_ASSET_TYPEDEFS_HPP
