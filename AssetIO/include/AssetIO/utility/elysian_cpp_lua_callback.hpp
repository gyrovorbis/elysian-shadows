#ifndef ELYSIAN_CPP_LUA_CALLBACK_HPP
#define ELYSIAN_CPP_LUA_CALLBACK_HPP

#include <functional>
#include <AssetIO/utility/elysian_functional.hpp>
#include <AssetIO/crtp/elysian_lua_variant.hpp>

namespace elysian {

template<typename Ret, typename... Args>
class CppLuaCallback;

template<typename Ret, typename... Args>
bool operator==(const CppLuaCallback<Ret, Args...>& f, std::nullptr_t);
template<typename Ret, typename... Args>
bool operator==(const CppLuaCallback<Ret, Args...>& f, const LuaVariant& v);
template<typename Ret, typename... Args>
bool operator!=(const CppLuaCallback<Ret, Args...>& f, std::nullptr_t);
template<typename Ret, typename... Args>
bool operator!=(const CppLuaCallback<Ret, Args...>& f, const LuaVariant& v);

    template<typename Ret, typename... Args>
    class CppLuaCallback {

        friend bool operator == <>(const CppLuaCallback<Ret, Args...>& f, std::nullptr_t);
        friend bool operator == <>(const CppLuaCallback<Ret, Args...>& f, const LuaVariant& v);
        friend bool operator != <>(const CppLuaCallback<Ret, Args...>& f, std::nullptr_t);
        friend bool operator != <>(const CppLuaCallback<Ret, Args...>& f, const LuaVariant& v);
    public:

        using           StdCallback = std::function<Ret(Args...)>;

        enum class Type {
                        Null,
                        Cpp,
                        Lua
        };

                        CppLuaCallback(void);

                        template<typename F,
                                 typename = enable_if_t<!std::is_same<remove_reference_t<F>, LuaVariant>::value, void>,
                                 typename = enable_if_t<is_callable<std::remove_reference_t<F>, Ret(Args...) const>::value, F>>
                        CppLuaCallback(F&& f);

                        template<typename F,
                                 typename = enable_if_t<std::is_same<remove_reference_t<F>, LuaVariant>::value
                                 /* || is_callable<std::remove_reference_t<F>, LuaVariant(Args...) const>::value*/, F>>
                        CppLuaCallback(F&& f);

                        CppLuaCallback(std::nullptr_t);

                        CppLuaCallback(const CppLuaCallback& rhs);
                        CppLuaCallback(CppLuaCallback&& rhs);

                        ~CppLuaCallback(void);

                        template<typename F>
        CppLuaCallback& operator=(F&& rhs);

        CppLuaCallback& operator=(std::nullptr_t);

        explicit        operator bool(void) const;

        template <typename... _Args>
        Ret             operator()(_Args&&... args) const;

        void            setValue(std::nullptr_t);
        void            setValue(LuaVariant v);
                        template<typename F,
                                 typename meta::is_callable<F>::value,
                                 typename std::enable_if_t<!std::is_same<F, CppLuaCallback&>::value, void>>
        void            setValue(F&& f);

        StdCallback     getCppCallback(void) const;
        LuaVariant      getLuaCallback(void) const;

        bool            isNull(void) const;
        Type            getType(void) const;
    private:

        void            _setType(Type type);

        union Union {
        public:
            Union(void): lua() {}
            Union(const StdCallback& callback): cpp(callback) {}
            Union(StdCallback&& callback):      cpp(callback) {}
            Union(const LuaVariant& variant):   lua(variant) {}
            Union(LuaVariant&& variant):        lua(variant) {}
            ~Union(void)                                    {}
            StdCallback cpp;
            LuaVariant  lua;
        }               _union;

        Type            _type = Type::Null;

    };







    //=========INLINEZ=========

    template<typename Ret, typename... Args>
    CppLuaCallback<Ret, Args...>::CppLuaCallback(void):
        _type(Type::Null)
    {}

    template<typename Ret, typename... Args>
    template<typename F, typename, typename>
    inline CppLuaCallback<Ret, Args...>::CppLuaCallback(F&& f):
        _union(std::forward<F>(f)),
        _type(Type::Cpp)
    {}

    template<typename Ret, typename... Args>
    template<typename F, typename>
    inline CppLuaCallback<Ret, Args...>::CppLuaCallback(F&& f):
        _union(std::forward<F>(f)),
        _type(Type::Lua) {}

    template<typename Ret, typename... Args>
    inline CppLuaCallback<Ret, Args...>::CppLuaCallback(nullptr_t):
        CppLuaCallback()
    {}

    template<typename Ret, typename... Args>
    CppLuaCallback<Ret, Args...>::~CppLuaCallback(void) {
        _setType(Type::Null);
    }

    template<typename Ret, typename... Args>
    CppLuaCallback<Ret, Args...>::CppLuaCallback(const CppLuaCallback<Ret, Args...>& rhs):
        _type(rhs._type)
    {
        if(rhs._type == Type::Cpp) _union.cpp = rhs._union.cpp;
        else if(rhs._type == Type::Lua) _union.lua = rhs._union.lua;
    }

    template<typename Ret, typename... Args>
    CppLuaCallback<Ret, Args...>::CppLuaCallback(CppLuaCallback&& rhs):
        _type(rhs._type)
    {
        if(rhs._type == Type::Cpp) {
            _union.cpp = rhs._union.cpp;
        } else if(rhs._type == Type::Lua) {
            _union.lua = rhs._union.lua;
        }
        rhs.setValue(nullptr);
    }

    template<typename Ret, typename... Args>
    inline void CppLuaCallback<Ret, Args...>::_setType(Type type) {
        if(_type != type) {
            if(_type == Type::Cpp) {
                _union.cpp.~StdCallback();
            } else if(_type == Type::Lua) {
                _union.lua.~LuaVariant();
            }

            if(type == Type::Cpp) {
                new (&_union.cpp) std::function<Ret(Args...)>();
            } else if(type == Type::Lua) {
                new (&_union.lua) LuaVariant();
            }

            _type = type;
        }
    }

    template<typename Ret, typename... Args>
    inline auto CppLuaCallback<Ret, Args...>::getType(void) const -> Type {
        return _type;
    }

    template<typename Ret, typename... Args>
    inline bool CppLuaCallback<Ret, Args...>::isNull(void) const {
        return !static_cast<bool>(*this);
    }
/* Has to be partially specialized so void functions don't still try to pull a value from Lua!
 */
    template<typename Ret, typename... Args>
    template<typename... _Args>
    inline Ret CppLuaCallback<Ret, Args...>::operator()(_Args&&... args) const {
        switch(_type) {
        case Type::Cpp:
            return _union.cpp(std::forward<_Args>(args)...);
        case Type::Lua:
            return _union.lua(std::forward<_Args>(args)...).template getValue<Ret>();
        default: GY_ASSERT(false);
        }

    }


    template<typename Ret, typename... Args>
    inline CppLuaCallback<Ret, Args...>::operator bool(void) const {
        return (_type == Type::Cpp && _union.cpp) || (_type == Type::Lua && _union.lua.getType() == LUA_TFUNCTION);
    }


    template<typename Ret, typename... Args>
    template<typename F>
    inline CppLuaCallback<Ret, Args...>& CppLuaCallback<Ret, Args...>::operator=(F&& rhs) {
        setValue(std::forward<Args>(rhs)...);
    }

    template<typename Ret, typename... Args>
    inline CppLuaCallback<Ret, Args...>& CppLuaCallback<Ret, Args...>::operator=(std::nullptr_t) {
        _setType(Type::Null);
    }

    template<typename Ret, typename... Args>
    inline void CppLuaCallback<Ret, Args...>::setValue(std::nullptr_t) {
        _setType(Type::Null);
    }

    template<typename Ret, typename... Args>
    inline void CppLuaCallback<Ret, Args...>::setValue(LuaVariant v) {
        _setType(Type::Lua);
        _union.lua = std::move(v);
    }

    template<typename Ret, typename... Args>
    template<typename F, typename meta::is_callable<F>::value, typename std::enable_if_t<!std::is_same<F, CppLuaCallback<Ret, Args...>&>::value, void>>
    inline void CppLuaCallback<Ret, Args...>::setValue(F&& f) {
        _setType(Type::Cpp);
        _union.cpp = f;
    }

    template<typename Ret, typename... Args>
    inline std::function<Ret(Args...)> CppLuaCallback<Ret, Args...>::getCppCallback(void) const {
        GY_ASSERT(_type == Type::Cpp);
        return _union.cpp;
    }

    template<typename Ret, typename... Args>
    inline LuaVariant CppLuaCallback<Ret, Args...>::getLuaCallback(void) const {
        GY_ASSERT(_type == Type::Lua);
        return _union.lua;
    }

    //overloaded comparison operators
    template<typename Ret, typename... Args>
    inline bool operator==(const CppLuaCallback<Ret, Args...>& f, const LuaVariant& v) {
        if(f.getType() == CppLuaCallback<Ret, Args...>::Type::Lua) {
            return f._union.lua == v;
        }
        return false;
    }

    template<typename Ret, typename... Args>
    inline bool operator==(const CppLuaCallback<Ret, Args...>& f, std::nullptr_t) {
         return (f.getType() == CppLuaCallback<Ret, Args...>::Type::Null);
    }

    template<typename Ret, typename... Args>
    inline bool operator!=(const CppLuaCallback<Ret, Args...>& f, const LuaVariant& v) {
        return !(f == v);
    }

    template<typename Ret, typename... Args>
    inline bool operator!=(const CppLuaCallback<Ret, Args...>& f, std::nullptr_t) {
        return !(f == nullptr);
    }


}

#endif // ELYSIAN_CPP_LUA_Callback_HPP
