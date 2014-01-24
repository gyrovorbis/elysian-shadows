#include <fstream>
#include <cassert>
#include "elysian_animation.hpp"
#include "elysian_area.hpp"
#include "elysian_entity.hpp"
#include "elysian_renderable.hpp"
#include "elysian_generic.hpp"

namespace elysian {

//----NONSTATIC----

void AnimationSequence::addFrame(const float xOffset, const float yOffset, const float duration, const unsigned int frame) {
	AnimationFrame animFrame;
	animFrame.offset.x = xOffset;
	animFrame.offset.y = yOffset;
	animFrame.duration = duration;
	animFrame.spriteFrame = frame;
	_frames.push_back(animFrame);
}

bool Animation::load(std::ifstream& stream) {
	AnimationSequence *sequence;
	unsigned sequenceAmount;
	unsigned frameAmount;				
	AnimationFrame frame;
	
	dbgprintf(LOG_VERBOSE, "Animation::Load(ifstream)");
    dbgPush();
	
	/*stream >> _prefab;
	
	if(_prefab) {
		dbgprintf(LOG_VERBOSE, "Prefab");
                //delete _attrib;
		//Uhm.... no.
	}
	else {
		dbgprintf(LOG_VERBOSE, "Non-Prefab");
                //_attrib = new AnimationAttributes;*/
	
		//Load animation attributes
		stream >> sequenceAmount;
		dbgprintf(LOG_VERBOSE, "sequenceAmount - %d", sequenceAmount);
		for(unsigned int i = 0; i < sequenceAmount; ++i) {
			sequence = new AnimationSequence();
			stream >> sequence->_name;
			_attrib->_sequences[sequence->_name] = sequence;
			stream >> frameAmount;
			dbgprintf(LOG_VERBOSE, "sequence->name - %s, sequence->frameAmount - %d", sequence->_name, frameAmount);
			for(unsigned int j = 0; j < frameAmount; ++j) {

				stream >> frame.offset.x;
				stream >> frame.offset.y;
				stream >> frame.duration;
				stream >> frame.spriteFrame;
				dbgprintf(LOG_VERBOSE, "frame.offset.x - %0.2f, frame.offset.y - %0.2f, frame.duration - %0.2f, frame.sheetIndex - %d", frame.offset.x, frame.offset.y, frame.duration, frame.spriteFrame);
				sequence->_frames.push_back(frame);
			
			}
		}
			
	//}
    dbgPop();
	return true;
}

bool Animation::save(std::ofstream& stream) {
	dbgprintf(LOG_VERBOSE, "Animation::Save(ofstream)");
	dbgPush();
    
        //stream << _prefab << std::endl;
	
	//if(_prefab) {
		
	//}
	//else {
		
		//animation
		stream << _attrib->_sequences.size() << std::endl;
		for(std::map<const char *, AnimationSequence *, ltstr>::iterator it = _attrib->_sequences.begin(); it != _attrib->_sequences.end(); ++it) {
			stream << it->first << std::endl;
			stream << it->second->_frames.size() << std::endl;
			for(unsigned int i = 0; i < it->second->_frames.size(); ++i) {
				stream << it->second->_frames[i].offset.x << std::endl;
				stream << it->second->_frames[i].offset.y << std::endl;
				stream << it->second->_frames[i].duration << std::endl;
				stream << it->second->_frames[i].spriteFrame << std::endl;
			}
			
		}
	//}
    dbgPop();
	return true;
}

bool Animation::canAddTo(Entity &parent) {
	//There are no prereqs for this component.
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

	_timer += deltaTime;
	if(_timer >= _currentSequence->_frames[_currentFrame].duration) {
		if(_currentSequence->_frames.size() <= ++_currentFrame) _currentFrame = 0;
		_timer = 0.0f;

		_entity->getRenderable()->setOffset(_currentSequence->_frames[_currentFrame].offset);
		_entity->getRenderable()->setFrame(_currentSequence->_frames[_currentFrame].spriteFrame);
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
}

#endif

}
