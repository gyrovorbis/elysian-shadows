#include "elysian_lua_behavior.hpp"
#include "elysian_lua_wrappers.hpp"

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
	for(unsigned i = 0; i < _hierarchyDepth; ++i) {
		_script = _scriptHierarchy[i];
		_ooSelfTableRef = &_ooSelfTableRefHierarchy[i];
		if(_script->getAttrib()->getSupportedTriggers() & ON_DESTROY) onDestroy();
	}

	LuaManager::unloadBehaviorScript(*_script);
}

bool LuaBehavior::load(FILE* /*file*/) {
#if 0
    dbgprintf(LOG_VERBOSE, "LuaBehavior::Load(ifstream&)");
    dbgPush();
	bool success = true;
    
    success &= Behavior::load(stream);
	
#ifdef ELYSIAN_ENGINE
	char scriptName[ELYSIAN_LUA_SCRIPT_PATH_SIZE];
	stream >> scriptName; 

	success &= loadScript(scriptName);
	
#endif
    dbgPop();
	return success;
#endif
	return true;
}

bool LuaBehavior::save(FILE* /*file*/) {
#if 0
    dbgprintf(LOG_VERBOSE, "LuaBehavior::Save(ofstream&)");
    dbgPush();
    
    Behavior::save(stream);

#ifdef ELYSIAN_ENGINE

	
#endif
    dbgPop();
	return true;
#endif
	return true;
}

bool LuaBehavior::loadScript(const char *file, const OOLUA::Lua_table_ref paramTable) {
	dbgprintf(LOG_VERBOSE, "LuaBehavior::loadScript(%s)", file);
    dbgPush();
	if(_script) {
		dbgprintf(LOG_CRITICAL, "Already has a script loaded!");
        dbgPop();
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
			if(paramTable.ref() != LUA_REFNIL) //Call trigger with custom paramter table
				_PROTECTED(_callTrigger<void>("onCreate", _ooSelfTableRef, paramTable));
			//call basic trigger
			else onCreate();
		}

		parentSelfTableRef = selfTableRef;
	}

    (void)paramTable;

    dbgPop();
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
	_PROTECTED(_callTrigger<void>("onUpdate", _ooSelfTableRef));
}

void LuaBehavior::onRender(void) {

}

void LuaBehavior::onCreate(void) {
	_PROTECTED(_callTrigger<void>("onCreate", _ooSelfTableRef));
}

void LuaBehavior::onDestroy(void) {
	_PROTECTED(_callTrigger<void>("onDestroy", _ooSelfTableRef));
}

void LuaBehavior::onLoad(void) {
	_PROTECTED(_callTrigger<void>("onLoad", _ooSelfTableRef));
}

void LuaBehavior::onUnload(void) {
	_PROTECTED(_callTrigger<void>("onUnload", _ooSelfTableRef));
}

void LuaBehavior::onEnter(void) {
	_PROTECTED(_callTrigger<void>("onEnter", _ooSelfTableRef));
}

void LuaBehavior::onExit(void) {
	_PROTECTED(_callTrigger<void>("onExit", _ooSelfTableRef));
}
	
void LuaBehavior::onCollide(CollisionContact*const contact, elysian::Collidable *collidable) {
	(void)collidable;
    (void)contact;
	_PROTECTED(_callTrigger<void>("onCollide", _ooSelfTableRef, contact, collidable));
}

void LuaBehavior::onCollisionEnter(CollisionContact*const contact, elysian::Collidable *collidable) {
	(void)collidable;
    (void)contact;
	_PROTECTED(_callTrigger<void>("onCollisionEnter", _ooSelfTableRef, contact, collidable));
}

void LuaBehavior::onCollisionExit(elysian::Collidable *collidable) {
	(void)collidable;
	_PROTECTED(_callTrigger<void>("onCollisionExit", _ooSelfTableRef, collidable));
}

void LuaBehavior::onShatter(CollisionContact* contact) {
    (void)contact;
    _PROTECTED(_callTrigger<void>("onShatter", _ooSelfTableRef, contact));
}

void LuaBehavior::onExamine(Entity* entity) {
	(void)entity;
	_PROTECTED(_callTrigger<void>("onExamine", _ooSelfTableRef, entity));
}

void LuaBehavior::onDialog(const unsigned screen, const int option) {
	(void)screen;
	(void)option;
	_PROTECTED(_callTrigger<void>("onDialog", _ooSelfTableRef, screen, option));
}

void LuaBehavior::onCombatEnter(void) {
	_PROTECTED(_callTrigger<void>("onCombatEnter", _ooSelfTableRef));
}

void LuaBehavior::onCombatExit(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCombatExit", _ooSelfTableRef));
#endif
}

void LuaBehavior::onCombatDeath(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCombatDeath", _ooSelfTableRef));
#endif
}

void LuaBehavior::onCombatReceiveStatusModifier(CombatStatus*const modifier) {
	(void)modifier;
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCombatReceiveStatusModifier", _ooSelfTableRef, modifier));
#endif
}

void LuaBehavior::onCombatTargetCollisionEnter(CollisionContact*const contact, Collidable*const collidable, CombatStatus*const modifier) {
	(void)contact;
	(void)collidable;
	(void)modifier;
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCombatTargetCollisionEnter", _ooSelfTableRef, contact, collidable, modifier));
#endif
}


}
