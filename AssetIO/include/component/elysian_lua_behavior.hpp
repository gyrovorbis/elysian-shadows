#ifndef ELYSIAN_LUA_BEHAVIOR_HPP
#define ELYSIAN_LUA_BEHAVIOR_HPP

#include "elysian_behavior.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_lua_table_proxy.hpp"

#define ELYSIAN_LUA_BEHAVIOR_NAME				"LuaBehavior"
#define ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH	5

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(Collidable)

namespace elysian {

	class LuaBehaviorScript;

    class LuaBehavior:
            public LuaTableProxy<LuaBehavior, LuaBehavior>,
            public Behavior
    //        public LuaManager (inherited from LuaTableProxy already)
    {
    friend class BaseEntity;
    public:

    ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELD(path,                   getBehavName,       setBehavName)
        ELYSIAN_LUA_PROXY_FIELD(paramTable,             getParamTable,      setParamTable)
        ELYSIAN_LUA_PROXY_FIELD(triggers,               getTriggers,        setTriggers)
        ELYSIAN_LUA_PROXY_FIELD(selfTable,              getSelfTable)
        ELYSIAN_LUA_PROXY_FIELD(script,                 getScript)
    ELYSIAN_LUA_PROXY_FIELDS_END()



    protected:
        LuaBehaviorScript*	_scriptHierarchy[ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH]		= { nullptr };
        //int					_selfTableRefHierarchy[ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH]	= { LUA_NOREF };
        LuaBehaviorScript*	_script = nullptr;
        //int					_selfTableRef = LUA_NOREF;

        unsigned			_hierarchyDepth = 0;

        OOLUA::Lua_table_ref    _ooSelfTableRefHierarchy[ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH];
        OOLUA::Lua_table_ref*   _ooSelfTableRef = nullptr;
        OOLUA::Lua_table_ref    _paramTableRef;

        template<typename RType, typename... PTypes>
        RType _callTriggerByIndex(const uint64_t trigger, PTypes... parameters) {
            lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());
            lua_pushinteger(*_mainState, trigger);
            lua_gettable(*_mainState, -2);      //must invoke metamethods for behavior inheritance
            //lua_rawgeti(*_mainState, -1, trigger);
            lua_remove(*_mainState, -2);
            return LuaManager::_callFunction<RType>(_script, parameters...);
        }

        template<typename RType, typename... PTypes>
        RType _callTriggerByName(const char* funcName, PTypes... parameters) {
            return LuaManager::_callGlobalFunction<RType>(funcName, _script, parameters...);
        }

        void				_selfTableDump(void);
    public:

      /*  ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(self, getSelfTable)
        ELYSIAN_LUA_PROXY_FIELDS_END()
*/

                            LuaBehavior(Entity& entity);
                            LuaBehavior(Entity& entity, const char *script, const OOLUA::Lua_table_ref paramTable);
        virtual             ~LuaBehavior(void) override;

        bool				loadScript(void);
                template<typename RType, typename... PTypes>
        RType               _callLuaFunction(const char* funcName, PTypes... parameters) const;
		
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
        virtual void        onButtonPress(const GameInput::BUTTON button) override;
        virtual void        onButtonRelease(const GameInput::BUTTON button) override;

        //ESTk triggers
        virtual void        onTkSelect(void) override;
        virtual void        onTkUnselect(void) override;
		

        void                setBehavName(const char*const path);

		int					getSelfTableRef(void) const;
        void                setParamTable(const OOLUA::Lua_table_ref ref);
        LuaBehaviorScript*  getScript(void) const;

        OOLUA::Lua_table_ref& getSelfTable(void);
        OOLUA::Lua_table_ref getParamTable(void);

    };


    //======= INLINEZ ======

    inline const char* LuaBehavior::getBehavName(void) const {
        return (_script)? _script->getPath() : "";
    }

    inline LuaBehaviorScript* LuaBehavior::getScript(void) const { return _script; }

    inline OOLUA::Lua_table_ref& LuaBehavior::getSelfTable(void) { return *_ooSelfTableRef; }

    inline OOLUA::Lua_table_ref LuaBehavior::getParamTable(void) { return _paramTableRef; }

    inline void LuaBehavior::setParamTable(const OOLUA::Lua_table_ref ref) {
        _paramTableRef = ref;
    }

    inline int LuaBehavior::getSelfTableRef(void) const { return _ooSelfTableRef->ref(); }


}

#endif
