#ifndef ELYSIAN_LUA_MANAGER_BASE_HPP
#define ELYSIAN_LUA_MANAGER_BASE_HPP

#include <map>
#include <oolua.h>
#include <functional>
#include <utility/elysian_stl_algorithm.hpp>
#include <elysian_lua_script.hpp>
#include <elysian_debug.hpp>
#include <gyro_defines.h>

#if GYRO_PLATFORM == GYRO_PLATFORM_x86
#	define ELYSIAN_LUA_DEBUG                            true
#   define ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH     100
//#	define OOLUA_DEBUG_CHECKS 1
#else
#	define ELYSIAN_LUA_DEBUG                            true
#   define ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH     100
#endif

#define ELYSIAN_LUA_MANAGER_SERIALIZE_SCRIPT_PATH       "/assets/scripts/utility/serialize.lua"
#define ELYSIAN_LUA_MANAGER_TABLE_MANIP_SCRIPT_PATH     "/assets/scripts/utility/table_manip.lua"
#define ELYSIAN_LUA_PREFAB_PATH                         "/assets/scripts/prefab"

#define ELYSIAN_LUA_PROXY_TYPE_METAFIELD                "__typename"
#define ELYSIAN_LUA_PROXY_DOWN_CAST_METAFIELD           "__downcast"

namespace elysian {
	
    class LuaVariantBase;
    using LuaVariant = LuaVariantBase;
    using LuaPair = std::pair<LuaVariantBase, LuaVariantBase>;

    class LuaManagerBase {
    protected:
		
		static std::map<const char *, LuaScript *, ltstr> _scripts;
        static int              _errorCode;
        static Debug::SEVERITY  _printSeverity;
#ifdef ELYSIAN_LUA_DEBUG
        static int              _stackBalanceTop[ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH];
        static int              _stackBalanceRecursiveDepth;
#endif
        static const char       _rPrint[];
        static int              _traceBackDbgPushes;
        static bool             _serializing;

        static int              _changePrintSeverity(const int sev);
        static void             _errorprintf(const char *const string, ...);
        static void             _registerWrappers(void);

        //================== TEMPLATE INTERNALS =====================

        struct RawTableReference {
            RawTableReference(const int _ref): ref(_ref) {}
            int ref = 0;
        };

		template<class Type>
		static void __pushArg(Type& arg) {
			OOLUA::push(*_mainState, arg);
		}

		static void __pushArg(OOLUA::Lua_table_ref* ref) {
			//Lua_table_refs cannot be pushed with
			ref->push(*_mainState);
		}

        static void __pushArg(RawTableReference ref) {
            lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref.ref);
        }

		static unsigned __consumeArgs(void) {
			return 0;
		}

		template <class First, class... Rest>
		static unsigned __consumeArgs(First first, Rest... rest) {
			unsigned argCount = 1;
			__pushArg(first);

			argCount += __consumeArgs(rest...); // Unpack the arguments for further treatment
			return argCount;
		}

		template <typename RType>
		struct __RValueCount {
			static unsigned count(void) { return 1; }
		};

		template<typename RType>
		struct __RValuePull {
            static RType pull(lua_State* const state) {
				RType retVal;
				//pull return value from Lua
				OOLUA::pull(state, retVal);
				return retVal;
			}
		};


        //============== /TEMPLATE INTERNALS ==================

	
	protected:
		static OOLUA::Script *_mainState;


		//Assumes actual function is already at the top of the stack!
		template<typename RType, typename... PTypes>
		static RType _callFunction(LuaScript* script, PTypes... parameters) {
			unsigned argc = __consumeArgs(parameters...);
			argc = sizeof...(PTypes);
			unsigned retc = __RValueCount<RType>::count();

			_protectedCall(argc, retc, script);
			return __RValuePull<RType>::pull(*_mainState);
        }
    public:
		template<typename RType, typename... PTypes>
		static RType _callGlobalFunction(const char*const funcName, LuaScript* script, PTypes... parameters) {

			//Put the function on the top of the stack
            if(script) { //if script is provided, check _ENV first and let it defer to _G
                lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, script->getEnvironRef());	//push ENV
                lua_getfield(*_mainState, -1, funcName);								//get ENV.<function>()
                lua_remove(*_mainState, -2);											//remove ENV
            } else { //No _ENV provided, call _G directly
                lua_getglobal(*_mainState, funcName);
            }
			//Invoke next template to push args, make the call, and pop retVal
			return _callFunction<RType>(script, parameters...);
		}

    protected:

                                    LuaManagerBase(void) = default;
		static void					_showError(void);
		static int					_getErrorCode(void);
		
        static bool					_protectedCall(const int nargs, const int nresults, LuaScript *script, const char*const msg=nullptr, ...);
		static LuaScript*			_createScriptFromExisting(const char* const file);

        static int					_traceBack(lua_State *L);
		static unsigned				_envDump(LuaScript* scr);
		static void					_cStackDump(void);
		static void					_runtimeStackDump(void);



        static bool                 genericIterate(const int index, std::function<void(void)> lambda);

	public:

		static bool					initialize(void);
		static bool					uninitialize(void);
		
        static LuaVariant           exec(const char*const str, ...);
		static bool					runString(const char *const string);
		static LuaScript*			loadScript(const char *const file);
		static LuaBehaviorScript*	loadBehaviorScript(const char *const file);
		static bool					unloadBehaviorScript(LuaBehaviorScript &script);
		static bool					unloadScript(LuaScript &script);
        static bool					runScriptSandbox(const char *const file, LuaScript* const script=nullptr, const unsigned retCount = 0);

		static void					runGarbageCollector(void);
		static void					dumpStats(void);
        static int                  stackDepth(void);
        static void                 stackBalanceBegin(void);
        static void                 stackBalanceEnd(void);

		static unsigned				getScriptCount(void);
		static unsigned				getScriptInstances(void);
		static unsigned				getKbytesUsed(void);
        static bool                 isSerializing(void);
        static void                 setSerializing(const bool v);

        static void					rPrintByIndex(const int index, const unsigned depth, const char* name);
        static void					rPrintByRef(const int ref, const unsigned depth, const char* name);
        static void                 luaPrint(const char*const string, const Debug::SEVERITY severity = Debug::LOG_OFF);

        static bool                 getMetatable(const int index);
        static void                 pushGlobalField(const char*const key);
        static LuaVariant           getGlobalsTable(void);
        static LuaVariant           getPrefab(const char*const name);
        static const char*          getPrefabName(const LuaVariant& v);

        static bool                 saveTable(const LuaVariant& v, const char*const filename);





        // NEWFANGLED-ASS STACK API

        //for everything where LuaVariant creation is not more expensive than raw
        //Anything creating a temporary reference can be less expensive
        static void push(const LuaVariant& variant);
        static void push(const char*const string); //gracefully push nil for nullptr
        static void push(const std::nullptr_t null);
        static void push(void); //doesn't do shit
        static void push(const int number);
        static void push(const float number);
        static void push(const double number);
        static void push(const bool boolean);
        static void push(void*const lightUd);
        template<typename T>
        static void push(T*const userdata);
        static void push(OOLUA::Lua_table_ref& tableRef);
        static void push(OOLUA::Lua_func_ref& funcRef);
        static void push(const std::initializer_list<LuaPair>& pairs);
        template<typename... Upvalues>
        static void push(const lua_CFunction funcPtr/*, Upvalues...*/);
        template<typename Ret, typename... Args>
        static void push(const std::function<Ret(Args...)>& closure);
        template<typename T>
        static void push(const T lambda);
        template<typename First, typename... Rest>
        static void push(First first, Rest... rest);


        //template<typename T>
        //static void pullTuple(const T& tuple, )




        void pushField(const int index, const LuaVariant& variant);
        void pushField(const int index, const char*const string);

        int getTop(void) const;

        void pop(const unsigned count=1);

        template<typename T>
        T pull(void) const;

        void insertAt(const int index, const LuaVariant& variant);

        void removeAt(const int index) const;
        void swap(const int index1, const int index2) const;

        void clear(void) const;

	};











    // ========= INLINEZ ========


    inline bool LuaManagerBase::_protectedCall(const int nargs, const int nresults, LuaScript *script, const char* /*context*/, ...) {
#ifdef ELYSIAN_LUA_DEBUG
        if(script) {
            lua_pushcfunction(*_mainState, &LuaManagerBase::_traceBack);
            lua_insert(*_mainState, 1);
        }
        if((_errorCode = lua_pcall(*_mainState, nargs, nresults, script? 1 : 0)) != 0) {
            DbgEvent event("Lua Exception");
            if(script) {
                lua_remove(*_mainState, 1);
                script->setError();
                _cStackDump();
                lua_remove(*_mainState, lua_gettop(*_mainState));		//clean up stack after reesting
                //_envDump(script);
                dumpStats();
                //if(_traceBackDbgPushes) {
                    //Debug::pop();
                //    --_traceBackDbgPushes;
               // }
            } else {
                _showError();
                //lua_remove(*_mainState, 1);
                //_cStackDump();
                //dumpStats();
            }
            return false; //shit reested
        } else {
            if(script)
                lua_remove(*_mainState, 1); //remove c traceback function from bottom of stack
        }
        return true; // shit was just fine
#else	//ain't NOBODY got time for debug overhead!
        lua_call(*_mainState, nargs, nresults);
#endif
	}

	//private template specializations
	template <>
    struct LuaManagerBase::__RValueCount<void> {
		static unsigned count(void) { return 0; }
	};

	template<>
    struct LuaManagerBase::__RValuePull<void> {
		static void pull(lua_State* const /*state*/) {}
	};


    inline int LuaManagerBase::stackDepth(void) {
        return _mainState->stack_count();
    }

    inline void LuaManagerBase::stackBalanceBegin(void) {
        GY_ASSERT(_stackBalanceRecursiveDepth < ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH-1);
        _stackBalanceTop[_stackBalanceRecursiveDepth++] = stackDepth();
    }

    inline void LuaManagerBase::stackBalanceEnd(void) {
        if(_stackBalanceTop[--_stackBalanceRecursiveDepth] != stackDepth()) {
            _cStackDump();
            _errorprintf("Start Depth - %d, End Depth - %d", _stackBalanceTop[_stackBalanceRecursiveDepth], stackDepth());
            GY_ASSERT(false);
        }
    }

    inline bool LuaManagerBase::getMetatable(const int index) {
        return (bool)lua_getmetatable(*_mainState, index);
    }

    inline void LuaManagerBase::pushGlobalField(const char * const key) {
        lua_getglobal(*_mainState, key);
        GY_ASSERT(!lua_isnil(*_mainState, -1));
    }

    template<>
    struct LuaManagerBase::__RValuePull<const char*> {
        static const char* pull(lua_State* const state) {
            static char _internalBuffer[GYRO_PRINTF_BUFFER_SIZE];

            if(lua_isnil(state, -1)) {
                lua_pop(state, 1);
               return nullptr;
            } else {
                strcpy(_internalBuffer, lua_tostring(state, -1));
                lua_pop(state, 1);
            }
            return _internalBuffer; //you better copy this shit asap
        }
    };

    template<>
    struct LuaManagerBase::__RValuePull<LuaManagerBase::RawTableReference> {
        LuaManagerBase::RawTableReference pull(lua_State* const state) {
            LuaManagerBase::RawTableReference ref(luaL_ref(state, -1));
            return ref;
        }
    };

    inline int LuaManagerBase::_changePrintSeverity(const int sev) {
        int current = _printSeverity;
        _printSeverity = (Debug::SEVERITY)sev;
        return current;
    }

    inline bool LuaManagerBase::isSerializing(void) { return _serializing; }
    inline void LuaManagerBase::setSerializing(const bool v) { _serializing = v; }













    inline void LuaManagerBase::push(const char*const string) {
        if(string) lua_pushstring(*_mainState, string);
        else lua_pushnil(*_mainState);
    }

    inline void LuaManagerBase::push(const std::nullptr_t) {
        lua_pushnil(*_mainState);
    }


    inline void LuaManagerBase::push(const int number) {
        lua_pushnumber(*_mainState, number);
    }

    inline void LuaManagerBase::push(const float number) {
        lua_pushnumber(*_mainState, number);
    }

    inline void LuaManagerBase::push(const double number) {
        lua_pushnumber(*_mainState, number);
    }

    inline void LuaManagerBase::push(const bool boolean) {
        lua_pushboolean(*_mainState, boolean);
    }

    inline void LuaManagerBase::push(void*const lightUd) {
        lua_pushlightuserdata(*_mainState, lightUd);
    }

    inline void LuaManagerBase::push(OOLUA::Lua_table_ref& tableRef) {
        OOLUA::push(*_mainState, tableRef);
    }

    inline void LuaManagerBase::push(OOLUA::Lua_func_ref& funcRef) {
        OOLUA::push(*_mainState, funcRef);
    }

    template<typename T>
    inline void LuaManagerBase::push(T* const userdata) {
        OOLUA::push(*_mainState, userdata);
    }

    template<typename First, typename... Rest>
    inline void LuaManagerBase::push(First first, Rest... rest) {
        push(first);
        push(rest...);
    }


    template<typename... Upvalues>
    inline void LuaManagerBase::push(const lua_CFunction funcPtr/*, Upvalues... upvalues*/) {
        //push(upvalues...);
        lua_pushcclosure(*_mainState, funcPtr, 0/*sizeof...(Upvalues)*/);
    }

    template<typename Ret, typename... Args>
    inline void push(const std::function<Ret(Args...)>&/* closure*/) {
#if 0
        auto _callLambda = [](lua_State* vm) {
            std::tuple<Args...> argTuple;

        };

        //Assumes actual function is already at the top of the stack!
        template<typename Ret, typename... Args>
        inline int LuaVariantBase::_callLambda(lua_State*/*vm*/) {
            std::tuple<Args...> argTuple;

            pull_tuple(argTuple, int_<sizeof...(Args)>());

            lua_pushvalue(*_mainState, lua_upvalueindex(1));
            LuaVariantBase t = LuaVariantBase::fromStack();

            auto* closure = static_cast<const std::function<Ret(Args...)>*>( t[1]->getValue<void*>() );

            GY_ASSERT(closure);

            const Ret retVal = forward<Ret>(argTuple, *closure);

            OOLUA::push(*_mainState, retVal);
            return 1;
        }

        LuaVariantBase upValue = {
            { 1, (void*)new std::function<Ret(Args...)>(closure) },
            { "__gc", (lua_CFunction)[](lua_State*) {
                  LuaVariantBase t = LuaVariantBase::fromStack();
                    GY_ASSERT(t.getType() == LUA_TTABLE);
                    dbgprintf("LUA LAMBDA FUNCTION FINALIZER INVOKED, NOT A MEMORY LEAK!!!111");
                    delete static_cast<std::function<Ret(Args...)>*>(t[1]->getValue<void*>());
                    return 0;
                }
            }
        };

        upValue.setMetatable(upValue);
        upvalue.push();

        //push as a regular C function, with lambda table as upvalue
        //push((lua_CFunction)&_callLambda<Ret, Args...>, upValue);
        lua_pushcclosure(*_mainState, (lua_CFunction)&_callLambda<Ret, Args...>, 1);
#endif

    }

    template<typename T>
    inline void LuaManagerBase::push(const T /*lambda*/) {

    }

}

#endif
