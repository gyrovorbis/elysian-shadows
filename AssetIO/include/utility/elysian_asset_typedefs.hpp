#ifndef ELYSIAN_ASSET_TYPEDEFS_HPP
#define ELYSIAN_ASSET_TYPEDEFS_HPP

#ifdef ELYSIAN_ENGINE
#   define ELYSIAN_ASSET_TYPEDEF_GENERIC(name, type) \
        namespace elysian { \
            type name##Base; \
            typedef name##Base name; \
        }

#   define ELYSIAN_ASSET_HEADER(gamma, tk) gamma

#   define ELYSIAN_ASSET_VIRTUAL

#else

#   define ELYSIAN_ASSET_TYPEDEF_GENERIC(name, type) \
        type Tk##name; \
        namespace elysian { \
            typedef Tk##name name; \
        } \
        typedef Tk##name name;

#   define ELYSIAN_ASSET_HEADER(gamma, tk) tk

#   define ELYSIAN_ASSET_VIRTUAL virtual

#endif

#   define ELYSIAN_ASSET_TYPEDEF_CLASS(name) ELYSIAN_ASSET_TYPEDEF_GENERIC(name, class)

#   define ELYSIAN_ASSET_TYPEDEF_GET_MACRO(_1, _2, NAME, ...) NAME
#   define ELYSIAN_ASSET_TYPEDEF(...) ELYSIAN_ASSET_TYPEDEF_GET_MACRO(__VA_ARGS__, ELYSIAN_ASSET_TYPEDEF_GENERIC, ELYSIAN_ASSET_TYPEDEF_CLASS)(__VA_ARGS__)


#endif // ELYSIAN_ASSET_TYPEDEFS_HPP
