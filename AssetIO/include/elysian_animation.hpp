#ifndef ELYSIAN_ANIMATION_HPP
#define ELYSIAN_ANIMATION_HPP

#include "elysian_component.hpp"
#include "elysian_stl_algorithm.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_event_monitor.hpp"
#include <gyro.h>
#include <vector>
#include <map>

#define ELYSIAN_ANIMATION_SEQUENCE_NAME_SIZE    50
#define ELYSIAN_ANIMATION_MAX_CHILDREN          99
#define ELYSIAN_ANIMATION_STATE_COUNT_MAX       99
#define ELYSIAN_ANIMATION_STATE_LENGTH_MAX      50

namespace elysian {

	class Entity;

	struct AnimationFrame {
		GYVector2			offset;
		float				duration;
		unsigned int		spriteFrame;
		GY_RECT_TEX_ORIENT	texOrient;
	};

    template<typename P, typename T>
    class AnimationSequenceBase {
    protected:
        std::map<const char*, P*, ltstr> _sequences;
        std::vector<T>                                      _frames;
        char                                                _name[ELYSIAN_ANIMATION_SEQUENCE_NAME_SIZE];
        bool                                                _linearInterpolation = true;
                        AnimationSequenceBase(void) = default;

    public:
                        AnimationSequenceBase(const char* name);
        const char*     getName(void) const;
        P*              addSequence(const char* name);
        void            addFrame(const T&& frame);
        const P*        getSequence(const char*name) const;
        const P*        getSequenceInclusive(const char*name) const;
        const T*        getFrame(const unsigned index) const;
        bool            getLinearInterpolation(void) const;
        void            setLinearInterpolation(const bool val);
        unsigned        getFrameCount(void) const;
    };


    class AnimationSequence:
            public AnimationSequenceBase<AnimationSequence, AnimationFrame>
    {
        friend class Animation;
    protected:

                            AnimationSequence(void) = default;
	public:
                            AnimationSequence(const char* name);
        void                addFrame(const float xOffset, const float yOffset, const float duration,
                                    const unsigned int sheetIndex, const GY_RECT_TEX_ORIENT texOrient);
    };

    class AnimationAttrib: public AnimationSequence {
		friend class Animation;

	public:
        ~AnimationAttrib(void);
	};

	struct ChildAnimationFrame {
		GYVector3	offset;
		float		zOrient;
	};

    class ChildAnimationSequence:
            public AnimationSequenceBase<ChildAnimationSequence, ChildAnimationFrame>  {
		friend class Animation;

	public:
								ChildAnimationSequence(const char* name);
		void					addFrame(const float x, const float y, const float z, const float theta);

	};

	//Subcomponent of Animation
    class ChildAnimation: public ChildAnimationSequence {
		friend class Animation;
    private:
		GYVector3												_globalOffset = { 0.0f, 0.0f, 0.0f };
        bool                                                    _inheritSpeedMultiplier = false;
		bool													_inheritScale	= true;
	public:
								ChildAnimation(const char* name);
								~ChildAnimation(void);
		void					setGlobalOffset(const float x, const float y, const float z);
        void                    setInheritSpeedMultiplier(const bool val);
		void					setInheritScale(const bool val);
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

    typedef CharArrayPoolEntry<ELYSIAN_ANIMATION_STATE_LENGTH_MAX> AnimStateEntry;


    template<typename S, typename T>
    struct AnimationStateStack {
    friend class Animation;
    protected:
        const S*          stateStack[ELYSIAN_ANIMATION_STATE_COUNT_MAX] = { nullptr };
        unsigned          stackDepth = 0;
        const S*          currentSequence = nullptr;
        const T*          currentFrame    = nullptr;
        unsigned          currentIndex    = 0;

    public:
        void setStackBottomState(const S* bottom);

        template <typename A>
        const A*   getStackBottomState(void) const;

        void        setSequence(const char* name);
        void        updateState(const Pool<AnimStateEntry>& states);
        void        setCurrentIndex(const unsigned index);
        unsigned    getPreviousIndex(void) const;
        void        advanceFrame(void);
    };


    class Animation: public Component,
                     public AnimationStateStack<AnimationSequence, AnimationFrame>
    {
        friend class Entity;
	protected:

        struct ChildAnimEntry:
                public AnimationStateStack<ChildAnimationSequence, ChildAnimationFrame>
        {
			Entity*					entity			= nullptr;
            Animation*              animComp        = nullptr;
		};

        Pool<AnimStateEntry>    _statePool          = Pool<AnimStateEntry>(ELYSIAN_ANIMATION_MAX_CHILDREN);
		GYVector2				_childPivot			= GYVector2 { 0.0f, 0.0f };
		Pool<ChildAnimEntry>*	_children			= nullptr;
		ChildAnimationAttrib*	_childAttrib		= nullptr;
        EventMonitor*           _monitor            = nullptr;
		float					_speedMultiplier	= 1.0f;
		float					_timer				= 0.0f;
		bool					_isActive			= false;
		bool					_looping			= true;
        AnimationAttrib*        _attrib             = nullptr;
		
				Animation(Entity& parent);
		virtual ~Animation(void) override;
		void	_updateChildren(void);
        void    _updateStates(void);

	public:
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool canAddTo(Entity& entity) override;
		virtual TYPE getType(void) const override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;

        void        addState    (const char* state);
        void        removeState (const char* state);
        void        replaceState(const char* oldState, const char* newState);

#ifdef ELYSIAN_ENGINE
        void        update(const float deltaTime);
        void        playAnimation(const char *name, EventMonitor*const monitor = nullptr);
        void        stopAnimation(void);
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
        //const AnimationSequence* getSequence(const char* name);
	};


	// ====== INLINEZ ======

    inline AnimationSequence::AnimationSequence(const char* name): AnimationSequenceBase(name) { }
	inline void AnimationSequence::addFrame(const float xOffset, const float yOffset, const float duration,
											const unsigned int frame, const GY_RECT_TEX_ORIENT texOrient) {
        AnimationSequenceBase::addFrame(AnimationFrame {
			GYVector2 { xOffset, yOffset },
			duration,
			frame,
			texOrient
		});
	}
	inline AnimationAttrib::~AnimationAttrib(void) {
		for(auto it: _sequences) delete it.second;
	}

    inline ChildAnimationSequence::ChildAnimationSequence(const char* name): AnimationSequenceBase(name) { }
	inline void ChildAnimationSequence::addFrame(const float x, const float y, const float z, const float theta) {
        AnimationSequenceBase::addFrame(ChildAnimationFrame{
			GYVector3 { x, y, z },
			theta,
		});
	}

    inline ChildAnimation::ChildAnimation(const char* name): ChildAnimationSequence(name) {  }
	inline void ChildAnimation::setInheritScale(const bool val) { _inheritScale = val; }
    inline void ChildAnimation::setInheritSpeedMultiplier(const bool val) { _inheritSpeedMultiplier = val; }

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
        setStackBottomState(attributes);
        _attrib = attributes;
	}
	inline void Animation::setChildAttrib(ChildAnimationAttrib *childAttrib) {
		_childAttrib = childAttrib;
		if(_childAttrib && !_children) {
			_children = new Pool<ChildAnimEntry>(ELYSIAN_ANIMATION_MAX_CHILDREN);
		}
	}
	inline bool Animation::isActive(void) const { return _isActive; }
    inline unsigned Animation::getCurrentFrame(void) const { return currentIndex; }
	inline bool Animation::isLooping(void) const { return _looping; }
	inline void Animation::setLooping(const bool val) { _looping = val; }
	inline void Animation::setChildPivot(const float x, const float y) { _childPivot.x = x; _childPivot.y = y; }
    inline const GYVector2& Animation::getChildPivot(void) const { return _childPivot; }\

    template<typename P, typename T>
    inline AnimationSequenceBase<P, T>::AnimationSequenceBase(const char*name) {
        strcpy(_name, name);
    }

    template<typename P, typename T>
    inline const char* AnimationSequenceBase<P, T>::getName(void) const {
        return _name;
    }

    template<typename P, typename T>
    inline P* AnimationSequenceBase<P, T>::addSequence(const char* name) {
        P*const seq = new P(name);
        _sequences[seq->getName()] = seq;
        return seq;
    }

    template<typename P, typename T>
    inline void AnimationSequenceBase<P, T>::addFrame(const T&& frame) {
        _frames.push_back(frame);
    }

    template<typename P, typename T>
    inline const T* AnimationSequenceBase<P, T>::getFrame(const unsigned index) const {
        return (index < _frames.size())? &_frames[index] : nullptr;
    }

    template<typename P, typename T>
    inline void AnimationSequenceBase<P, T>::setLinearInterpolation(const bool v) {
        _linearInterpolation = v;
    }

    template<typename P, typename T>
    inline bool AnimationSequenceBase<P, T>::getLinearInterpolation(void) const {
        return _linearInterpolation;
    }

    template<typename P, typename T>
    inline const P* AnimationSequenceBase<P, T>::getSequence(const char* name) const {
        auto it = _sequences.find(name);
        return (it == _sequences.end())? nullptr : it->second;
    }

    template<typename P, typename T>
    inline const P* AnimationSequenceBase<P, T>::getSequenceInclusive(const char* name) const {
        return (strcmp(_name, name) == 0)? static_cast<const P*>(this) : getSequence(name);
    }

    template<typename P, typename T>
    inline unsigned AnimationSequenceBase<P, T>::getFrameCount(void) const {
        return _frames.size();
    }

    template<typename S, typename T>
    inline void AnimationStateStack<S,T>::setCurrentIndex(const unsigned index) {
		if(!currentSequence) return;
		//assert(currentSequence);
     //   if(currentSequence) {
            const unsigned frameCount = currentSequence->getFrameCount();
            currentIndex = (frameCount > 0)? index % frameCount : 0;
            currentFrame = currentSequence->getFrame(currentIndex);
      //  } else {
           // currentIndex = 0;
           // currentFrame = nullptr;
       // }
    }

    template<typename S, typename T>
    inline unsigned AnimationStateStack<S,T>::getPreviousIndex(void) const {
        assert(currentSequence);
        if(currentIndex > 0) {
            return currentIndex-1;
        } else {
            const unsigned frameCount = currentSequence->getFrameCount();
            return (frameCount)? frameCount-1 : 0;
        }
    }

    template<typename S, typename T>
    inline void AnimationStateStack<S,T>::advanceFrame(void) {
        assert(currentSequence);
        if(++currentIndex >= currentSequence->getFrameCount()) {
            currentIndex = 0;
        }
        currentFrame = currentSequence->getFrame(currentIndex);
    }

    template<typename S, typename T> template<typename A>
    inline const A* AnimationStateStack<S, T>::getStackBottomState(void) const { return static_cast<const A*>(stateStack[0]); }
    template<typename S, typename T>
    inline void AnimationStateStack<S, T>::setStackBottomState(const S* bottom) { stateStack[0] = bottom; }

    template<typename S, typename T>
    inline void AnimationStateStack<S, T>::setSequence(const char* name) {
		//if(currentSequence && strcmp(currentSequence->getName(), name) == 0) return; //no change

        const S* seq = stateStack[stackDepth]->getSequence(name);
        if(!seq && stateStack[stackDepth]->getFrameCount() > 0) {
            seq = stateStack[stackDepth];
        } else if(!seq) {

            //work backwards and try to find deepest (most specialized) version of sequence
            for(int i = stackDepth-1; i >= 0; --i) {
                seq = stateStack[i]->getSequence(name);
                if(seq) {
                    break;
                }
            }
        }

        currentSequence = seq;
        if(seq) {
            currentIndex = 0;//(seq->getFrame(currentIndex))?
					//currentIndex : 0;
            currentFrame = seq->getFrame(currentIndex);
        } else {
            currentIndex = 0;
            currentFrame = nullptr;
        }
    }

}

#endif
