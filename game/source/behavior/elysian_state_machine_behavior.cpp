/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <game/behavior/elysian_state_machine_behavior.hpp>

namespace elysian {

namespace game {

const char* StateMachineBehavior::getBehavName(void) const {
    return ELYSIAN_STATE_MACHINE_BEHAVIOR_NAME;
}

void StateMachineBehavior::setCurrentState(State* state) {
#if ELYSIAN_STATE_MACHINE_DEBUG
 //   DbgEvent event("SM STATE CHANGE: Entity[%s].StateMachineBehavior[%s] - %s to %s.",
   //                getEntity()->getName(), _name, _curState? _curState->_name : "NULL", state? state->_name : "NULL");
#endif

    if(_curState) {
     //   _curState->_onStateExit();
    }

    _curState = state;

    if(state) {
      //  state->_onStateEnter();
    }

}

void StateMachineBehavior::setStateTable(LuaVariant t) {
    _stateTable = std::move(t);
    //Find triggers within each state
    //Cache triggers as integer indices

    //setCurrentState(t[ELYSIAN_STATE_MACHINE_DEFAULT_NAME]->getValue<State*>());
}

void StateMachineBehavior::onUpdate(void) {
    if(isActive() && _curState /* && _curState->_flags & ON_UPDATE*/) {
     //   _curState->_onUpdate();
    }
}

}


}
