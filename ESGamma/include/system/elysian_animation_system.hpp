#ifndef ELYSIAN_ANIMATION_SYSTEM_HPP
#define ELYSIAN_ANIMATION_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_subprocess_manager.hpp"
#include "elysian_instance_transform.hpp"

namespace elysian {

	class AnimationSystem;

	typedef LinearInterpolatorSubProcess<InstanceTransform, AnimationSystem> InstanceTransformLinearInterpolator;

	class AnimationSystem:
			public SubProcessManager<AnimationSystem,
									InstanceTransformLinearInterpolator,
									InstanceTransformLinearInterpolator,
									InstanceTransformLinearInterpolator>,
            public System
    {

		enum SUBPROCS { SUBPROC_XFORM_INTERP_1 };

	public:
		void render(void);
		void update(void);

		void linearInterpTransform(InstanceTransform*const curFrame, const InstanceTransform& endFrame, const float interval=0.0f, EventMonitor*const monitor=nullptr);
	
	//===FUCK WITH PALETTES?===
	
	};



	// ====== INLINEZ =======

	inline void AnimationSystem::linearInterpTransform(InstanceTransform*const curFrame, const InstanceTransform& endFrame, const float interval, EventMonitor*const monitor) {
		auto &interp = get<SUBPROC_XFORM_INTERP_1> ( static_cast<SubProcessManager<AnimationSystem,
												  InstanceTransformLinearInterpolator>& >(*this));

		if(interp.getProcState() == PROC_STATE_RUNNING) return;

		interp.setKeyFrames(curFrame, endFrame);
		interp.setTimeInterval(interval);
		interp.setEventMonitor(monitor);
		interp.start(this);

	}

	
}

#endif
