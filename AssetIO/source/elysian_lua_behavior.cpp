#include "elysian_lua_behavior.hpp"
//#include <initializer_list>

#ifdef ELYSIAN_ENGINE
#	include "elysian_lua_wrappers.hpp"
#endif 

#ifdef ELYSIAN_LUA_DEBUG
#	define _PROTECTED(a) \
		do {\
			if(!_script || _script->getError()) return; \
			a; \
			if(_script->getError()) _selfTableDump(); \
		} while(0)
#else
#	define _PROTECTED(a) (a)
#endif


namespace elysian {

LuaBehavior::LuaBehavior(Entity &entity,
                         const char *
#ifdef ELYSIAN_ENGINE
                         script
#endif
                         ):
	Behavior(entity)
{
#ifdef ELYSIAN_ENGINE

	loadScript(script);
	
#endif

}
	
LuaBehavior::~LuaBehavior(void) {
#ifdef ELYSIAN_ENGINE

	//Traverse hierarchy from BOTTOM TO TOP cleaning shit up
	//    - Call onDestroy if script implements it (NOT if behavior uses it)
	//    - OOLUA will delete selfTables automatically
	for(unsigned i = 0; i < _hierarchyDepth; ++i) {
		_script = _scriptHierarchy[i];
		_ooSelfTableRef = &_ooSelfTableRefHierarchy[i];
		if(_script->getAttrib()->getSupportedTriggers() & ON_DESTROY) onDestroy();
	}

	LuaManager::unloadBehaviorScript(*_script);
#endif
}

bool LuaBehavior::load(std::ifstream &stream) {
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
}

bool LuaBehavior::save(std::ofstream &stream) {
    dbgprintf(LOG_VERBOSE, "LuaBehavior::Save(ofstream&)");
    dbgPush();
    
    Behavior::save(stream);

#ifdef ELYSIAN_ENGINE

	
#endif
    dbgPop();
	return true;
}

bool LuaBehavior::loadScript(const char *file) {
	dbgprintf(LOG_VERBOSE, "LuaBehavior::loadScript(%s)", file);
    dbgPush();
	if(_script) {
		dbgprintf(LOG_CRITICAL, "Already has a script loaded!");
		return false;
	}

#ifdef ELYSIAN_ENGINE
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

		} else { //initialize self.selfEntity if parent

			OOLUA::push(*_mainState, getEntity());							//push parent entity
			lua_setfield(*_mainState, -2, "selfEntity");					//selfTable.selfEntity = entity
		}

		_script = _scriptHierarchy[curIndex];

		//add script triggers to behavior default triggers
		addTriggers(_script->getAttrib()->getSupportedTriggers());

		if(_triggerBits & ON_CREATE) onCreate();
		parentSelfTableRef = selfTableRef;
	}

#endif
	return true;
}

#ifdef ELYSIAN_ENGINE
void LuaBehavior::_selfTableDump(void) {
	char selfDisplay[20] = {0};
	Debug::luaPrint("**** BEHAVIOR SELF TABLE DUMP ****");

	for(int i = _hierarchyDepth-1; i >= 0; --i) {
		sprintf(selfDisplay, "self[%d]", _hierarchyDepth-1-i);
		_rPrintByRef(_ooSelfTableRefHierarchy[i].ref(), 0, selfDisplay);
	}
}
#endif

void LuaBehavior::onUpdate(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onUpdate", _ooSelfTableRef));
#endif
}

void LuaBehavior::onRender(void) {
#ifdef ELYSIAN_ENGINE

#endif
}

void LuaBehavior::onCreate(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCreate", _ooSelfTableRef));
#endif
}

void LuaBehavior::onDestroy(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onDestroy", _ooSelfTableRef));
#endif
}

void LuaBehavior::onLoad(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onLoad", _ooSelfTableRef));
#endif
}

void LuaBehavior::onUnload(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onUnload", _ooSelfTableRef));
#endif
}

void LuaBehavior::onEnter(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onEnter", _ooSelfTableRef));
#endif
}

void LuaBehavior::onExit(void) {
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onExit", _ooSelfTableRef));
#endif
}
	
void LuaBehavior::onCollide(elysian::Collidable *collidable) {
	(void)collidable;
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCollide", _ooSelfTableRef, collidable));
#endif
}

void LuaBehavior::onCollisionEnter(elysian::Collidable *collidable) {
	(void)collidable;
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCollisionEnter", _ooSelfTableRef, collidable));
#endif
}

void LuaBehavior::onCollisionExit(elysian::Collidable *collidable) {
	(void)collidable;
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCollisionExit", _ooSelfTableRef, collidable));
#endif
}

void LuaBehavior::onExamine(Entity* entity) {
	(void)entity;
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onExamine", _ooSelfTableRef, entity));
#endif
}

void LuaBehavior::onDialog(const unsigned screen, const int option) {
	(void)screen;
	(void)option;
#ifdef ELYSIAN_ENGINE
	_PROTECTED(_callTrigger<void>("onCollide", screen, option));
#endif
}


}
