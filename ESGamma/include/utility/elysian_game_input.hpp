#ifndef ELYSIAN_GAME_INPUT_HPP
#define ELYSIAN_GAME_INPUT_HPP

#include <gyro_input_api.h>
#include "elysian_system.hpp"

namespace elysian {

    class MouseInput: public GYMouse {
    public:
        MouseInput(void);

        bool getTapped(const GY_MOUSE_BUTTON button) const;
        bool getDown(const GY_MOUSE_BUTTON button) const;
        void getScreenPos(GYVector2& pos) const;
        void getScrollWheel(GYVector2& dir) const;

        void poll(void);

    };

    class GameInput: public System {
	public:
		enum BUTTON { 
            BUT_NONE, BUT_A, BUT_B, BUT_C, BUT_D, BUT_E, BUT_F, BUT_G, BUT_H, BUT_I, BUT_J, BUT_K, BUT_L, BUT_M, BUT_N, BUT_O, BUT_P,
			BUT_Q, BUT_R, BUT_S, BUT_T, BUT_U, BUT_V, BUT_W, BUT_X, BUT_Y, BUT_Z, 
			BUT_DPAD_U, BUT_DPAD_D, BUT_DPAD_L, BUT_DPAD_R,
			BUT_0, BUT_1, BUT_2, BUT_3, BUT_4, BUT_5, BUT_6, BUT_7, BUT_8, BUT_9, 
			BUT_SPACE, BUT_RETURN, BUT_ESC,
			BUT_BACKSLASH, BUT_FORWARDSLASH,
			BUT_LSHIFT, BUT_RSHIFT, BUT_LCTRL, BUT_RCTRL, BUT_RALT, BUT_LALT, BUT_BACKQUOTE,
			BUT_MAX 
		};
		
		GameInput(void);

		void clear(void);

		bool getTapped(BUTTON key) const;
		bool getDown(BUTTON key) const;

		static void translate(const GYInput &platInput, GameInput &gameInput);

		void setDown(BUTTON key);
		void setUp(BUTTON key);

	private:
		bool keyDown[BUT_MAX];
		bool keyTapped[BUT_MAX];

		bool invalidKey(BUTTON key) const;

	};










    // ========= INLINEZ ===========
    inline MouseInput::MouseInput(void) { memset(this, 0, sizeof(MouseInput)); }
    inline bool MouseInput::getTapped(const GY_MOUSE_BUTTON button) const { return butTapped[button]; }
    inline bool MouseInput::getDown(const GY_MOUSE_BUTTON button) const { return butDown[button]; }
    inline void MouseInput::getScreenPos(GYVector2& pos) const { pos = screenPos; }
    inline void MouseInput::getScrollWheel(GYVector2& dir) const { dir = scrollWheel; }
    inline void MouseInput::poll(void) { gyMousePoll(this); }

}

#endif
