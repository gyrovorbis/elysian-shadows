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




#   define ELYSIAN_ASSET_TYPEDEF_GENERIC(name, type) \
        type Tk##name; \
        namespace elysian { \
            typedef Tk##name name; \
        }

#   define ELYSIAN_ASSET_TYPEDEF_CLASS(name) ELYSIAN_ASSET_TYPEDEF_GENERIC(name, class)

#define ELYSIAN_ASSET_TYPEDEF_GET_MACRO(_1, _2, NAME, ...) NAME
#define ELYSIAN_ASSET_TYPEDEF(...) ELYSIAN_ASSET_TYPEDEF_GET_MACRO(__VA_ARGS__, ELYSIAN_ASSET_TYPEDEF_GENERIC, ELYSIAN_ASSET_TYPEDEF_CLASS)(__VA_ARGS__)


#   define ELYSIAN_ASSET_HEADER(gamma, tk) tk

#endif



//ELYSIAN_ASSET_HEADER("elysian_level.hpp", "level.h")


//ELYSIAN_ASSET_TYPEDEF(Level)
//ELYSIAN_ASSET_TYPEDEF(Area)
//ELYSIAN_ASSET_TYPEDEF(Entity)




#endif // ELYSIAN_ASSET_TYPEDEFS_HPP
