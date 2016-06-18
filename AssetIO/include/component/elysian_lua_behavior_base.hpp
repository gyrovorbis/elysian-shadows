#ifndef ELYSIAN_LUA_BEHAVIOR_BASE_HPP
#define ELYSIAN_LUA_BEHAVIOR_BASE_HPP

#include <crtp/elysian_behavior.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>

#define ELYSIAN_LUA_BEHAVIOR_NAME				"LuaBehavior"
#define ELYSIAN_LUA_BEHAVIOR_INHERITANCE_DEPTH	5

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(Collidable)
ELYSIAN_ASSET_TYPEDEF(LuaBehavior)

namespace elysian {

	class LuaBehaviorScript;

    class LuaBehaviorBase:
            public Behavior,
            public LuaTableProxy<LuaBehaviorBase, LuaBehavior>  {
    friend LuaBehavior;
    friend class BaseEntity;
    friend Entity;

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
        RType _callTriggerByIndex(const uint32_t trigger, PTypes&&... parameters) {
            lua_rawgeti(*LuaManager::_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());
            lua_pushinteger(*LuaManager::_mainState, trigger);
            lua_gettable(*LuaManager::_mainState, -2);      //must invoke metamethods for behavior inheritance
            //lua_rawgeti(*_mainState, -1, trigger);
            lua_remove(*LuaManager::_mainState, -2);
            return LuaManager::_callFunction<RType>(_script, std::forward<PTypes>(parameters)...);
        }

        template<typename RType, typename... PTypes>
        RType _callTriggerByName(const char* funcName, PTypes&&... parameters) {
            return LuaManager::callGlobalFunction<RType>(funcName, _script, std::forward<PTypes>(parameters)...);
        }

        void				_selfTableDump(void);
    public:

                            LuaBehaviorBase(Entity& entity);
                            LuaBehaviorBase(Entity& entity, const LuaBehavior* rhs);
                            LuaBehaviorBase(Entity& entity, const char *script, const OOLUA::Lua_table_ref paramTable);
                            ~LuaBehaviorBase(void);

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
        //virtual void        onButtonPress(const GameInput::BUTTON button) override;
        //virtual void        onButtonRelease(const GameInput::BUTTON button) override;

        //ESTk triggers
        virtual void        onTkSelect(void) override;
        virtual void        onTkUnselect(void) override;
		

        void                setBehavName(const char*const path);

		int					getSelfTableRef(void) const;
        void                setParamTable(OOLUA::Lua_table_ref ref);
        LuaBehaviorScript*  getScript(void) const;

        OOLUA::Lua_table_ref getSelfTable(void);
        OOLUA::Lua_table_ref getParamTable(void);

    };


    //======= INLINEZ ======

    inline const char* LuaBehaviorBase::getBehavName(void) const {
        return (_script)? _script->getPath() : "none";
    }

    inline LuaBehaviorScript* LuaBehaviorBase::getScript(void) const { return _script; }

    inline OOLUA::Lua_table_ref LuaBehaviorBase::getSelfTable(void) { return _ooSelfTableRef? *_ooSelfTableRef : OOLUA::Lua_table_ref(); }

    inline OOLUA::Lua_table_ref LuaBehaviorBase::getParamTable(void) { return _paramTableRef; }

    inline int LuaBehaviorBase::getSelfTableRef(void) const { return _ooSelfTableRef->ref(); }


}

#endif
