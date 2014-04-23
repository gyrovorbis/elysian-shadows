#ifndef ELYSIAN_SUBPROCESS_MANAGER_HPP
#define ELYSIAN_SUBPROCESS_MANAGER_HPP

#include "elysian_event_monitor.hpp"

namespace elysian {

// SubSubProcess manager
template<typename P, class... _Types> class SubProcessManager;

template <typename P, typename M>
class SubProcess {
	friend  M;
public:
	enum STATE { STATE_IDLE, STATE_RUNNING };

	inline void start(M*const m) {
		static_cast<P*>(this)->start(m);
	}

	inline void update(M*const m) {
		static_cast<P*>(this)->update(m);
	}

	inline void stop(M*const m) {
		static_cast<P*>(this)->stop(m);
	}


private:
	STATE _state = STATE_IDLE;
};

template <typename P, typename M>
class MonitorableSubProcess: public SubProcess<P, M> {
public:
	enum STATE { STATE_IDLE, STATE_RUNNING };

	inline void _start(M*const m) {
		static_cast<P*>(this)->start(m);
		if(_monitor) _monitor->onStart();
	}

	inline void _update(M*const m) {
		static_cast<P*>(this)->update(m);
	}

	inline void _stop(M*const m) {
		static_cast<P*>(this)->stop(m);
		if(_monitor) _monitor->onFinish();
	}

	inline void setEventMonitor(EventMonitor*const monitor) {
		_monitor = (EventMonitor::isNull(monitor))? nullptr : monitor;
	}


private:
	EventMonitor* _monitor = nullptr; //may need more in the future... :(
};


// empty SubProcess manager
template<typename P> class SubProcessManager<P> {
public:
	inline void updateSubProcesses(void) {}

};

// recursive tuple definition
template<typename P, class _This,
		 class... _Rest>
  class SubProcessManager<P, _This, _Rest...>
  : public SubProcessManager<P, _Rest...>
{
	  friend _This;
public:
	_This _Myfirst;

protected:
	inline void updateSubSubProcesses(void) {
		_Myfirst._update(static_cast<P*>(this));
		SubProcessManager<P, _Rest...>::updateSubSubProcesses();
	}

};

  // tuple_element
  template<size_t _Index, class _ProcManager> struct SubProcessManagerSubProcessElement;

  // select first element
  template<typename P, class _This, class... _Rest>
  struct SubProcessManagerSubProcessElement<0, SubProcessManager<P, _This, _Rest...>>
  {
	typedef _This& type;
	typedef SubProcessManager<P, _This, _Rest...> _Ttype;
  };

  // recursive tuple_element definition
  template <size_t _Index, class _P, class _This, class... _Rest>
  struct SubProcessManagerSubProcessElement<_Index, SubProcessManager<_P, _This, _Rest...>>
	: public SubProcessManagerSubProcessElement<_Index - 1, SubProcessManager<_P, _Rest...> >
  {
  };


  // get reference to _Index element of tuple
  template<size_t _Index, typename _P, class... _Types> inline
	typename SubProcessManagerSubProcessElement<_Index, SubProcessManager<_P, _Types...>>::type
	get(SubProcessManager<_P, _Types...>& _SubProcessManager)
  {
	typedef typename SubProcessManagerSubProcessElement<_Index, SubProcessManager<_P, _Types...>>::_Ttype _Ttype;
	return (((_Ttype&) _SubProcessManager)._Myfirst);
  }


}

#endif // ELYSIAN_SubProcess_MANAGER_HPP
