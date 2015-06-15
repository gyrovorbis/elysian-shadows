#ifndef ELYSIAN_LUA_SCRIPT_H
#define ELYSIAN_LUA_SCRIPT_H

#include "elysian_behavior.hpp"
#include <oolua.h>

#define ELYSIAN_LUA_SCRIPT_PATH_SIZE 200

struct lua_State;

namespace elysian {
	
	/*
	  Abstraction for a single lua script file and its ENV
	*/
	class LuaScript {
	protected:
        char            _path[ELYSIAN_LUA_SCRIPT_PATH_SIZE];
        unsigned        _refCount		= 1;
        int             _environRef;
        bool            _error			= false;

	public:
		enum TYPE { TYPE_SCRIPT, TYPE_BEHAVIOR };
		
                        LuaScript(const char *path);
        virtual         ~LuaScript(void) = default;
		
        const char*     getPath(void) const;
        unsigned        getRefCount(void) const;
        void            incRefCount(void);
        void            decRefCount(void);
        int             getEnvironRef(void) const;
        void            setEnvironRef(const int ref);
        bool            getError(void) const;
        void            setError(const bool err=true);

        virtual TYPE    getType(void) const;
	};

	//Supplied by Lua
	class LuaBehaviorScriptAttrib {
	private:
		uint64_t	_triggers				= 0;
		char		_parentPath[200]		= "none";
		bool		_hasParent				= false;
	public:
        uint64_t    getSupportedTriggers(void) const;
        const char* getParentPath(void) const;
        void        addSupportedTrigger(const Behavior::TRIGGER trigger);
        void        inheritFromBehavior(const char*const path);
        bool        hasParent(void) const;
	};

	class LuaBehaviorScript: public LuaScript {
	private:
        LuaBehaviorScriptAttrib     _attrib;
        LuaBehaviorScript*          _parent				= nullptr;
	public:
                                    LuaBehaviorScript(const char* file);
        virtual						~LuaBehaviorScript(void)=default;
        virtual TYPE				getType(void) const override;
        LuaBehaviorScript*			getParent(void) const;
        LuaBehaviorScriptAttrib*	getAttrib(void);
        void						setParent(LuaBehaviorScript* const parent);
        void                        cacheTriggers(lua_State*const state);
	};
}

namespace OOLUA
{
    typedef elysian::LuaBehaviorScriptAttrib    LuaBehaviorScriptAttrib;
    typedef elysian::LuaScript                  LuaScript;
    typedef elysian::LuaBehaviorScript          LuaBehaviorScript;
}

OOLUA_PROXY(LuaBehaviorScriptAttrib)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC(inheritFromBehavior)
    OOLUA_MFUNC(addSupportedTrigger)
OOLUA_PROXY_END

OOLUA_PROXY(LuaScript)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(const char*, getPath)
    OOLUA_MEM_FUNC_CONST(unsigned, getRefCount)
    OOLUA_MEM_FUNC_CONST(bool, getError)
    //OOLUA_MEM_FUNC(Lua_table_ref&, getEnvironTable)
OOLUA_PROXY_END

OOLUA_PROXY(LuaBehaviorScript, LuaScript)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC_CONST(maybe_null<LuaBehaviorScript*>, getParent)
    OOLUA_MEM_FUNC(LuaBehaviorScriptAttrib*, getAttrib)
OOLUA_PROXY_END





//====== INLINEZ ========

namespace elysian {
    inline LuaBehaviorScript::LuaBehaviorScript(const char* file): LuaScript(file) {}
    inline auto	LuaBehaviorScript::getType(void) const -> TYPE { return TYPE_BEHAVIOR; }
    inline LuaBehaviorScript* LuaBehaviorScript::getParent(void) const { return _parent; }
    inline LuaBehaviorScriptAttrib*	LuaBehaviorScript::getAttrib(void) { return &_attrib; }
    inline void	LuaBehaviorScript::setParent(LuaBehaviorScript* const parent) { _parent = parent; }

    inline uint64_t LuaBehaviorScriptAttrib::getSupportedTriggers(void) const { return _triggers; }
    inline const char* LuaBehaviorScriptAttrib::getParentPath(void) const { return _parentPath; }
    inline void LuaBehaviorScriptAttrib::addSupportedTrigger(const Behavior::TRIGGER trigger) { _triggers |= trigger; }
    inline void LuaBehaviorScriptAttrib::inheritFromBehavior(const char*const path) { _hasParent = true; strcpy(_parentPath, path); }
    inline bool LuaBehaviorScriptAttrib::hasParent(void) const { return _hasParent; }

    inline const char *LuaScript::getPath(void) const { return _path; }
    inline unsigned LuaScript::getRefCount(void) const { return _refCount; }
    inline void LuaScript::incRefCount(void) { ++_refCount; }
    inline void LuaScript::decRefCount(void) { --_refCount; }
    inline int LuaScript::getEnvironRef(void) const { return _environRef; }
    inline void LuaScript::setEnvironRef(const int ref) { _environRef = ref; }
    inline bool LuaScript::getError(void) const { return _error; }
    inline void LuaScript::setError(const bool err) { _error = err; }


    inline auto LuaScript::getType(void) const -> TYPE { return TYPE_SCRIPT; }
}


#endif
