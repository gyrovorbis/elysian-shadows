#ifndef ELYSIAN_LUA_BEHAVIOR_HPP
#define ELYSIAN_LUA_BEHAVIOR_HPP

#include "elysian_behavior.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_asset_typedefs.hpp"

#define ELYSIAN_LUA_BEHAVIOR_NAME				"LuaBehavior"
#define ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH	5

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class LuaBehaviorScript;
	class Collidable;

    class LuaBehavior: public Behavior, public LuaManager {
    friend class BaseEntity;
	protected:
		LuaBehaviorScript*	_scriptHierarchy[ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH]		= { nullptr };
		//int					_selfTableRefHierarchy[ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH]	= { LUA_NOREF };
		LuaBehaviorScript*	_script = nullptr;
		//int					_selfTableRef = LUA_NOREF;

		unsigned			_hierarchyDepth = 0;

		OOLUA::Lua_table_ref _ooSelfTableRefHierarchy[ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH];
		OOLUA::Lua_table_ref* _ooSelfTableRef = nullptr;

		template<typename RType, typename... PTypes>
		RType _callTrigger(const char*const triggerName, PTypes... parameters) {
			return _callGlobalFunction<RType>(triggerName, _script, parameters...);
		}

		void				_selfTableDump(void);
	public:

      /*  ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(self, getSelfTable)
        ELYSIAN_LUA_PROXY_FIELDS_END()
*/
                            LuaBehavior(Entity &entity, const char *script, const OOLUA::Lua_table_ref paramTable);
                            virtual ~LuaBehavior(void) override;

		bool				loadScript(const char *file, const OOLUA::Lua_table_ref paramTable);
		
        template<typename RType, typename... PTypes>
        RType _callLuaFunction(const char* funcName, PTypes... parameters) const;
		
		//overridden from component
		virtual bool		load(FILE* file) override;
		virtual bool		save(FILE* file) override;
		
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
		virtual void		onCollide(CollisionContact*const contact, Collidable* const collidable) override;
		virtual void		onCollisionEnter(CollisionContact*const contact, Collidable* const collidable) override;
		virtual void		onCollisionExit(Collidable* const collidable) override;
        virtual void        onShatter(CollisionContact*const contact) override;
		virtual void		onExamine(Entity* entity) override;
		virtual void		onDialog(const unsigned screen, const int option) override;
		virtual void		onCombatEnter(void) override;
		virtual void		onCombatExit(void) override;
		virtual void		onCombatDeath(void) override;
		virtual void		onCombatReceiveStatusModifier(CombatStatus*const modifier) override;
		virtual void		onCombatTargetCollisionEnter(CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier) override;
		
		int					getSelfTableRef(void) const;

		OOLUA::Lua_table_ref& getSelfTable(void) { return *_ooSelfTableRef; }

    };


	//======= INLINEZ ======
	
    inline const char* LuaBehavior::getBehavName(void) const {
        return (_script)? _script->getPath() : nullptr;
    }
	//inline int LuaBehavior::getSelfTableRef(void) const { return _selfTableRef; }


	
}

#endif
