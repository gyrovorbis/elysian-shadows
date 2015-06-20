#include "elysian_lua_behavior.hpp"
#include "elysian_lua_wrappers.hpp"
#include <proxy/elysian_area_proxy.hpp>

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

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(LuaBehavior)

LuaBehavior::LuaBehavior(Entity &entity,
                         const char *
						 script,
						 const OOLUA::Lua_table_ref paramTable):
                         Behavior(entity)
{
	loadScript(script, paramTable);
}
	
LuaBehavior::~LuaBehavior(void) {
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

bool LuaBehavior::loadScript(const char *file, const OOLUA::Lua_table_ref paramTable) {
    dbgprintf("LuaBehavior::loadScript(%s)", file);

    dbgpush();
	if(_script) {
        Debug::printf(Debug::DBG_WARNING, "Already has a script loaded!");
        dbgpop();
		return false;
	}

	_script = LuaManager::loadBehaviorScript(file);

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
            OOLUA::push(*_mainState, this);                                 //push parent entity
            lua_setfield(*_mainState, -2, "selfBehav");                     //selfTable.selfBeahv
		}

		_script = _scriptHierarchy[curIndex];

		//add script triggers to behavior default triggers
		addTriggers(_script->getAttrib()->getSupportedTriggers());

		if(_triggerBits & ON_CREATE) {
            dbgprintf("LuaBehavior::onCreate(%s)", _script->getPath());
            dbgpush();
            if(paramTable.ref() != LUA_REFNIL) //Call trigger with custom parameter table
                _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_CREATE_INDEX, _ooSelfTableRef, paramTable));
			//call basic trigger
			else onCreate();
            dbgpop();
		}

		parentSelfTableRef = selfTableRef;
	}

    (void)paramTable;

    dbgpop();
	return true;
}

void LuaBehavior::_selfTableDump(void) {
	char selfDisplay[20] = {0};
	Debug::luaPrint("**** BEHAVIOR SELF TABLE DUMP ****");

	for(int i = _hierarchyDepth-1; i >= 0; --i) {
		sprintf(selfDisplay, "self[%d]", _hierarchyDepth-1-i);
        rPrintByRef(_ooSelfTableRefHierarchy[i].ref(), 0, selfDisplay);
	}
}

void LuaBehavior::onUpdate(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_UPDATE_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onRender(void) {

}

void LuaBehavior::onCreate(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_CREATE_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onDestroy(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_DESTROY_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onLoad(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_LOAD_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onUnload(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_UNLOAD_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onEnter(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_ENTER_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onExit(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_EXIT_INDEX, _ooSelfTableRef));
}
	
void LuaBehavior::onCollide(CollisionContact*const contact, elysian::Collidable *collidable) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COLLIDE_INDEX, _ooSelfTableRef, contact, collidable));
}

void LuaBehavior::onCollisionEnter(CollisionContact*const contact, elysian::Collidable *collidable) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COLLISION_ENTER_INDEX, _ooSelfTableRef, contact, collidable));
}

void LuaBehavior::onCollisionExit(elysian::Collidable *collidable) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COLLISION_EXIT_INDEX, _ooSelfTableRef, collidable));
}

void LuaBehavior::onShatter(CollisionContact* contact) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_SHATTER_INDEX, _ooSelfTableRef, contact));
}

void LuaBehavior::onExamine(Entity* entity) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_EXAMINE_INDEX, _ooSelfTableRef, entity));
}

void LuaBehavior::onDialog(const unsigned screen, const int option) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_DIALOG_INDEX, _ooSelfTableRef, screen, option));
}

void LuaBehavior::onCombatEnter(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_ENTER_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onCombatExit(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_EXIT_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onCombatDeath(void) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_DEATH_INDEX, _ooSelfTableRef));
}

void LuaBehavior::onCombatReceiveStatusModifier(CombatStatus*const modifier) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_RECEIVE_STATUS_MODIFIER_INDEX, _ooSelfTableRef, modifier));
}

void LuaBehavior::onCombatTargetCollisionEnter(CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_COMBAT_TARGET_COLLISION_ENTER_INDEX, _ooSelfTableRef, contact, collidable, modifier));
}

void LuaBehavior::onButtonPress(const GameInput::BUTTON button) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_BUTTON_PRESS_INDEX, _ooSelfTableRef, button));
}

void LuaBehavior::onButtonRelease(const GameInput::BUTTON button) {
    _PROTECTED(_callTriggerByIndex<void>(Behavior::ON_BUTTON_RELEASE_INDEX, _ooSelfTableRef, button));
}

void LuaBehavior::onTkSelect(void) {
    _PROTECTED(_callTriggerByName<void>("onTkSelect", _ooSelfTableRef));
}

void LuaBehavior::onTkUnselect(void) {
    _PROTECTED(_callTriggerByName<void>("onTkUnselect", _ooSelfTableRef));
}

}
