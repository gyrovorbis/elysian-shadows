#include "elysian_event_monitor.hpp"
#include "elysian_lua_wrappers.hpp"

namespace elysian {

EventMonitor EventMonitor::_nullMonitor;

void LuaEventCallback::onFinish(void) {
	_callback.push(*_mainState);
	_table.push(*_mainState);
	_protectedCall(1, 0, nullptr);
}

}
