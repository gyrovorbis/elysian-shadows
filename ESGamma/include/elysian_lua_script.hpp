#ifndef ELYSIAN_LUA_SCRIPT_H
#define ELYSIAN_LUA_SCRIPT_H

#include "elysian_behavior.hpp"

#define ELYSIAN_LUA_SCRIPT_PATH_SIZE 200

namespace elysian {
	
	/*
	  Abstraction for a single lua script file and its ENV
	*/
	class LuaScript {
	protected:
		char _path[ELYSIAN_LUA_SCRIPT_PATH_SIZE];
		unsigned _refCount;
		int _environRef;
		bool _error;

	public:
		enum TYPE { TYPE_SCRIPT, TYPE_BEHAVIOR };
		
		LuaScript(const char *path);
		virtual ~LuaScript(void);
		
		const char *getPath(void) const;
		unsigned getRefCount(void) const;
		void incRefCount(void);
		void decRefCount(void);
		int getEnvironRef(void) const;
		void setEnvironRef(const int ref);
		bool getError(void);
		void setError(const bool err=true);

		virtual TYPE getType(void) const { return TYPE_SCRIPT; }
	};

	//Supplied by Lua
	class LuaBehaviorScriptAttrib {
	private:
		uint64_t	_triggers				= 0;
		char		_parentPath[200]		= {0};
		bool		_hasParent				= false;
	public:
		uint64_t getSupportedTriggers(void) const { return _triggers; }
		const char* getParentPath(void) const { return _parentPath; }
		void addSupportedTrigger(const Behavior::TRIGGER trigger) { _triggers |= trigger; }
		void inheritFromBehavior(const char*const path) { _hasParent = true; strcpy(_parentPath, path); }
		bool hasParent(void) const { return _hasParent; }
	};

	class LuaBehaviorScript: public LuaScript {
	private:
		LuaBehaviorScriptAttrib	_attrib;
		LuaBehaviorScript*		_parent				= nullptr;
	public:
									LuaBehaviorScript(const char* file): LuaScript(file) {}
		virtual						~LuaBehaviorScript(void) {}
		virtual TYPE				getType(void) const override { return TYPE_BEHAVIOR; }
		LuaBehaviorScript*			getParent(void) const { return _parent; }
		LuaBehaviorScriptAttrib*	getAttrib(void) { return &_attrib; }
		void						setParent(LuaBehaviorScript* const parent) { _parent = parent; }

	};
	
	inline const char *LuaScript::getPath(void) const { return _path; }
	inline unsigned LuaScript::getRefCount(void) const { return _refCount; }
	inline void LuaScript::incRefCount(void) { ++_refCount; }
	inline void LuaScript::decRefCount(void) { --_refCount; }
	inline int LuaScript::getEnvironRef(void) const { return _environRef; }
	inline void LuaScript::setEnvironRef(const int ref) { _environRef = ref; }
	inline bool LuaScript::getError(void) { return _error; }
	inline void LuaScript::setError(const bool err) { _error = err; }
}


#endif
