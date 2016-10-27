#ifndef ELYSIAN_DIALOG_SYSTEM_HPP
#define ELYSIAN_DIALOG_SYSTEM_HPP

#include "elysian_system.hpp"
#include <crtp/elysian_dialog.hpp>
#include <elysian_quad_sprite.hpp>
#include <gyro_wav_api.h>

#define ELSYIAN_DIALOG_SYSTEM_CHAR_APPEAR_TIME      0.05f
#define ELYSIAN_DIALOG_SYSTEM_MAX_LINES             3

#define ELYSIAN_DIALOG_SYSTEM_MARGIN_SIZE           0.1f
#define ELYSIAN_DIALOG_SYSTEM_DIST_MARGIN_SIZE      0.2f

#define ELYSIAN_DIALOG_SYSTEM_WIDTH                 640.0f
#define ELYSIAN_DIALOG_SYSTEM_HEIGHT                110.0f

#define ELYSIAN_DIALOG_SYSTEM_MAX_NONMODAL_DIST     200.0f

#define ELYSIAN_DIALOG_SYSTEM_SFX_OPEN              "/assets/audio/sfx/UI/dialogBoxAppear.wav"
#define ELYSIAN_DIALOG_SYSTEM_SFX_CLOSE             "/assets/audio/sfx/UI/dialogBoxDispose.wav"
#define ELYSIAN_DIALOG_SYSTEM_SFX_TEXT              "/assets/audio/sfx/UI/scrollingText_01.wav"
#define ELYSIAN_DIALOG_SYSTEM_SFX_SELECT            "/assets/audio/sfx/UI/menuSelect_01.wav"

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class GameInput;
	class EventMonitor;

	class DialogSystem: public System {
	public:
        enum POS_STATE { POS_BOTTOM, POS_TOP, POS_MOVE_UP, POS_MOVE_DOWN };
        enum BOX_STATE { BOX_OUTPUT, BOX_OUTPUT_END, BOX_INPUT };
	private:

        static constexpr const float _nextCharTime          = ELSYIAN_DIALOG_SYSTEM_CHAR_APPEAR_TIME;
        static constexpr const float _maxNonmodalDist       = ELYSIAN_DIALOG_SYSTEM_MAX_NONMODAL_DIST;

		// ==== INTERNAL DISPLAY STATE ====
        int                         _currentScreen          = 0;
        unsigned int                _currentLine            = 0;
        unsigned int                _currentCharacter       = 0;
        float                       _characterTimer         = 0.0f;

		char _textLines[3][ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE]; //current text string split into lines

		// ==== USER STATE =====
        int                         _currentOption          = 0;

        Dialog*                     _curDialog              = nullptr;
		union Screen {
            DialogScreen*           screen                  = nullptr;
            //DialogText*           text;
            //DialogPrompt*         prompt;
        }                           _curScreen, _prevScreen;
        EventMonitor*               _monitor                = nullptr;

		// ==== GUI SHIT ====
        QuadSprite                  _mainCanvas; //gets and sets for Lua to fuck with this shit!
        QuadSprite                  _optionBox;
        QuadSprite                  _border[2];
        QuadSprite                  _cursor;

        POS_STATE                   _posState               = POS_BOTTOM;
        BOX_STATE                   _boxState               = BOX_OUTPUT;

        GYWav*                      _sfxOpen                = nullptr;
        GYWav*                      _sfxClose               = nullptr;
        GYWav*                      _sfxText                = nullptr;
        GYWav*                      _sfxSelect              = nullptr;

        bool                        _advLogWasEnabled       = true;


        void                        _nextScreen(void);
        void                        _checkModal(void) const;
        void                        _facePlayer(void) const;
        void                        _updateCamera(void);
        void                        _splitText(void);

	public:
                                    DialogSystem(void);
                                    ~DialogSystem(void);
        bool                        loadAssets(void);
        void                        update(void);
        void                        render(void);
        void                        processInput(void);

		//Quick helper functions that use default values internally (mostly debugging)
        void                        openText(const char*const str);
		//void openTextf(const char*const tex, ...)
        void                        openTextAt(const char*const str, const float worldX, const float worldY);
        void                        openTextFrom(const char*const str, const Entity*const entity);

		//Used for segmented/dynamic dialog
        void                        openTextScreen(DialogText& screen);
        void                        openPromptScreen(DialogPrompt& screen);

        void                        startDialog(Dialog*const dialog, EventMonitor*const monitor=nullptr);
        void                        pauseDialog(void);   //stops, but keeps state information to resume later
        void                        closeDialog(void);

	   // unsigned getActiveScreenCount();
        void                        setEnabled(const bool val);
        Dialog*                     getDialog(void) const;
        bool                        isOpen(void) const;

	};

}


#endif
