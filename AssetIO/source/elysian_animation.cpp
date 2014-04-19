#include <cassert>
#include "elysian_animation.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include "elysian_renderable.hpp"
#include "elysian_generic.hpp"

namespace elysian {

//----NONSTATIC----

Animation::~Animation(void) {
	delete _children;
}

Component::TYPE Animation::getType(void) const { return Component::ANIMATION; }


void Animation::addChildEntity(Entity*const entity, const char*animName) {
	ChildAnimEntry& entry = _children->claimFreeEntry();

	entry.entity	= entity;
	entry.anim		= _childAttrib->getAnimation(animName);
	entry.seq		= entry.anim->getSequence(_currentSequence->getName());
	entry.frame		= entry.seq->getFrame(_currentFrame);
}

void Animation::removeChildEntity(const char *animName) {
	for(auto it = _children->begin(); it < _children->end(); ++it) {
		if(strcmp((*it).anim->getName(), animName) == 0) {
			_children->remove(it);
			return;
		}
	}
}

void Animation::_updateChildren(void) {
	if(!_children || _children->size() == 0) return;

	InstanceTransform*	parentTransform = &getEntity()->transform;
	InstanceTransform*	childTransform;
	Animation*			childAnim;

	for(auto it = _children->begin(); it < _children->end(); ++it) {

		//update children's positions and orientations
		childTransform = &(*it).entity->transform;
		childTransform->_pos = GYVector3 {
			parentTransform->_pos.x + (*it).frame->offset.x*0.5f*parentTransform->_size.x*parentTransform->_scale.x,
			parentTransform->_pos.y + (*it).frame->offset.y*0.5f*parentTransform->_size.y*parentTransform->_scale.y,
			parentTransform->_pos.z + (*it).frame->offset.z
		};
		childTransform->_orientZ = parentTransform->_orientZ + (*it).frame->zOrient;

		//update children's animations
		childAnim = (*it).entity->getAnimation();
		if(childAnim) {
			childAnim->playAnimation(_currentSequence->getName());
			childAnim->setCurrentFrame(_currentFrame);
		}
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
	_curArea->animations.push_back(this);
	return true;
}

//O(n)
bool Animation::removeFromLevel(void) {
    return containerEraseOnMatch(this, _curArea->animations);
}

#ifdef ELYSIAN_ENGINE

void Animation::update(const float deltaTime) {
	if(!_isActive || !_currentSequence) return;

	_updateChildren();

	_timer += deltaTime;
	if(_timer >= _currentSequence->_frames[_currentFrame].duration) {
		if(_currentSequence->_frames.size() <= ++_currentFrame) _currentFrame = 0;
		_timer = 0.0f;

		_entity->getRenderable()->setOffset(_currentSequence->_frames[_currentFrame].offset);
		_entity->getRenderable()->setFrame(_currentSequence->_frames[_currentFrame].spriteFrame);
		_entity->getRenderable()->setTexOrient(_currentSequence->_frames[_currentFrame].texOrient);
		if(!_looping) _isActive = false;
	}
}

void Animation::playAnimation(const char *name) {
	assert((_attrib->_sequences.find(name) != _attrib->_sequences.end()));
	if((_attrib->_sequences.find(name) == _attrib->_sequences.end())) return;
	if(_isActive && _attrib->_sequences[name] == _currentSequence) return;

	_currentFrame = 0;
	_timer = 0.0f;
	_currentSequence = _attrib->_sequences[name];
	_isActive = true;

	//Begin initial animation frame immediately!
	_entity->getRenderable()->setOffset(_currentSequence->_frames[0].offset);
	_entity->getRenderable()->setFrame(_currentSequence->_frames[0].spriteFrame);
	_entity->getRenderable()->setTexOrient(_currentSequence->_frames[0].texOrient);
}

void Animation::stopAnimation(void) {
	if(!_currentSequence) return;
	_currentFrame = 0;
	_timer = 0.0f;
// NEEDS TO BE DEREESTED WITH NEW RENDERABLE PARADIGM!!!!
	//_entity->setFrame(_currentSequence->_frames[_currentFrame].sheetIndex);
	//gyroSprite.transform.setPosition(_currentSequence->_frame[_currentFrame].offset);
	_isActive = false;
}

void Animation::setCurrentFrame(const unsigned frame) {
	_currentFrame = frame;
	_timer = 0.0f;
	_entity->getRenderable()->setOffset(_currentSequence->_frames[frame].offset);
	_entity->getRenderable()->setFrame(_currentSequence->_frames[frame].spriteFrame);
	_entity->getRenderable()->setTexOrient(_currentSequence->_frames[frame].texOrient);
}

#endif

}
