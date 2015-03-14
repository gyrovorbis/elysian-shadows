#ifndef ELYSIAN_LUA_MANAGER_BASE_HPP
#define ELYSIAN_LUA_MANAGER_BASE_HPP

#include <map>
#include <oolua.h>
#include "elysian_stl_algorithm.hpp"
#include "elysian_lua_script.hpp"
#include "elysian_debug.hpp"

#if GYRO_PLATFORM == GYRO_PLATFORM_x86
#	define ELYSIAN_LUA_DEBUG                            true
#   define ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH     100
//#	define OOLUA_DEBUG_CHECKS 1
#else
#	define ELYSIAN_LUA_DEBUG                            true
#   define ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH     100
#endif

#define ELYSIAN_LUA_MANAGER_SERIALIZE_SCRIPT_PATH       "/assets/scripts/utils/serialize.lua"

namespace elysian {
	
    class LuaManagerBase {
    protected:
		
		static std::map<const char *, LuaScript *, ltstr> _scripts;
		static int _errorCode;
#ifdef ELYSIAN_LUA_DEBUG
        static int _stackBalanceTop[ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH];
        static int _stackBalanceRecursiveDepth;
#endif
		static const char _rPrint[];
        //static const char _serializeTable[];
        static int _traceBackDbgPushes;

		static void _errorprintf(const char *const string, ...);
		static void _registerWrappers(void);

        //================== TEMPLATE INTERNALS =====================

		template<class Type>
		static void __pushArg(Type& arg) {
			OOLUA::push(*_mainState, arg);
		}

		static void __pushArg(OOLUA::Lua_table_ref* ref) {
			//Lua_table_refs cannot be pushed with
			ref->push(*_mainState);
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
			static unsigned pull(lua_State* const state) {
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

		template<typename RType, typename... PTypes>
		static RType _callGlobalFunction(const char*const funcName, LuaScript* script, PTypes... parameters) {

			//Put the function on the top of the stack
			lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, script->getEnvironRef());	//push ENV
			lua_getfield(*_mainState, -1, funcName);								//get ENV.<function>()
			lua_remove(*_mainState, -2);											//remove ENV
			//Invoke next template to push args, make the call, and pop retVal
			return _callFunction<RType>(script, parameters...);
		}

                                    LuaManagerBase(void) = default;
		static void					_showError(void);
		static int					_getErrorCode(void);
		
		static void					_protectedCall(const int nargs, const int nresults, LuaScript *script);
		static LuaScript*			_createScriptFromExisting(const char* const file);

		static int					_traceBack (lua_State *L);
		static unsigned				_envDump(LuaScript* scr);
		static void					_cStackDump(void);
		static void					_runtimeStackDump(void);

        static int                  _stackDepth(void);

#ifdef ELYSIAN_LUA_DEBUG
#endif

	public:
        static void                 _stackBalanceBegin();
        static void                 _stackBalanceEnd();
		static bool					initialize(void);
		static bool					uninitialize(void);
		
		static bool					runString(const char *const string);
		static LuaScript*			loadScript(const char *const file);
		static LuaBehaviorScript*	loadBehaviorScript(const char *const file);
		static bool					unloadBehaviorScript(LuaBehaviorScript &script);
		static bool					unloadScript(LuaScript &script);
		static bool					runScriptSandbox(const char *const file, LuaScript* const script=nullptr);

		static void					runGarbageCollector(void);
		static void					dumpStats(void);

		static unsigned				getScriptCount(void);
		static unsigned				getScriptInstances(void);
		static unsigned				getKbytesUsed(void);

        static void					rPrintByIndex(const int index, const unsigned depth, const char* name);
        static void					rPrintByRef(const int ref, const unsigned depth, const char* name);

        static bool                 getMetatable(const int index);
        static void                 getGlobal(const char*const key);

	};

    inline void LuaManagerBase::_protectedCall(const int nargs, const int nresults, LuaScript *script) {
#ifdef ELYSIAN_LUA_DEBUG
        if(script) {
            lua_pushcfunction(*_mainState, &LuaManagerBase::_traceBack);
            lua_insert(*_mainState, 1);
        }
        if((_errorCode = lua_pcall(*_mainState, nargs, nresults, script? 1 : 0)) != 0) {
            if(script) {
                lua_remove(*_mainState, 1);
				script->setError();
				_cStackDump();
				lua_remove(*_mainState, lua_gettop(*_mainState));		//clean up stack after reesting
				_envDump(script);
				dumpStats();
                //if(_traceBackDbgPushes) {
                    Debug::pop();
                //    --_traceBackDbgPushes;
               // }
            } else {
                _showError();
                //lua_remove(*_mainState, 1);
                //_cStackDump();
                //dumpStats();
            }
		} else {
            if(script)
                lua_remove(*_mainState, 1); //remove c traceback function from bottom of stack
		}
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


    inline int LuaManagerBase::_stackDepth(void) {
        return _mainState->stack_count();
    }

    inline void LuaManagerBase::_stackBalanceBegin(void) {
        assert(_stackBalanceRecursiveDepth < ELYSIAN_LUA_STACK_BALANCE_RECURSIVE_DEPTH-1);
        _stackBalanceTop[_stackBalanceRecursiveDepth++] = _stackDepth();
    }

    inline void LuaManagerBase::_stackBalanceEnd(void) {
        if(_stackBalanceTop[--_stackBalanceRecursiveDepth] != _stackDepth()) {
            _cStackDump();
            _errorprintf("Start Depth - %d, End Depth - %d", _stackBalanceTop[_stackBalanceRecursiveDepth], _stackDepth());
            assert(false);
        }
    }

    inline bool LuaManagerBase::getMetatable(const int index) {
        return (bool)lua_getmetatable(*_mainState, index);
    }

    inline void LuaManagerBase::getGlobal(const char * const key) {
        lua_getglobal(*_mainState, key);
    }

}

#endif
