#ifndef ELYSIAN_CPP_LUA_FUNCTION_HPP
#define ELYSIAN_CPP_LUA_FUNCTION_HPP

#include <functional>
#include <utility/elysian_functional.hpp>
#include <crtp/elysian_lua_variant.hpp>

namespace elysian {

    template<typename Ret, typename... Args>
    class CppLuaFunction {
    public:

        using           StdFunction = std::function<Ret(Args...)>;

        enum class Type {
                        Null,
                        Cpp,
                        Lua
        };

                        CppLuaFunction(void) = default;
                        CppLuaFunction(LuaVariant v);
                        template<typename F,
                                 typename meta::is_callable<F>::value,
                                 typename std::enable_if_t<!std::is_same<F, CppLuaFunction&>::value, void>> //prevent generation of nonconst copy constructor
                        CppLuaFunction(F&& f);
                        CppLuaFunction(std::nullptr_t);

                        CppLuaFunction(const CppLuaFunction& rhs);
                        CppLuaFunction(CppLuaFunction&& rhs);

                        ~CppLuaFunction(void);

                        template<typename F>
        CppLuaFunction& operator=(F&& rhs);

        explicit        operator bool(void) const;

        Ret             operator()(Args... args) const;

        void            setValue(std::nullptr_t);
        void            setValue(LuaVariant v);
                        template<typename F,
                                 typename meta::is_callable<F>::value,
                                 typename std::enable_if_t<!std::is_same<F, CppLuaFunction&>::value, void>>
        void            setValue(F&& f);

        StdFunction     getCppFunction(void) const;
        LuaVariant      getLuaFunction(void) const;

        bool            isNull(void) const;
        Type            getType(void) const;
    private:

        void            _setType(Type type);

        union Union {
            StdFunction cpp;
            LuaVariant  lua;
        }               _union;

        Type            _type = Type::Null;

    };







    //=========INLINEZ=========

    template<typename Ret, typename... Args>
    inline CppLuaFunction<Ret, Args...>::CppLuaFunction(LuaVariant v):
        _union(std::move(v)), //Can I trust this since LuaVariant can also be assigned to StdFunction?
        _type(Type::Lua) {
    }


    template<typename Ret, typename... Args>
    template<typename F, typename meta::is_callable<F>::value, typename std::enable_if_t<!std::is_same<F, CppLuaFunction<Ret, Args...>&>::value, void>>
    inline CppLuaFunction<Ret, Args...>::CppLuaFunction(F&& f):
        _union(f),
        _type(Type::Cpp) {
    }

    template<typename Ret, typename... Args>
    inline CppLuaFunction<Ret, Args...>::CppLuaFunction(nullptr_t):
        CppLuaFunction()
    {}

    template<typename Ret, typename... Args>
    CppLuaFunction<Ret, Args...>::~CppLuaFunction(void) {
        _setType(Type::Null);
    }

    template<typename Ret, typename... Args>
    CppLuaFunction<Ret, Args...>::CppLuaFunction(const CppLuaFunction<Ret, Args...>& rhs):
        _type(rhs._type)
    {
        if(rhs._type == Type::Cpp) _union.cpp = rhs._union.cpp;
        else if(rhs._type == Type::Lua) _union.lua = rhs._union.lua;
    }

    template<typename Ret, typename... Args>
    CppLuaFunction<Ret, Args...>::CppLuaFunction(CppLuaFunction&& rhs):
        _type(rhs._type)
    {
        if(rhs._type == Type::Cpp) {
            _union.cpp = rhs._union.cpp;
        } else if(rhs._type == Type::Lua) {
            _union.lua = rhs._union.lua;
        }
        rhs = nullptr;
    }

    template<typename Ret, typename... Args>
    inline void CppLuaFunction<Ret, Args...>::_setType(Type type) {
        if(_type != type) {
            if(_type == Type::Cpp) {
                _union.cpp.~function();
            } else if(_type == Type::Lua) {
                _union.lua.~LuaVariant();
            }

            if(type == Type::Cpp) {
                new (_union.cpp) std::function<Ret(Args...)>();
            } else if(type == Type::Lua) {
                new (_union.lua) LuaVariant();
            }

            _union._type = type;
        }
    }

    template<typename Ret, typename... Args>
    inline auto CppLuaFunction<Ret, Args...>::getType(void) const -> Type {
        return _type;
    }

    template<typename Ret, typename... Args>
    inline bool CppLuaFunction<Ret, Args...>::isNull(void) const {
        return !static_cast<bool>(*this);
    }


    template<typename Ret, typename... Args>
    inline CppLuaFunction<Ret, Args...>::operator bool(void) const {
        return (_type == Type::Cpp && _union.cpp) || (_type == Type::Lua && _union.lua);
    }


    template<typename Ret, typename... Args>
    template<typename F>
    inline CppLuaFunction<Ret, Args...>& CppLuaFunction<Ret, Args...>::operator=(F&& rhs) {
        setValue(std::forward(rhs));
    }

    template<typename Ret, typename... Args>
    inline void CppLuaFunction<Ret, Args...>::setValue(std::nullptr_t) {
        _setType(Type::Null);
    }

    template<typename Ret, typename... Args>
    inline void CppLuaFunction<Ret, Args...>::setValue(LuaVariant v) {
        _setType(Type::Lua);
        _union.lua = std::move(v);
    }

    template<typename Ret, typename... Args>
    template<typename F, typename meta::is_callable<F>::value, typename std::enable_if_t<!std::is_same<F, CppLuaFunction<Ret, Args...>&>::value, void>>
    inline void CppLuaFunction<Ret, Args...>::setValue(F&& f) {
        _setType(Type::Cpp);
        _union.cpp = f;
    }

    template<typename Ret, typename... Args>
    inline std::function<Ret(Args...)> CppLuaFunction<Ret, Args...>::getCppFunction(void) const {
        GY_ASSERT(_type == Type::Cpp);
        return _union.cpp;
    }

    template<typename Ret, typename... Args>
    inline LuaVariant CppLuaFunction<Ret, Args...>::getLuaFunction(void) const {
        GY_ASSERT(_type == Type::Lua);
        return _union.lua;
    }

    //overloaded comparison operators
    template<typename Ret, typename... Args>
    inline bool operator==(const CppLuaFunction<Ret, Args...>& f1, const CppLuaFunction<Ret, Args...>& f2);
    template<typename Ret, typename... Args>
    inline bool operator==(const CppLuaFunction<Ret, Args...>& f, const LuaVariant& v);
    template<typename Ret, typename... Args>
    inline bool operator==(const CppLuaFunction<Ret, Args...>& f, const std::function<Ret(Args...)>& s);
    template<typename Ret, typename... Args>
    inline bool operator==(const CppLuaFunction<Ret, Args...>& f, std::nullptr_t);

    template<typename Ret, typename... Args>
    inline bool operator!=(const CppLuaFunction<Ret, Args...>& f1, const CppLuaFunction<Ret(Args...)>& f2);
    template<typename Ret, typename... Args>
    inline bool operator!=(const CppLuaFunction<Ret, Args...>& f, const LuaVariant& v);
    template<typename Ret, typename... Args>
    inline bool operator!=(const CppLuaFunction<Ret, Args...>& f, const std::function<Ret(Args...)>& s);
    template<typename Ret, typename... Args>
    inline bool operator!=(const CppLuaFunction<Ret, Args...>& f, std::nullptr_t);


}

#endif // ELYSIAN_CPP_LUA_FUNCTION_HPP
