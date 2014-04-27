#include "elysian_event_monitor.hpp"
#include "elysian_lua_wrappers.hpp"

namespace elysian {

EventMonitor EventMonitor::_nullMonitor;

void LuaEventCallback::onFinish(void) {
	_callback.push(*_mainState);
	_table.push(*_mainState);
	_protectedCall(1, 0, nullptr);
}

void LuaEventBarrier::start(void) {
	lua_rawgeti(_coroutine, LUA_REGISTRYINDEX, _function.ref());
	//OOLUA::push(_coroutine, this);
	lua_resume(_coroutine, *_mainState, 0);
}

inline void LuaEventBarrier::onFinish(void) {
	lua_resume(_coroutine, *_mainState, 0);

	//_completed = true;
}

}
