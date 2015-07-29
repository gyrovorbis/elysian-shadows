#include "elysian_game_input.hpp"
#include "elysian_debug.hpp"
#include "elysian_behavior_system.hpp"

/*
 The Translate() function is for mapping platform-specific input devices
 (controllers, mice, keyboards, touch screens, etc) to an engine-specific
 virtual input class.
 */


namespace elysian {

void GameInput::translate(const GYInput& platInput, GameInput& gameInput) {
#if GYRO_PLATFORM == GYRO_PLATFORM_x86 || GYRO_PLATFORM == GYRO_PLATFORM_ANDROID || GYRO_PLATFORM == GYRO_PLATFORM_Qt

#	if GYRO_PLATFORM == GYRO_PLATFORM_x86
	if(platInput.keyTapped[GY_KEY_ESC]) exit(0);
#	endif

    for(unsigned i = 0; i < GY_KEY_LAST; ++i) {

        switch(i) {
 #if 0
        case GY_KEY_A:
            if(platInput.keyDown[i]) gameInput.setDown(GameInput::BUT_J);
            else gameInput.setUp(GameInput::BUT_J);
            break;

        case GY_KEY_O:
            if(platInput.keyDown[i]) gameInput.setDown(GameInput::BUT_I);
            else gameInput.setUp(GameInput::BUT_I);
            break;
 /*
        case GameInput::BUT_Y:
            if(platInput.keyDown[i]) gameInput.setDown((GameInput::BUTTON)i);
            else gameInput.setUp((GameInput::BUTTON)i);
            break;
        case GameInput::BUT_U:
            if(platInput.keyDown[i]) gameInput.setDown((GameInput::BUTTON)i);
            else gameInput.setUp((GameInput::BUTTON)i);
            break;

*/
           #endif

        default:
            if(platInput.keyDown[i]) gameInput.setDown((GameInput::BUTTON)i);
            else {
                bool changed = gameInput.getDown((GameInput::BUTTON)i);
                gameInput.setUp((GameInput::BUTTON)i);
                if(changed) getBehavSys()->onButtonRelease((GameInput::BUTTON)i);
            }

            break;
        }

       gameInput.keyTapped[i] = platInput.keyTapped[i];
       if(gameInput.keyTapped[i]) getBehavSys()->onButtonPress((GameInput::BUTTON)i);

	}

    if(gameInput.getDown(BUT_DPAD_U)) {
        gameInput._analogDir[0].y = -1.0f;
    } else if(gameInput.getDown(BUT_DPAD_D)) {
        gameInput._analogDir[0].y = 1.0f;
    } else gameInput._analogDir[0].y = 0.0f;

    if(gameInput.getDown(BUT_DPAD_L)) {
        gameInput._analogDir[0].x = -1.0f;
    } else if(gameInput.getDown(BUT_DPAD_R)) {
        gameInput._analogDir[0].x = 1.0f;
    } else gameInput._analogDir[0].x = 0.0f;


#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
    for(unsigned i = 0; i < GameInput::BUT_MAX; ++i) gameInput.setUp((GameInput::BUTTON)i);
    
    // ===== DREAMCAST CONTROLLER INPUT =====
    if(platInput.butDown[GY_CONT_DPAD_U]) gameInput.setDown(BUT_DPAD_U);
    else gameInput.setUp(BUT_DPAD_U);
        
    if(platInput.butDown[GY_CONT_DPAD_D]) gameInput.setDown(BUT_DPAD_D);
    else gameInput.setUp(BUT_DPAD_D);
                
    if(platInput.butDown[GY_CONT_DPAD_R]) gameInput.setDown(BUT_DPAD_R);
    else gameInput.setUp(BUT_DPAD_R);
                        
    if(platInput.butDown[GY_CONT_DPAD_L]) gameInput.setDown(BUT_DPAD_L);
    else gameInput.setUp(BUT_DPAD_L);
    
    if(platInput.butDown[GY_CONT_LTRIG]) gameInput.setDown(BUT_S);
    else gameInput.setUp(BUT_S);
    
    if(platInput.butDown[GY_CONT_RTRIG]) gameInput.setDown(BUT_W);
    else gameInput.setUp(BUT_W);
    
    //===== DREAMCAST KEYBOARD INPUT (HACK-Y!)=====
    maple_device_t *kbd = maple_enum_type(0, MAPLE_FUNC_KEYBOARD);
    int key;
    
    while((key = kbd_get_key()) != -1) { //returns ASCII value of key!
        
        switch(key) {
            case 'q':
                gameInput.setDown(BUT_Q);
                break;
            case 'a':
                gameInput.setDown(BUT_A);
                break;
            case 'w':
                gameInput.setDown(BUT_W);
                break;
            case 's':
                gameInput.setDown(BUT_S);
                break;
            case 'z':
                gameInput.setDown(BUT_Z);
                break;
            case 'x':
                gameInput.setDown(BUT_X);
                break;
            case '0':
                gameInput.setDown(BUT_0);
                break;
            case '1':
                gameInput.setDown(BUT_1);
                break;
            case '2':
                gameInput.setDown(BUT_2);
                break;
            case '3':
                gameInput.setDown(BUT_3);
                break;
            case '4':
                gameInput.setDown(BUT_4);
                break;
            case '5':
                gameInput.setDown(BUT_5);
                break;
            case '6':
                gameInput.setDown(BUT_6);
                break;
            case '7':
                gameInput.setDown(BUT_7);
                break;
            case '8':
                gameInput.setDown(BUT_8);
                break;
            case '9':
                gameInput.setDown(BUT_9);
                break;
            default:
                Debug::printf(Debug::DBG_VERBOSE, "Key Pressed - %04x", key);
        }
        
    }
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS

    if(platInput.up) gameInput.setDown(GameInput::BUT_DPAD_U);
    else gameInput.setUp(GameInput::BUT_DPAD_U);

    if(platInput.down) gameInput.setDown(GameInput::BUT_DPAD_D);
    else gameInput.setUp(GameInput::BUT_DPAD_D);

    if(platInput.left) gameInput.setDown(GameInput::BUT_DPAD_L);
    else gameInput.setUp(GameInput::BUT_DPAD_L);

    if(platInput.right) gameInput.setDown(GameInput::BUT_DPAD_R);
    else gameInput.setUp(GameInput::BUT_DPAD_R);

    if(platInput.shake) gameInput.setDown(GameInput::BUT_I);
    else gameInput.setUp(GameInput::BUT_I);

    gameInput._pinch = platInput.pinch;

    gameInput.acceleration = platInput.accelerometer;

    gameInput._analogDir[0] = platInput.cursorDir;


#else
	(void)platInput;
	(void)gameInput;
#endif
}

GameInput::GameInput(void) {
	clear();
}

void GameInput::clear(void) {
	for(int i = 0; i < BUT_MAX; ++i) {
		keyDown[i] = false;
		keyTapped[i] = false;
	}
}

void GameInput::setDown(GameInput::BUTTON key) {
	if(invalidKey(key)) return;
	keyDown[key] = true;
	keyTapped[key] = true;
}


void GameInput::setUp(GameInput::BUTTON key) {
	if(invalidKey(key)) return;
	keyDown[key] = false;
	keyTapped[key] = false;
}

bool GameInput::getDown(GameInput::BUTTON key) const {
	if(invalidKey(key)) return false;
	return keyDown[key];
}

bool GameInput::getTapped(GameInput::BUTTON key) const {
	if(invalidKey(key)) return false;
	return keyTapped[key];
}

//Debug, checks if supplied key is valid.
bool GameInput::invalidKey(GameInput::BUTTON key) const {
	if(key > BUT_MAX || key < 0) {
		return true;
	}
	else return	false;
}

}
