#include <proxy/elysian_entity_proxy.hpp>
#include <crtp/elysian_debug.hpp>

#ifdef ELYSIAN_LUA_DEBUG
#	define _PROTECTED(a) \
		do {\
			if(!_script || _script->getError()) break; \
			a; \
			if(_script->getError()) _selfTableDump(); \
		} while(0)
#else
#	define _PROTECTED(a) (a)
#endif


namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(LuaBehaviorBase)

LuaBehaviorBase::LuaBehaviorBase(Entity &entity): Behavior(entity) {}

LuaBehaviorBase::LuaBehaviorBase(Entity &entity,
                         const char *script,
						 const OOLUA::Lua_table_ref paramTable):
                         Behavior(entity)
{
    setParamTable(paramTable);
    setBehavName(script);
    loadScript();
}

void LuaBehaviorBase::setBehavName(const char * const path) {
    if(_script) LuaManager::unloadBehaviorScript(*_script);
    _script = LuaManager::loadBehaviorScript(path);
}

void LuaBehaviorBase::setParamTable(OOLUA::Lua_table_ref ref) {
    if(ref.valid()) {
        _paramTableRef = ref;
    } else {
        lua_newtable(*LuaManager::_mainState);
        _paramTableRef.pull(*LuaManager::_mainState);
    }
}

	
LuaBehaviorBase::~LuaBehaviorBase(void) {
	//Traverse hierarchy from BOTTOM TO TOP cleaning shit up
	//    - Call onDestroy if script implements it (NOT if behavior uses it)
	//    - OOLUA will delete selfTables automatically

    if(_script) {
        for(unsigned i = 0; i < _hierarchyDepth; ++i) {
            _script = _scriptHierarchy[i];
            _ooSelfTableRef = &_ooSelfTableRefHierarchy[i];
            if(_script->getAttrib()->getSupportedTriggers() & ON_DESTROY) onDestroy();
        }

        LuaManager::unloadBehaviorScript(*_script);
    }
}

bool LuaBehaviorBase::loadScript(void) {
    DbgEvent event("LuaBehavior::loadScript(%s)", getBehavName());

    if(!_script) {
        errprintf("Could not load script!");
        return false;
    }

    if(_script->getError()) {
        warnprintf("Adding dead Lua script in error state!");
    }

	//Climb hierarchy and create top-down list
	for(auto* script = _script;
		script != nullptr;
		script = script->getParent())
	{
		_scriptHierarchy[_hierarchyDepth++] = script;
	}

	//Iterate over hierarchy from parent to child order
	//Should we ever store the selfTable hierarchy?
	int parentSelfTableRef = LUA_NOREF;
	for(int curIndex = _hierarchyDepth-1; curIndex >= 0; --curIndex) {

		lua_newtable(*_mainState);											//create new selfTable
		lua_pushvalue(*_mainState, -1);										//duplicate on stack
		int selfTableRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);			//store in registry
		//_selfTableRefHierarchy[curIndex] = _selfTableRef;
		_ooSelfTableRefHierarchy[curIndex].set_ref(*_mainState, selfTableRef);
		_ooSelfTableRef = &_ooSelfTableRefHierarchy[curIndex];


		if(parentSelfTableRef != LUA_NOREF) { 	//inherit parent's self table if child

			lua_newtable(*_mainState);										//metatable
			lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, parentSelfTableRef);	//parentSelfTable
			lua_setfield(*_mainState, -2, "__index");						//metatable[__index] = parentSelfTable;
			lua_setmetatable(*_mainState, -2);								//setmetatable(selfTable, metatable);

        } else { //initialize self.selfEntity and self.selfBehav if parent

			OOLUA::push(*_mainState, getEntity());							//push parent entity
            lua_setfield(*_mainState, -2, "selfEntity");					//selfTable.selfBehav = entity
            OOLUA::push(*_mainState, static_cast<LuaBehavior*>(this));                                 //push parent entity
            lua_setfield(*_mainState, -2, "selfBehav");                     //selfTable.selfBeahv
		}

		_script = _scriptHierarchy[curIndex];

		//add script triggers to behavior default triggers
		addTriggers(_script->getAttrib()->getSupportedTriggers());

		if(_triggerBits & ON_CREATE) {
            DbgEvent event("LuaBehavior::onCreate(%s)", _script->getPath());
            if(_paramTableRef.ref() != LUA_REFNIL) //Call trigger with custom parameter table
                _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_CREATE_INDEX, *_ooSelfTableRef, _paramTableRef));
			//call basic trigger
			else onCreate();
		}

		parentSelfTableRef = selfTableRef;
	}

	return true;
}

void LuaBehaviorBase::_selfTableDump(void) {
	char selfDisplay[20] = {0};
	Debug::luaPrint("**** BEHAVIOR SELF TABLE DUMP ****");

	for(int i = _hierarchyDepth-1; i >= 0; --i) {
		sprintf(selfDisplay, "self[%d]", _hierarchyDepth-1-i);
        LuaManager::rPrintByRef(_ooSelfTableRefHierarchy[i].ref(), 1, selfDisplay);
	}
}

void LuaBehaviorBase::onUpdate(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_UPDATE_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onRender(void) {

}

void LuaBehaviorBase::onCreate(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_CREATE_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onDestroy(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_DESTROY_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onLoad(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_LOAD_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onUnload(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_UNLOAD_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onEnter(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_ENTER_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onExit(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_EXIT_INDEX, *_ooSelfTableRef));
}
	
void LuaBehaviorBase::onCollide(CollisionContact*const contact, elysian::Collidable *collidable) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COLLIDE_INDEX, *_ooSelfTableRef, contact, collidable));
}

void LuaBehaviorBase::onCollisionEnter(CollisionContact*const contact, elysian::Collidable *collidable) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COLLISION_ENTER_INDEX, *_ooSelfTableRef, contact, collidable));
}

void LuaBehaviorBase::onCollisionExit(elysian::Collidable *collidable) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COLLISION_EXIT_INDEX, *_ooSelfTableRef, collidable));
}

void LuaBehaviorBase::onShatter(CollisionContact* contact) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_SHATTER_INDEX, *_ooSelfTableRef, contact));
}

void LuaBehaviorBase::onExamine(Entity* entity) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_EXAMINE_INDEX, *_ooSelfTableRef, entity));
}

void LuaBehaviorBase::onDialog(const unsigned screen, const int option) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_DIALOG_INDEX, *_ooSelfTableRef, screen, option));
}

void LuaBehaviorBase::onCombatEnter(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_ENTER_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onCombatExit(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_EXIT_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onCombatDeath(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_DEATH_INDEX, *_ooSelfTableRef));
}

void LuaBehaviorBase::onCombatReceiveStatusModifier(CombatStatus*const modifier) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_RECEIVE_STATUS_MODIFIER_INDEX, *_ooSelfTableRef, modifier));
}

void LuaBehaviorBase::onCombatTargetCollisionEnter(CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_TARGET_COLLISION_ENTER_INDEX, *_ooSelfTableRef, contact, collidable, modifier));
}

/*
void LuaBehaviorBase::onButtonPress(const GameInput::BUTTON button) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_BUTTON_PRESS_INDEX, _ooSelfTableRef, button));
}

void LuaBehaviorBase::onButtonRelease(const GameInput::BUTTON button) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_BUTTON_RELEASE_INDEX, _ooSelfTableRef, button));
}*/

void LuaBehaviorBase::onTkSelect(void) {
    _PROTECTED(_callTriggerByName<void>("onTkSelect", *_ooSelfTableRef));
}

void LuaBehaviorBase::onTkUnselect(void) {
    _PROTECTED(_callTriggerByName<void>("onTkUnselect", *_ooSelfTableRef));
}

}
