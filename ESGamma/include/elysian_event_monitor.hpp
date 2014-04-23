#ifndef ELYSIAN_EVENT_MONITOR_HPP
#define ELYSIAN_EVENT_MONITOR_HPP

#include <functional>

namespace elysian {


class EventMonitor {
private:
	static EventMonitor		_nullMonitor;
public:
	static bool				isNull(const EventMonitor*const mon);
	static EventMonitor*	null(void);
	virtual void			onStart(void);
	virtual void			onFinish(void);
};

class EventCallback: public EventMonitor {
private:
	std::function<void(void)> _callback = nullptr;
public:

	void			setCallbackFunction(const std::function<void(void)> func);
	virtual void	onFinish(void) override;
};


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
