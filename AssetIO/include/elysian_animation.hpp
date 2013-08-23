#ifndef ELYSIAN_ANIMATION_HPP
#define ELYSIAN_ANIMATION_HPP

#include "elysian_component.hpp"
#include "elysian_stl_algorithm.hpp"
#include <gyro.h>
#include <vector>
#include <map>

#define ELYSIAN_ANIMATION_SEQUENCE_NAME_SIZE 50

namespace elysian {

	struct AnimationFrame {
		GYVector2 offset;
		float duration;
		unsigned int spriteFrame;
	};

	class AnimationSequence { 
		friend class Animation;
	private:
		inline AnimationSequence(void) {}
		char _name[ELYSIAN_ANIMATION_SEQUENCE_NAME_SIZE];
		std::vector<AnimationFrame> _frames;
	public:
		inline AnimationSequence(const char* name) { strcpy(_name, name); }
		void addFrame(const float xOffset, const float yOffset, const float duration, const unsigned int sheetIndex); 
		const char *getName(void) { return _name; }
	};

	
	class AnimationAttrib {
		friend class Animation;
	private:
		//unsigned int _sequenceAmount;
		std::map<const char*, AnimationSequence*, ltstr> _sequences;
	public:
		inline AnimationAttrib(void) { }
		inline ~AnimationAttrib(void) { }
		inline AnimationSequence* addSequence(const char *name) { 
			AnimationSequence *const seq = new AnimationSequence(name);
			_sequences[seq->getName()] = seq;
			return seq;
		}
	};


	class Animation: public Component { 
        friend class Entity;
	protected:
		AnimationAttrib *_attrib; //prefab-able
		Animation(Entity& entity);
		virtual ~Animation(void) { }

		bool _isActive;
		unsigned int _currentFrame;
		float _timer;
		AnimationSequence *_currentSequence;
		
	public:
		virtual bool load(std::ifstream& stream);
		virtual bool save(std::ofstream& stream);
		virtual bool canAddTo(Entity& entity);
		virtual Component::TYPE getType(void) const { return Component::ANIMATION; }
		virtual bool addToLevel(void);
		virtual bool removeFromLevel(void);

#ifdef ELYSIAN_ENGINE
		void update(float deltaTime);
		void playAnimation(const char *name);
		void stopAnimation(void);
		inline bool isActive(void) const { return _isActive; }
#endif
		
		inline AnimationAttrib* getAttrib(void) const { return _attrib; }
		inline void setAttrib(AnimationAttrib* attributes) { _attrib = attributes; }
	
	};
	
}

#endif
