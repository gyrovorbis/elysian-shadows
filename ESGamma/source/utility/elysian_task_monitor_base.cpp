#include <elysian_task_monitor.hpp>
#include <elysian_debug.hpp>

namespace elysian {

void TaskMonitorBase::startNewFrame(void) {
    //calculate running average over last n frames
    int64_t accum = 0;
    if(_buffDepth == PERFORMANCE_MONITOR_CIRC_BUFFER_SIZE) {
        _dirty = true;  //mark that this process isn't doing shit anymore
        for(unsigned i = 0; i < _buffDepth; ++i) {
            accum += _tickBuffer[i];
            if(_tickBuffer[i] != 0) {
                _dirty = false;
            }
        }
    }
    _avgTime = (double)accum/(double)_buffDepth; //don't convert to float until division

    //update buffer index
    if(_buffDepth < PERFORMANCE_MONITOR_CIRC_BUFFER_SIZE) ++_buffDepth;
    if(++_curIndex >= PERFORMANCE_MONITOR_CIRC_BUFFER_SIZE) _curIndex = 0;

    //clear accumulator
    _tickBuffer[_curIndex] = 0;

    for(unsigned i = 0; i < PERFORMANCE_MONITOR_MAX_CHILDREN; ++i) {
        if(_children[i]) _children[i]->startNewFrame();
    }
}

void TaskMonitorBase::print(void) const {
    dbgprintf("%s\t%f %% \t%lfns", _name, getPercentTotalTime()*100.0f, getAvgTime());
    dbgpush();
    for(unsigned i = 0; i < PERFORMANCE_MONITOR_MAX_CHILDREN; ++i) {
        if(_children[i]) _children[i]->print();
    }
    dbgpop();
}

double TaskMonitorBase::getPercentTotalTime(void) const {
    const TaskMonitorBase* topLevel = this;

    while(topLevel->getParent()) {
        topLevel = topLevel->getParent();
    }

    return getAvgTime()/topLevel->getAvgTime();
}

double TaskMonitorBase::getPercentParentTime(void) const {
    return getParent()? getAvgTime()/getParent()->getAvgTime() : 1.0f;
}

bool TaskMonitorBase::addChild(TaskMonitor*const child) {
    int freeIndex   = -1;
    int dirtyIndex  = -1;

    for(int  i = 0; i < PERFORMANCE_MONITOR_MAX_CHILDREN; ++i) {
        if(!_children[i]) freeIndex = i;
        else if(_children[i] == child) return false; //already a child
        else if(_children[i]->_dirty) dirtyIndex = i;
    }

    if(freeIndex != -1) {       //new child
        _children[freeIndex] = child;
        child->_parent = static_cast<elysian::TaskMonitor*>(this);
        return true;
    } else if(dirtyIndex != -1) {
        _children[dirtyIndex] = child; //replace a child that isn't doing shit
        child->_parent = static_cast<elysian::TaskMonitor*>(this);
        return true;
    } else {
        return false; //guess you're just fucked...
    }
}



unsigned TaskMonitorBase::getChildCount(void) const {
    unsigned size = 0;
    for(unsigned i = 0; i < PERFORMANCE_MONITOR_MAX_CHILDREN; ++i) if(_children[i]) ++size;
    return size;
}

TaskMonitor* TaskMonitorBase::getChild(const unsigned index) const {
    return _children[index];
}

TaskMonitor* TaskMonitorBase::getChild(const char*const name) {
    for(unsigned i = 0; i < PERFORMANCE_MONITOR_MAX_CHILDREN; ++i)
        if(_children[i] && strcmp(_children[i]->_name, name) == 0) return _children[i];
     return nullptr;
}


void TaskMonitorBase::clearChildren(void) {
    for(unsigned i = 0; i < PERFORMANCE_MONITOR_MAX_CHILDREN; ++i) _children[i] = nullptr;
}

void TaskMonitorBase::sortChildrenBySlowest(void) {
    TaskMonitor* temp;

    for(unsigned i = 0; i < PERFORMANCE_MONITOR_MAX_CHILDREN; ++i) {
        for(unsigned j = i+1; j < PERFORMANCE_MONITOR_MAX_CHILDREN; ++j) {
			if(!_children[i] || (_children[j] && _children[i]->getAvgTime() < _children[j]->getAvgTime())) {
				temp			= _children[i];
				_children[i]	= _children[j];
				_children[j]	= temp;
            }
        }
    }

	for(unsigned i = 0; i < PERFORMANCE_MONITOR_MAX_CHILDREN; ++i) {
		if(!_children[i]) break;
		_children[i]->sortChildrenBySlowest();
	}

}

}
