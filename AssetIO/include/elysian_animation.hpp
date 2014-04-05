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
		Animation(Entity& parent): Component(parent) {}
		virtual ~Animation(void) override	= default;

		AnimationAttrib* _attrib			= nullptr;
		AnimationSequence* _currentSequence = nullptr;
		float _timer						= 0.0f;
		unsigned int _currentFrame			= 0;
		bool _isActive						= false;
		bool _looping						= true;
		
	public:
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool canAddTo(Entity& entity) override;
		virtual Component::TYPE getType(void) const override { return Component::ANIMATION; }
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;

#ifdef ELYSIAN_ENGINE
		void update(const float deltaTime);
		void playAnimation(const char *name);
		void stopAnimation(void);
#endif
		
		AnimationAttrib* getAttrib(void) const { return _attrib; }
		void setAttrib(AnimationAttrib* attributes) { _attrib = attributes; }
		bool isActive(void) const { return _isActive; }
		unsigned getCurrentFrame(void) const { return _currentFrame; }
		void setCurrentFrame(const unsigned frame);
		bool isLooping(void) const { return _looping; }
		void setLooping(const bool val) { _looping = val; }
	
	};
	
}

#endif
