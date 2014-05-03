#ifndef ELYSIAN_ANIMATION_HPP
#define ELYSIAN_ANIMATION_HPP

#include "elysian_component.hpp"
#include "elysian_stl_algorithm.hpp"
#include "elysian_mem_pool.hpp"
#include <gyro.h>
#include <vector>
#include <map>

#define ELYSIAN_ANIMATION_SEQUENCE_NAME_SIZE 50
#define ELYSIAN_ANIMATION_MAX_CHILDREN		5

namespace elysian {

	class Entity;

	struct AnimationFrame {
		GYVector2			offset;
		float				duration;
		unsigned int		spriteFrame;
		GY_RECT_TEX_ORIENT	texOrient;
	};

	class AnimationSequence { 
		friend class Animation;
	private:
		char						_name[ELYSIAN_ANIMATION_SEQUENCE_NAME_SIZE];
		bool						_linearInterpolation = true;
		std::vector<AnimationFrame> _frames;
					AnimationSequence(void) = default;
	public:
					AnimationSequence(const char* name);
		const char*	getName(void);
		void		addFrame(const float xOffset, const float yOffset, const float duration,
							 const unsigned int sheetIndex, const GY_RECT_TEX_ORIENT texOrient);
		void		setLinearInterpolation(const bool val);
		bool		getLinearInterpolation(void) const;
	};

	class AnimationAttrib {
		friend class Animation;
	private:
		std::map<const char*, AnimationSequence*, ltstr> _sequences;
	public:
		AnimationSequence*	addSequence(const char *name);
							~AnimationAttrib(void);
	};

	struct ChildAnimationFrame {
		GYVector3	offset;
		float		zOrient;
	};

	class ChildAnimationSequence {
		friend class Animation;
	private:
		char								_name[ELYSIAN_ANIMATION_SEQUENCE_NAME_SIZE];
		std::vector<ChildAnimationFrame>	_frames;
		bool								_linearInterpolation	= true;
	public:
								ChildAnimationSequence(const char* name);
		const char*				getName(void);
		void					addFrame(const float x, const float y, const float z, const float theta);
		ChildAnimationFrame*	getFrame(const unsigned index);
		void					setLinearInterpolation(const bool val);
		bool					getLinearInterpolation(void) const;

	};

	//Subcomponent of Animation
	class ChildAnimation {
		friend class Animation;
	private:
		std::map<const char*, ChildAnimationSequence*, ltstr>	_sequences;
		GYVector3												_globalOffset = { 0.0f, 0.0f, 0.0f };
		char													_name[ELYSIAN_ANIMATION_SEQUENCE_NAME_SIZE];
	public:
								ChildAnimation(const char* name);
								~ChildAnimation(void);
		ChildAnimationSequence* addSequence(const char* name);
		ChildAnimationSequence* getSequence(const char* name);
		const char*				getName(void);
		void					setGlobalOffset(const float x, const float y, const float z);
	};

	class ChildAnimationAttrib {
		friend class Animation;
	private:

		std::map<const char*, ChildAnimation*, ltstr> _animations;
	public:
						~ChildAnimationAttrib(void);
		ChildAnimation* addAnimation(const char* name);
		ChildAnimation* getAnimation(const char* name);

	};


	class Animation: public Component { 
        friend class Entity;
	protected:

		struct ChildAnimEntry {
			Entity*					entity			= nullptr;
			ChildAnimation*			anim			= nullptr;
			ChildAnimationSequence* seq				= nullptr;
			ChildAnimationFrame*	frame			= 0;
			float					timer			= 0.0f;
			unsigned				frIndex			= 0;
		};

		GYVector2				_childPivot			= GYVector2 { 0.0f, 0.0f };
		Pool<ChildAnimEntry>*	_children			= nullptr;
		AnimationAttrib*		_attrib				= nullptr;
		ChildAnimationAttrib*	_childAttrib		= nullptr;
		AnimationSequence*		_currentSequence	= nullptr;
		float					_speedMultiplier	= 1.0f;
		float					_timer				= 0.0f;
		unsigned int			_currentFrame		= 0;
		bool					_isActive			= false;
		bool					_looping			= true;
		
				Animation(Entity& parent);
		virtual ~Animation(void) override;
		void	_updateChildren(void);

	public:
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool canAddTo(Entity& entity) override;
		virtual TYPE getType(void) const override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;

#ifdef ELYSIAN_ENGINE
		void update(const float deltaTime);
		void playAnimation(const char *name);
		void stopAnimation(void);
#endif
		
		AnimationAttrib*		getAttrib(void) const;
		ChildAnimationAttrib*	getChildAttrib(void) const;
		void					setSpeedMultiplier(const float factor);
		void					setAttrib(AnimationAttrib* attributes);
		void					setChildAttrib(ChildAnimationAttrib* childAttrib);
		bool					isActive(void) const;
		unsigned				getCurrentFrame(void) const;
		void					setCurrentFrame(const unsigned frame);
		bool					isLooping(void) const;
		void					setLooping(const bool val);
		void					setChildPivot(const float x, const float y);
		const GYVector2&		getChildPivot(void) const;
		void					addChildEntity(Entity*const entity, const char*animName);
		void					removeChildEntity(const char*animName);
		void					removeChildEntity(const Entity* entity);
	};


	// ====== INLINEZ ======

	inline AnimationSequence::AnimationSequence(const char* name) { strcpy(_name, name); }
	inline const char *AnimationSequence::getName(void) { return _name; }
	inline void AnimationSequence::addFrame(const float xOffset, const float yOffset, const float duration,
											const unsigned int frame, const GY_RECT_TEX_ORIENT texOrient) {
		_frames.push_back(AnimationFrame {
			GYVector2 { xOffset, yOffset },
			duration,
			frame,
			texOrient
		});
	}
	inline void AnimationSequence::setLinearInterpolation(const bool val) { _linearInterpolation = val; }
	inline bool AnimationSequence::getLinearInterpolation(void) const { return _linearInterpolation; }

	inline AnimationSequence* AnimationAttrib::addSequence(const char *name) {
		AnimationSequence *const seq = new AnimationSequence(name);
		_sequences[seq->getName()] = seq;
		return seq;
	}
	inline AnimationAttrib::~AnimationAttrib(void) {
		for(auto it: _sequences) delete it.second;
	}

	inline ChildAnimationSequence::ChildAnimationSequence(const char* name) { strcpy(_name, name); }
	inline const char* ChildAnimationSequence::getName(void) { return _name; }
	inline void ChildAnimationSequence::addFrame(const float x, const float y, const float z, const float theta) {
		_frames.push_back(ChildAnimationFrame{
			GYVector3 { x, y, z },
			theta,
		});
	}
	inline ChildAnimationFrame* ChildAnimationSequence::getFrame(const unsigned index) { return &_frames[index]; }
	inline void ChildAnimationSequence::setLinearInterpolation(const bool val) { _linearInterpolation = val; }
	inline bool ChildAnimationSequence::getLinearInterpolation(void) const { return _linearInterpolation; }

	inline ChildAnimation::ChildAnimation(const char* name) { strcpy(_name, name); }
	inline const char* ChildAnimation::getName(void) { return _name; }
	inline ChildAnimationSequence* ChildAnimation::addSequence(const char *name) {
		ChildAnimationSequence*const seq = new ChildAnimationSequence(name);
		_sequences[seq->getName()] = seq;
		return seq;
	}
	inline ChildAnimationSequence* ChildAnimation::getSequence(const char *name) {
		auto it = _sequences.find(name);
		return (it == _sequences.end())? nullptr : it->second;
	}
	inline ChildAnimation::~ChildAnimation(void) {
		for(auto it: _sequences) delete it.second;
	}
	inline void ChildAnimation::setGlobalOffset(const float x, const float y, const float z) {
		_globalOffset.x = x; _globalOffset.y = y; _globalOffset.z = z;
	}

	inline ChildAnimation* ChildAnimationAttrib::addAnimation(const char *name) {
		ChildAnimation*const anim = new ChildAnimation(name);
		_animations[anim->getName()] = anim;
		return anim;
	}
	inline ChildAnimation* ChildAnimationAttrib::getAnimation(const char *name) {
		auto it = _animations.find(name);
		return (it == _animations.end())? nullptr : it->second;
	}

	inline ChildAnimationAttrib::~ChildAnimationAttrib(void) {
		for(auto it: _animations) delete it.second;
	}

	inline Animation::Animation(Entity& parent): Component(parent) {}
	inline AnimationAttrib* Animation::getAttrib(void) const { return _attrib; }
	inline ChildAnimationAttrib* Animation::getChildAttrib(void) const { return _childAttrib; }
	inline void Animation::setAttrib(AnimationAttrib* attributes) {
		_attrib = attributes;
	}
	inline void Animation::setChildAttrib(ChildAnimationAttrib *childAttrib) {
		_childAttrib = childAttrib;
		if(_childAttrib && !_children) {
			_children = new Pool<ChildAnimEntry>(ELYSIAN_ANIMATION_MAX_CHILDREN);
		}
	}
	inline bool Animation::isActive(void) const { return _isActive; }
	inline unsigned Animation::getCurrentFrame(void) const { return _currentFrame; }
	inline bool Animation::isLooping(void) const { return _looping; }
	inline void Animation::setLooping(const bool val) { _looping = val; }
	inline void Animation::setChildPivot(const float x, const float y) { _childPivot.x = x; _childPivot.y = y; }
	inline const GYVector2& Animation::getChildPivot(void) const { return _childPivot; }
	inline void Animation::setSpeedMultiplier(const float factor) { _speedMultiplier = factor; }



}

#endif
