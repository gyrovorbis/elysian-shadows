#ifndef ELYSIAN_LUA_SCRIPT_H
#define ELYSIAN_LUA_SCRIPT_H

#define ELYSIAN_LUA_SCRIPT_PATH_SIZE 200


namespace elysian {
	
	/*
	  Abstraction for a single lua script file and its ENV
	*/
	class LuaScript {
	private:
		char _path[ELYSIAN_LUA_SCRIPT_PATH_SIZE];
		unsigned _refCount;
		int _environRef;
		bool _error;

	public:
		
		LuaScript(const char *path);
		~LuaScript(void);
		
		const char *getPath(void) const;
		unsigned getRefCount(void) const;
		void incRefCount(void);
		void decRefCount(void);
		int getEnvironRef(void) const;
		void setEnvironRef(const int ref);
		bool getError(void);
		void setError(const bool err=true);
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