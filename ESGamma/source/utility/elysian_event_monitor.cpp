#include <ESGamma/utility/elysian_event_monitor.hpp>
#include <ESGamma/proxy/elysian_event_monitor_proxy.hpp>

namespace elysian {

EventMonitor EventMonitor::_nullMonitor;

void LuaEventCallback::onFinish(void) {
    EventMonitor::onFinish();
	_callback.push(*_mainState);
	_table.push(*_mainState);
	ELYSIAN_LUA_MANAGER_PROTECTED_CALL(1, 0, nullptr);
}

}
