#ifndef ELYSIAN_ENGINE_HPP
#define ELYSIAN_ENGINE_HPP

#include "elysian_game_input.hpp"
#include "elysian_system.hpp"
#include <elysian_task_monitor.hpp>

ELYSIAN_ASSET_TYPEDEF(Debug)

namespace elysian {
	
	class Engine: public System {
        friend Debug;
	private:
        GYInput         _platInput;
        MouseInput      _mouseInput;
        GameInput       _gameInput;

        TaskMonitor     _updateMonitor;
        TaskMonitor     _renderMonitor;
        TaskMonitor     _inputMonitor;
		
	public:
                        Engine(void);
                        ~Engine(void);

        bool            loadAssets(void);
        ENGINE_STATE    getState(void);

        void            update(void);
        void            processInput(void);
        void            render(void (*func)(void));
    };

}

#endif
