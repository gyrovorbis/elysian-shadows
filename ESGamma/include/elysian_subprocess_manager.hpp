#ifndef ELYSIAN_SUBPROCESS_MANAGER_HPP
#define ELYSIAN_SUBPROCESS_MANAGER_HPP

#include "elysian_event_monitor.hpp"

namespace elysian {

// SubSubProcess manager
template<typename P, class... _Types> class SubProcessManager;

enum PROC_STATE { PROC_STATE_IDLE, PROC_STATE_RUNNING };

template <typename P, typename M>
class SubProcess {
	friend  M;
public:

	inline void start(M*const m) {
		_state = PROC_STATE_RUNNING;
		static_cast<P*>(this)->_start(m);
	}

	inline void update(M*const m, const float deltaTime) {
		if(_state == PROC_STATE_RUNNING) static_cast<P*>(this)->_update(m, deltaTime);
	}

	inline void stop(M*const m) {
		static_cast<P*>(this)->_stop(m);
		_state = PROC_STATE_IDLE;
	}

	inline PROC_STATE getProcState(void) const { return _state; }


protected:
	PROC_STATE _state = PROC_STATE_IDLE;
};

template <typename P, typename M>
class MonitorableSubProcess: public SubProcess<P, M> {
public:

	inline void start(M*const m) {
		static_cast<SubProcess<P,M>*>(this)->start(m);
		if(_monitor && SubProcess<P,M>::_state == PROC_STATE_RUNNING) _monitor->onStart();
	}

	inline void update(M*const m, const float deltaTime) {
		static_cast<SubProcess<P,M>*>(this)->update(m, deltaTime);
	}

	inline void stop(M*const m) {
		static_cast<SubProcess<P,M>*>(this)->stop(m);
		if(_monitor && SubProcess<P,M>::_state == PROC_STATE_IDLE) _monitor->onFinish();
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
	inline void updateSubProcesses(const float /*deltaTime*/) {}

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
	inline void updateSubProcesses(const float deltaTime) {
		_Myfirst.update(static_cast<P*>(this), deltaTime);
		SubProcessManager<P, _Rest...>::updateSubProcesses(deltaTime);
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
