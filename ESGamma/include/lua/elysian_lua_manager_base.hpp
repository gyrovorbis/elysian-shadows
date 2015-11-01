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

#define ELYSIAN_LUA_MANAGER_SERIALIZE_SCRIPT_PATH       "/assets/scripts/utils/serialize.lua"
#define ELYSIAN_LUA_MANAGER_TABLE_MANIP_SCRIPT_PATH     "/assets/scripts/utils/table_manip.lua"

#define ELYSIAN_LUA_PROXY_TYPE_METAFIELD                "__typename"

namespace elysian {
	
    class LuaVariantBase;
    using LuaVariant = LuaVariantBase;

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

        static void					rPrintByIndex(const int index, const unsigned depth, const char* name);
        static void					rPrintByRef(const int ref, const unsigned depth, const char* name);
        static void                 luaPrint(const char*const string, const Debug::SEVERITY severity = Debug::LOG_OFF);

        static bool                 getMetatable(const int index);
        static void                 getGlobal(const char*const key);

	};

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
        assert(_stackBalanceRecursiveDepth < ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH-1);
        _stackBalanceTop[_stackBalanceRecursiveDepth++] = stackDepth();
    }

    inline void LuaManagerBase::stackBalanceEnd(void) {
        if(_stackBalanceTop[--_stackBalanceRecursiveDepth] != stackDepth()) {
            _cStackDump();
            _errorprintf("Start Depth - %d, End Depth - %d", _stackBalanceTop[_stackBalanceRecursiveDepth], stackDepth());
            assert(false);
        }
    }

    inline bool LuaManagerBase::getMetatable(const int index) {
        return (bool)lua_getmetatable(*_mainState, index);
    }

    inline void LuaManagerBase::getGlobal(const char * const key) {
        lua_getglobal(*_mainState, key);
    }

    template<>
    struct LuaManagerBase::__RValuePull<const char*> {
        static const char* pull(lua_State* const state) {
            static char _internalBuffer[GYRO_PRINTF_BUFFER_SIZE];

            if(lua_isnil(state, -1)) {
               strcpy(_internalBuffer, "");
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


}

#endif
