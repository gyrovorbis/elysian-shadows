#include "elysian_lua_behavior.hpp"
//#include <initializer_list>

#ifdef ELYSIAN_ENGINE
#	include "elysian_lua_wrappers.hpp"
#endif 

namespace elysian {


template<typename RType, typename... PTypes>
RType LuaBehavior::_callLuaFunction(const char* funcName, PTypes... parameters) const {
    /*if(!_script || _script->getError()) return;

	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, funcName);								//retrieve ENV.funcName()
	lua_remove(*_mainState, -2);											//remove ENV

	auto list = { (OOLUA::push2lua(*_mainState, parameters))... };


	_protectedCall(0, 1, _script);											//call funcName()
	_dataTableRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);				//store ref to return table
    _ooDataTableRef.set_ref(*_mainState, _dataTableRef);*/
}


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
	//    - Delete selfTable
	for(unsigned i = 0; i < _hierarchyDepth; ++i) {
		_script = _scriptHierarchy[i];
		_selfTableRef = _selfTableRefHierarchy[i];
		if(_script->getAttrib()->getSupportedTriggers() & ON_DESTROY) onDestroy();
		luaL_unref(*_mainState, LUA_REGISTRYINDEX, _selfTableRefHierarchy[i]);
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
		_selfTableRef = luaL_ref(*_mainState, LUA_REGISTRYINDEX);			//store in registry
		_selfTableRefHierarchy[curIndex] = _selfTableRef;

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
		parentSelfTableRef = _selfTableRef;
	}

#endif
	return true;
}

// CAN'T THIS SHIT ALL BE TEMPLATED?!!?!

void LuaBehavior::onUpdate(void) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onUpdate");								//get ENV.onUpdate()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
	_protectedCall(1, 0, _script);											//call onUpdate(dataTable)
#endif
}

void LuaBehavior::onRender(void) {
#ifdef ELYSIAN_ENGINE

#endif
}

void LuaBehavior::onCreate(void) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onCreate");								//get ENV.onCreate()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push selfTable arg
	_protectedCall(1, 0, _script);											//call onCreate(selfTable)
	
#endif
}

void LuaBehavior::onDestroy(void) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onDestroy");								//get ENV.onDestroy()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
	_protectedCall(1, 0, _script);		
#endif
}

void LuaBehavior::onLoad(void) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onLoad");								//get ENV.onLoad()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
	_protectedCall(1, 0, _script);		
#endif
}

void LuaBehavior::onUnload(void) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onUnload");								//get ENV.onUnload()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
	_protectedCall(1, 0, _script);		
#endif
}

void LuaBehavior::onEnter(void) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onEnter");								//get ENV.onEnter()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
	_protectedCall(1, 0, _script);		
#endif
}

void LuaBehavior::onExit(void) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onExit");								//get ENV.onExit()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
	_protectedCall(1, 0, _script);		
#endif
}
	
void LuaBehavior::onCollide(elysian::Collidable *collidable) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onCollide");								//get ENV.onCollide()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
    OOLUA::push(*_mainState, collidable);
	_protectedCall(2, 0, _script);											//call onUpdate(dataTable)
#endif
}

void LuaBehavior::onCollisionEnter(elysian::Collidable *collidable) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onCollisionEnter");								//get ENV.onCollide()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
    OOLUA::push(*_mainState, collidable);
	_protectedCall(2, 0, _script);											//call onUpdate(dataTable)
#endif
}

void LuaBehavior::onCollisionExit(elysian::Collidable *collidable) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onCollisionExit");						//get ENV.onCollide()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
    OOLUA::push(*_mainState, collidable);
	_protectedCall(2, 0, _script);											//call onUpdate(dataTable)
#endif
}

void LuaBehavior::onExamine(Entity* entity) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onExamine");								//get ENV.onExamine()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
    OOLUA::push(*_mainState, &entity);
	_protectedCall(2, 0, _script);											//call onUpdate(dataTable)
#endif
}

void LuaBehavior::onDialog(const unsigned screen, const int option) {
#ifdef ELYSIAN_ENGINE
	if(!_script || _script->getError()) return;
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _script->getEnvironRef());	//push ENV
	lua_getfield(*_mainState, -1, "onDialog");								//get ENV.onExamine()
	lua_remove(*_mainState, -2);											//remove ENV
	lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, _selfTableRef);				//push dataTableRef arg
    OOLUA::push(*_mainState, screen);
    OOLUA::push(*_mainState, option);
	_protectedCall(2, 0, _script);											//call onUpdate(dataTable)
#endif
}


}
