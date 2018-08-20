#ifndef ELYSIAN_ENGINE_HPP
#define ELYSIAN_ENGINE_HPP

#include <system/elysian_system.hpp>
#include <crtp/elysian_task_monitor.hpp>
#include <utility/elysian_performance_monitor.hpp>

ELYSIAN_ASSET_TYPEDEF(Debug)

namespace elysian {

	class Engine: public System {
        friend Debug;
	private:

        TaskMonitor     _updateMonitor;
        TaskMonitor     _renderMonitor;
        TaskMonitor     _inputMonitor;



	public:

       PerformanceMonitor renderMonitor             = PerformanceMonitor("Engine::render");
       PerformanceMonitor shadowPassMonitor         = PerformanceMonitor("Shadow Render Pass");
       PerformanceMonitor lightPassMonitor          = PerformanceMonitor("Render Scene");
       PerformanceMonitor guiRenderMonitor          = PerformanceMonitor("Render UI");
       PerformanceMonitor bloomMonitor              = PerformanceMonitor("Bloom Lighting");
       PerformanceMonitor updateMonitor            = PerformanceMonitor("Engine::update");

        PerformanceMonitor updateRigidBodyMonitor   = PerformanceMonitor("RigidBodySystem");
        PerformanceMonitor updateCollisionMonitor   = PerformanceMonitor("CollisionSystem");
        PerformanceMonitor updatePartyMonitor       = PerformanceMonitor("PartySystem");
        PerformanceMonitor updateAudioMonitor       = PerformanceMonitor("AudioSystem");
        PerformanceMonitor updateSkeletonMonitor    = PerformanceMonitor("SkeletonSystem");
        PerformanceMonitor updateLightMonitor       = PerformanceMonitor("LightSystem");
        PerformanceMonitor updateMapMonitor         = PerformanceMonitor("MapSystem");
        PerformanceMonitor updateCharacterMonitor   = PerformanceMonitor("CharacterSystem");
        PerformanceMonitor updateGuiMonitor         = PerformanceMonitor("GuiSystem");
        PerformanceMonitor updateTimerMonitor       = PerformanceMonitor("TimerSystem");
        PerformanceMonitor updateWarpMonitor        = PerformanceMonitor("WarpSystem");
        PerformanceMonitor updateParticleMonitor    = PerformanceMonitor("ParticleSystem");
        PerformanceMonitor updateDayMonitor         = PerformanceMonitor("DaySystem");
        PerformanceMonitor updateDialogMonitor      = PerformanceMonitor("DialogSystem");
        PerformanceMonitor updateCombatMonitor      = PerformanceMonitor("CombatSystem");
        PerformanceMonitor updateLuaMonitor         = PerformanceMonitor("Behavior::onUpdate");


        PerformanceMonitor processInputMonitor = PerformanceMonitor("Engine::processInput");

                        Engine(void);
                        ~Engine(void);

        bool            loadAssets(void);
        ENGINE_STATE    getState(void);

        void            update(void);
        void            processInput(void);
        void            render(void);

        void            perfMonitorDump(void);
    };

}

#endif
