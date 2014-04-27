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
	entry.frIndex	= _currentFrame;
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
	ChildAnimEntry*		childEntry;
	GYVector4 localPos = { 0.0f, 0.0f, 0.0f, 1.0f };
	GYVector4 worldPos;

	for(auto it = _children->begin(); it < _children->end(); ++it) {
		childEntry = &(*it);
		childAnim = childEntry->entity->getAnimation();
		childTransform = &childEntry->entity->transform;

		GYVector3 offset;
		float zOrient;

		childEntry->seq		= childEntry->anim->getSequence(_currentSequence->getName());
		childEntry->frame	= childEntry->seq->getFrame(_currentFrame);
		childEntry->frIndex	= _currentFrame;


		if(childEntry->seq->getLinearInterpolation()) {
			gyLinearInterp((float*)&offset, 3, (childEntry->frIndex != 0)? (float*)&childEntry->seq->_frames[childEntry->frIndex-1].offset : (float*)&childEntry->seq->_frames[childEntry->seq->_frames.size()-1].offset,
					(float*)&childEntry->frame->offset, _currentSequence->_frames[childEntry->frIndex].duration, _timer);

			gyLinearInterp(&zOrient, 1, (childEntry->frIndex != 0)? &childEntry->seq->_frames[childEntry->frIndex-1].zOrient : &childEntry->seq->_frames[childEntry->seq->_frames.size()-1].zOrient,
					&childEntry->frame->zOrient, _currentSequence->_frames[childEntry->frIndex].duration, _timer);

		} else {
			offset = childEntry->frame->offset;
			zOrient = childEntry->frame->zOrient;

		}

		gyMatPush();
		gyMatLoadIdentity();

		gyMatTranslateScale(parentTransform->_pos.x + 0.5f*parentTransform->_size.x*parentTransform->_scale.x - offset.x,
					   parentTransform->_pos.y + 0.5f*parentTransform->_size.y*parentTransform->_scale.y - offset.y,
					   parentTransform->_pos.z + offset.z,
					   childTransform->_size.x*childTransform->_scale.x, childTransform->_size.y*childTransform->_scale.y, 1.0f);
		gyMatRotateZ(zOrient);

		if(childAnim) gyMatTranslate(childAnim->getChildPivot().x, childAnim->getChildPivot().y, 0.0f);

		gyMatMultVec(&localPos, &worldPos, 1);

		gyMatPop();

		//update children's positions and orientations
		childEntry->entity->setElev(getEntity()->getElev());

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
	} else if(_currentSequence->getLinearInterpolation()) {
		GYVector2 offset;

		gyLinearInterp((float*)&offset, 2, (float*)&_currentSequence->_frames[(_currentFrame)? _currentFrame-1 : _currentSequence->_frames.size()-1].offset,
										(float*)&_currentSequence->_frames[_currentFrame].offset, _currentSequence->_frames[_currentFrame].duration, _timer);

		_entity->getRenderable()->setOffset(offset);

	}
}

void Animation::playAnimation(const char *name) {

	if(_children) {
		for(auto it = _children->begin(); it < _children->end(); ++it) {
			if((*it).entity->getAnimation()) (*it).entity->getAnimation()->playAnimation(name);
		}
	}

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
	if(_currentSequence) {
		_currentFrame = 0;
		_timer = 0.0f;
	}

	if(_children) {
		for(auto it = _children->begin(); it < _children->end(); ++it) {
			if((*it).entity->getAnimation()) (*it).entity->getAnimation()->stopAnimation();
		}
	}

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
