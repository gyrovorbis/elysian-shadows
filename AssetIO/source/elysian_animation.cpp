#include <cassert>

#include "elysian_animation.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include "elysian_renderable.hpp"
#include "elysian_generic.hpp"

namespace elysian {

//----NONSTATIC----

void Animation::addState(const char* state) {
	if(_statePool.find(AnimStateEntry(state)) == _statePool.end()) {

		AnimStateEntry& entry = _statePool.claimFreeEntry();
		entry.setString(state);
		_updateStates();
	}

    if(_children) {
        for(auto it = _children->begin(); it < _children->end(); ++it) {
            if((*it).animComp) {
                (*it).animComp->addState(state);
            }
        }
    }
}

void Animation::removeState(const char* state) {
    auto it = _statePool.find(AnimStateEntry(state));

    if(it != _statePool.end()) {
        _statePool.remove(it);
        _updateStates();
    }

    if(_children) {
        for(auto it = _children->begin(); it < _children->end(); ++it) {
            if((*it).animComp) {
                (*it).animComp->removeState(state);
            }
        }
    }
}

void Animation::replaceState(const char *oldState, const char *newState) {
    auto it = _statePool.find(AnimStateEntry(oldState));
	auto itNew = _statePool.find(AnimStateEntry(newState));

	if(itNew == _statePool.end()) {

		if(it == _statePool.end()) { //never even had the old state
			return addState(newState);
		}

		(*it).setString(newState);
		_updateStates();

	} else if(it != _statePool.end()) { //we can still remove the old state
		return removeState(oldState);
	}

    if(_children) {
        for(auto it = _children->begin(); it < _children->end(); ++it) {
            if((*it).animComp) {
                (*it).animComp->replaceState(oldState, newState);
            }
        }
    }

}

void Animation::_updateStates(void) {
    updateState(_statePool);

    if(_children) {
        for(auto it = _children->begin(); it < _children->end(); ++it) {
            (*it).updateState(_statePool);
        }
    }
}

//INEFFICIENT AS SHIT!!
template<typename S, typename T>
void AnimationStateStack<S, T>::updateState(const Pool<AnimStateEntry>& states) {
    Pool<AnimStateEntry>::const_iterator iterators[ELYSIAN_ANIMATION_STATE_COUNT_MAX];
    bool            foundEntries[ELYSIAN_ANIMATION_STATE_COUNT_MAX] = { false };
    unsigned        entryCount         = 0;
    bool            found              = false;

    for(auto it = states.begin(); it < states.end(); ++it) iterators[entryCount++] = it;

    const S* state      = stateStack[0];
    const S* newState   = nullptr;
    const S* prevState  = stateStack[stackDepth];

    //Clear and repopulate the stack
    stackDepth = 0;
    do {
        found = false;

        for(unsigned i = 0; i < entryCount; ++i) {
            if(foundEntries[i]) continue;

            newState = state->getSequence((*iterators[i]).getString());
            if(newState) { //Found some shit! KEEP RECURSING!
                found = true;
                foundEntries[i] = true;
                state = newState;
                stateStack[++stackDepth] = state;
                break;
            }
        }

    } while(found);

    //Clear out states this animation does not give a shit about, REDUCING TIME COMPLEXITY OF THIS BITCH LATER ON
    //for(unsigned i = 0; i < entryCount; ++i) if(!foundEntries[i]) _statePool.remove(iterators[i]);

    if(state != prevState) { //state change!
        //curState = state;
        if(currentSequence) {
            setSequence(currentSequence->getName());
        }
    }
}


Animation::~Animation(void) {
	delete _children;
}

Component::TYPE Animation::getType(void) const { return Component::ANIMATION; }


void Animation::addChildEntity(Entity*const entity, const char*animName) {
    ChildAnimation* childAnim = _childAttrib->getAnimation(animName);

    assert(childAnim && !_children->isFull());

	ChildAnimEntry& entry = _children->claimFreeEntry();
    entry.setStackBottomState(childAnim);
    entry.entity            = entity;
    entry.animComp          = entity->getAnimation();
    entry.updateState(_statePool);
    if(currentSequence) entry.setSequence(currentSequence->getName());

}

void Animation::removeChildEntity(const char *animName) {
	for(auto it = _children->begin(); it < _children->end(); ++it) {
        if((*it).getStackBottomState<ChildAnimation>() && strcmp((*it).getStackBottomState<ChildAnimation>()->getName(), animName) == 0) {
			_children->remove(it);
			return;
		}
	}
}

void Animation::removeChildEntity(const Entity* const entity) {
	for(auto it = _children->begin(); it < _children->end(); ++it) {
		if((*it).entity == entity) {
			_children->remove(it);
			return;
		}
	}
}

void Animation::setSpeedMultiplier(const float factor) {
    if(_children) {
        for(auto it = _children->begin(); it < _children->end(); ++it) {
            auto* childAnim = (*it).getStackBottomState<ChildAnimation>();

            if(childAnim && childAnim->_inheritSpeedMultiplier && (*it).animComp) {
                (*it).animComp->setSpeedMultiplier(factor);
            }
        }
    }
    _speedMultiplier = factor;
}

void Animation::_updateChildren(void) {
	if(!_children || _children->size() == 0) return;

	InstanceTransform*	parentTransform = &getEntity()->transform;
	InstanceTransform*	childTransform;
	Animation*			childAnim;
	ChildAnimEntry*		childEntry;
	GYVector4 localPos = { 0.0f, 0.0f, 0.0f, 1.0f };
	GYVector4 worldPos;

	for(auto it = _children->begin(); it < _children->end(); ++it) {
		childEntry = &(*it);
        childAnim = childEntry->animComp;
		childTransform = &childEntry->entity->transform;

		GYVector3 offset = { 0.0f, 0.0f, 0.0f };
		float zOrient = 0.0f;

        if(childEntry->getStackBottomState<ChildAnimation>()) {


            childEntry->setSequence(currentSequence->getName());

            if(childEntry->currentSequence && childEntry->currentSequence->getFrameCount() > 0) {

                childEntry->setCurrentIndex(currentIndex);
                const unsigned prevIndex = childEntry->getPreviousIndex();

                if(childEntry->currentSequence->getLinearInterpolation()) {
                    gyLinearInterp((float*)&offset, 3, (float*)&childEntry->currentSequence->_frames[prevIndex].offset,
                            (float*)&childEntry->currentFrame->offset, currentFrame->duration, _timer);

                    gyLinearInterp(&zOrient, 1, &childEntry->currentSequence->_frames[prevIndex].zOrient,
                            &childEntry->currentFrame->zOrient, currentFrame->duration, _timer);

				} else {
                    offset = childEntry->currentFrame->offset;
                    zOrient = childEntry->currentFrame->zOrient;

				}
			} else { //assuming to use global defaults
                offset = childEntry->getStackBottomState<ChildAnimation>()->_globalOffset;
				//zOrient = 0.0f;
			}

            if(childEntry->getStackBottomState<ChildAnimation>()->_inheritScale) childTransform->_scale = parentTransform->_scale;
		}

		gyMatPush();
		gyMatLoadIdentity();

		gyMatTranslateScale(parentTransform->_pos.x +  (offset.x *parentTransform->_size.x*parentTransform->_scale.x),
					   parentTransform->_pos.y + (offset.y*parentTransform->_size.y*parentTransform->_scale.y) ,
					   parentTransform->_pos.z + (offset.z*parentTransform->_size.z*parentTransform->_scale.z),
					   childTransform->_size.x*childTransform->_scale.x, childTransform->_size.y*childTransform->_scale.y,  childTransform->_size.z*childTransform->_scale.z);
		gyMatRotateZ(zOrient);

		if(childAnim) gyMatTranslate(childAnim->getChildPivot().x, childAnim->getChildPivot().y, 0.0f);

		gyMatMultVec(&localPos, &worldPos, 1);

		gyMatPop();

		//update children's positions and orientations
		//childEntry->entity->setElev(getEntity()->getElev());

		childTransform->_pos = GYVector3 {
			worldPos.x,
			worldPos.y,
			worldPos.z
		};

		childTransform->_orientZ = parentTransform->_orientZ + zOrient;

		//update children's animations
/*
		if(childAnim) {
			childAnim->playAnimation(_currentSequence->getName());
			childAnim->setCurrentFrame(_currentFrame);
		}

*/
	}
}

bool Animation::load(FILE* /*file*/) {
	return true;
}

bool Animation::save(FILE* /*file*/) {
	return true;
}

bool Animation::canAddTo(Entity &parent) {
    return (parent.getRenderable());
}

//O(1)
bool Animation::addToLevel(void) {
	if(_children)
	for(auto it = _children->begin(); it < _children->end(); ++it) {
		auto childEntry = &(*it);
		_curArea->addEntity(childEntry->entity);
	}
	_curArea->animations.push_back(this);
	return true;
}

//O(n)
bool Animation::removeFromLevel(void) {
	if(_children)
	for(auto it = _children->begin(); it < _children->end(); ++it) {
		auto childEntry = &(*it);
		_curArea->removeEntity(childEntry->entity);
	}
    return containerEraseOnMatch(this, _curArea->animations);
}


void Animation::update(const float deltaTime) {
    if(!_isActive || !currentSequence) return;

        _timer += deltaTime * _speedMultiplier;
        if(_timer >= currentFrame->duration) {
            advanceFrame();
            if(currentIndex == 0) {
                if(!_looping) _isActive = false;
                if(_monitor) {
                    _monitor->onFinish();
                    _monitor = nullptr;
                }
            }

            _timer = 0.0f;
            _entity->getRenderable()->setOffset(currentFrame->offset);
            _entity->getRenderable()->setFrame(currentFrame->spriteFrame);
            _entity->getRenderable()->setTexOrient(currentFrame->texOrient);

        } else if(currentSequence->getLinearInterpolation()) {
            GYVector2 offset;
            const unsigned prevIndex = getPreviousIndex();

            gyLinearInterp((float*)&offset, 2, (float*)&currentSequence->_frames[prevIndex].offset,
                                            (float*)&currentFrame->offset, currentFrame->duration, _timer);

            _entity->getRenderable()->setOffset(offset);
        }

    _updateChildren();
}


void Animation::playAnimation(const char *name, EventMonitor*const monitor) {

    if(!_isActive || !currentSequence || strcmp(name, currentSequence->getName()) != 0) {
        setSequence(name);
        _timer = 0.0f;
        _isActive = true;
        _monitor = monitor;
        setCurrentIndex(0);

        //Begin initial animation frame immediately!
		if(currentFrame) {
			_entity->getRenderable()->setOffset(currentFrame->offset);
			_entity->getRenderable()->setFrame(currentFrame->spriteFrame);
			_entity->getRenderable()->setTexOrient(currentFrame->texOrient);
			if(monitor) monitor->onStart();
		}
    }

	if(_children) {
		for(auto it = _children->begin(); it < _children->end(); ++it) {
            if((*it).animComp) (*it).animComp->playAnimation(name);
		}
	}

}

void Animation::stopAnimation(void) {
    if(currentSequence) {
        setCurrentIndex(0);
        _timer = 0.0f;
	}

	if(_children) {
		for(auto it = _children->begin(); it < _children->end(); ++it) {
            if((*it).animComp) (*it).animComp->stopAnimation();
		}
	}

	_isActive = false;
}

void Animation::setCurrentFrame(const unsigned frame) {
    setCurrentIndex(frame);
	_timer = 0.0f;
    if(currentFrame) {
        _entity->getRenderable()->setOffset(currentFrame->offset);
        _entity->getRenderable()->setFrame(currentFrame->spriteFrame);
        _entity->getRenderable()->setTexOrient(currentFrame->texOrient);
    }
/*
    if(_children) {
        for(auto it = _children->begin(); it < _children->end(); ++it) {
            if((*it).animComp) (*it).animComp->setCurrentFrame(frame);
        }
    }*/

}

}
