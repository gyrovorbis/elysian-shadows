#ifndef ELYSIAN_LUA_BEHAVIOR_HPP
#define ELYSIAN_LUA_BEHAVIOR_HPP

#include "elysian_behavior.hpp"
#ifdef ELYSIAN_ENGINE
#	include "elysian_lua_manager.hpp"
#endif

#define ELYSIAN_LUA_BEHAVIOR_NAME "LuaBehavior"

namespace elysian {

	class Entity;
	class LuaScript;
	class Collidable;

    class LuaBehavior: 
		public Behavior
#ifdef ELYSIAN_ENGINE
		,public LuaManager
#endif
	{
	friend class Entity;
	protected:
		LuaScript*			_script;
		int					_selfTableRef;
#ifdef ELYSIAN_ENGINE
		OOLUA::Lua_table_ref _ooSelfTableRef;
#endif
							LuaBehavior(Entity &entity, const char *script);
							~LuaBehavior(void);
	public:

		bool				loadScript(const char *file);
		
        template<typename RType, typename... PTypes>
        RType _callLuaFunction(const char* funcName, PTypes... parameters) const;
		
		//overridden from component
		virtual bool		load(std::ifstream &stream) override;
		virtual bool		save(std::ofstream &stream) override;
		
		//overriden from behavior
		virtual const char* getBehavName(void) const override;
		virtual void		onUpdate(void) override;
		virtual void		onRender(void) override;
		virtual void		onCreate(void) override;
		virtual void		onDestroy(void) override;
		virtual void		onLoad(void) override;
		virtual void		onUnload(void) override;
		virtual void		onEnter(void) override;
		virtual void		onExit(void) override;
		virtual void		onCollide(Collidable* const collidable) override;
		virtual void		onCollisionEnter(Collidable* const collidable) override;
		virtual void		onCollisionExit(Collidable* const collidable) override;
		virtual void		onExamine(Entity* entity) override;
		virtual void		onDialog(const unsigned screen, const int option) override;
		
		int					getSelfTableRef(void) const;
#ifdef ELYSIAN_ENGINE
		OOLUA::Lua_table_ref& getSelfTable(void) { return _ooSelfTableRef; }
#endif
    };


	//======= INLINEZ ======
	
	inline const char* LuaBehavior::getBehavName(void) const { return ELYSIAN_LUA_BEHAVIOR_NAME; }
	inline int LuaBehavior::getSelfTableRef(void) const { return _selfTableRef; }


	
}

#endif
