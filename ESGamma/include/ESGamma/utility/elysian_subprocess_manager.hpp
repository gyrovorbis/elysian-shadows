/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_SUBPROCESS_MANAGER_HPP
#define ELYSIAN_SUBPROCESS_MANAGER_HPP

#include <ESGamma/utility/elysian_event_monitor.hpp>
#include <AssetIO/utility/elysian_mem_pool.hpp>

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
        SubProcess<P,M>::start(m);
		if(_monitor && SubProcess<P,M>::_state == PROC_STATE_RUNNING) _monitor->onStart();
	}

	inline void update(M*const m, const float deltaTime) {
        SubProcess<P,M>::update(m, deltaTime);
	}

	inline void stop(M*const m) {
        SubProcess<P,M>::stop(m);
		if(_monitor && SubProcess<P,M>::_state == PROC_STATE_IDLE) _monitor->onFinish();
	}

	inline void setEventMonitor(EventMonitor*const monitor) {
		_monitor = (EventMonitor::isNull(monitor))? nullptr : monitor;
        if(_monitor) {
            _monitor->setCancelCallback([&]() {
                //static_cast<SubProcess<P,M>*>(this)->stop(m);
                SubProcess<P,M>::_state = PROC_STATE_IDLE;
                _monitor = nullptr;
            });
        }
	}


private:
	EventMonitor* _monitor = nullptr; //may need more in the future... :(
};

template<typename T, typename M, unsigned C>
class MonitorableSubProcessPool: public SubProcess<MonitorableSubProcessPool<T,M,C>, M> {
private:
	Pool<T> _pool = Pool<T>(C);

public:
    inline void _start(M* const /*m*/) {
		/*for(auto it = _pool.begin(); it < _pool.end(); ++it) {
			(*it).start(m);
		}*/
	}

	inline void _stop(M* const m) {
		for(auto it = _pool.begin(); it < _pool.end(); ++it) {
			(*it).stop(m);
			_pool.remove(it);
		}
	}

	inline void _update(M* const m, const float deltaTime) {
		for(auto it = _pool.begin(); it < _pool.end(); ++it) {
			if((*it).getProcState() == PROC_STATE_IDLE) {
				_pool.remove(it);
			} else (*it).update(m, deltaTime);
		}
	}

	inline T& claimIdleSubProcess(void) {
		return _pool.claimFreeEntry();
	}

	inline Pool<T>& getPool(void) { return _pool; }

};

template<typename T, typename M>
class LinearInterpolatorSubProcess:
		public MonitorableSubProcess<LinearInterpolatorSubProcess<T, M>, M> {

	//friend MonitorableSubProcess<LinearInterpolatorSubProcess<T, M>, M>;
	friend SubProcess<LinearInterpolatorSubProcess, M>;
private:
	T		_startKeyFrame;
	T		_endKeyFrame;
	T*		_curKeyFrame		= nullptr;

	float	_timeInterval		= 0.0f;
	float	_timeElapsed		= 0.0f;

	inline void _start(M * const m) {
		_timeElapsed = 0.0f;
		if(_timeInterval == 0.0f) { //handle request immediately
			*_curKeyFrame = _endKeyFrame;
			MonitorableSubProcess<LinearInterpolatorSubProcess, M>::stop(m);
		}
		else _startKeyFrame = *_curKeyFrame;
	}

	inline void _stop(M * const /*m*/) {
		_timeElapsed = 0.0f;
		*_curKeyFrame = _endKeyFrame;
		//_endFrame = m->_curState->_keyFrame;
		//_startFrame = _endFrame;
	}

	inline void _update(M * const m, const float deltaTime) {
		_timeElapsed += deltaTime;
		bool done = false;
		if(_timeElapsed >= _timeInterval) {
			_timeElapsed = _timeInterval;
			done = true;
		}

		_curKeyFrame->linearInterpolate(_startKeyFrame, _endKeyFrame, _timeInterval, _timeElapsed);

		if(done) MonitorableSubProcess<LinearInterpolatorSubProcess, M>::stop(m);
	}

public:
	void setKeyFrames(T*const curFrame, const T& endFrame) {
		_curKeyFrame = curFrame;
		//_startKeyFrame = startFrame;
		_endKeyFrame = endFrame;
	}

	void setTimeInterval(const float interval) { _timeInterval = interval; }
    const T& getEndKeyFrame(void) const { return _endKeyFrame; }
    T& getEndKeyFrame(void) { return _endKeyFrame; }
	const T& getStartKeyFrame(void) const { return _startKeyFrame; }

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
    typedef SubProcessManager<P, _This, _Rest...> SubProcParent;

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
  template <size_t _Index, class P, class _This, class... _Rest>
  struct SubProcessManagerSubProcessElement<_Index, SubProcessManager<P, _This, _Rest...>>
    : public SubProcessManagerSubProcessElement<_Index - 1, SubProcessManager<P, _Rest...> >
  {
  };


  // get reference to _Index element of tuple
  template<size_t _Index, typename P, class... _Types> inline
    typename SubProcessManagerSubProcessElement<_Index, SubProcessManager<P, _Types...>>::type
    get(SubProcessManager<P, _Types...>& _SubProcessManager)
  {
    typedef typename SubProcessManagerSubProcessElement<_Index, SubProcessManager<P, _Types...>>::_Ttype _Ttype;
    return (((_Ttype&) _SubProcessManager)._Myfirst);
  }


}

#endif // ELYSIAN_SubProcess_MANAGER_HPP
