#ifndef ELYSIAN_EVENT_MONITOR_HPP
#define ELYSIAN_EVENT_MONITOR_HPP

#include <functional>
#include "elysian_lua_manager.hpp"

namespace elysian {


class EventMonitor {
private:
	static EventMonitor		_nullMonitor;
public:
	static bool				isNull(const EventMonitor*const mon);
	static EventMonitor*	null(void);
	virtual void			onStart(void);
	virtual void			onFinish(void);
	virtual					~EventMonitor(void) = default;
};

class EventCallback: public EventMonitor {
private:
	std::function<void(void)> _callback = nullptr;
public:

	void			setCallbackFunction(const std::function<void(void)> func);
	virtual void	onFinish(void) override;
};

class LuaEventMonitor: public EventMonitor, public LuaManager {
protected:
		OOLUA::Lua_table_ref	_table;
public:
		void					setTable(OOLUA::Lua_table_ref& table);
		OOLUA::Lua_table_ref&	getTable(void);
};



class LuaEventCallback: public LuaEventMonitor {
private:
	OOLUA::Lua_func_ref		_callback;

public:
					LuaEventCallback(OOLUA::Lua_func_ref& callback);
	void			setCallbackFunction(OOLUA::Lua_func_ref& callback);
	virtual void	onFinish(void) override;
};


inline void LuaEventMonitor::setTable(OOLUA::Lua_table_ref& table) { _table = table; }
inline OOLUA::Lua_table_ref& LuaEventMonitor::getTable(void) { return _table; }

inline LuaEventCallback::LuaEventCallback(OOLUA::Lua_func_ref &callback):
	_callback(callback) {}
inline void LuaEventCallback::setCallbackFunction(OOLUA::Lua_func_ref &callback) { _callback = callback; }



inline bool EventMonitor::isNull(const EventMonitor*const mon) { return (mon == &_nullMonitor); }
inline EventMonitor* EventMonitor::null(void) { return &_nullMonitor; }
inline void EventMonitor::onStart(void) { }
inline void EventMonitor::onFinish(void) { }

inline void EventCallback::setCallbackFunction(const std::function<void(void)> func) {
	_callback = func;
}

inline void EventCallback::onFinish(void) { _callback(); }

}

#endif // ELYSIAN_EVENT_MONITOR_HPP
