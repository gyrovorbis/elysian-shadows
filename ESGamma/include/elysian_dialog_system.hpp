#ifndef ELYSIAN_DIALOG_SYSTEM_HPP
#define ELYSIAN_DIALOG_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_dialog.hpp"
#include "elysian_quad_sprite.hpp"

#define ELSYIAN_DIALOG_SYSTEM_CHAR_APPEAR_TIME	0.05f
#define ELYSIAN_DIALOG_SYSTEM_MAX_LINES			3

#define ELYSIAN_DIALOG_SYSTEM_MARGIN_SIZE		0.2f
#define ELYSIAN_DIALOG_SYSTEM_DIST_MARGIN_SIZE	0.3f

namespace elysian {

	class Dialog;
	class GameInput;
	class Entity;
	class EventMonitor;

	class DialogSystem: public System {
	public:
		enum POS_STATE { POS_BOTTOM, POS_TOP, POS_MOVE_UP, POS_MOVE_DOWN };
		enum BOX_STATE { BOX_OUTPUT, BOX_OUTPUT_END, BOX_INPUT };
	private:

		static const float _nextCharTime;



		static const GYVector2 _maxNonmodalDist;

		// ==== INTERNAL DISPLAY STATE ====
		int				_currentScreen		= 0;
		unsigned int	_currentLine		= 0;
		unsigned int	_currentCharacter	= 0;
		float			_characterTimer		= 0.0f;

		char _textLines[3][ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE]; //current text string split into lines

		// ==== USER STATE =====
		int				_currentOption		= 0;

		Dialog*			_curDialog			= nullptr;
		union Screen {
			DialogScreen*	screen			= nullptr;
			DialogText*		text;
			DialogPrompt*	prompt;
		} _curScreen, _prevScreen;
		EventMonitor*	_monitor			= nullptr;
		//ConversationDialog* _currentConversationDialog; WTF?

		//bool _dynamicDialog = false; WTF?


		//DO SOMETHING, MAKE PRIVATE?
		void NextScreen(void);
		void CheckModal(void) const;
		void FacePlayer(void) const;
		void _updateCamera(void);

		void _splitText(void);
		//void _checkForDialog(void);

		// ==== GUI SHIT ====
		QuadSprite _mainCanvas; //gets and sets for Lua to fuck with this shit!
		QuadSprite _optionBox;
		QuadSprite _border[2];
		QuadSprite _cursor;

		POS_STATE _posState = POS_BOTTOM;
		BOX_STATE _boxState = BOX_OUTPUT;

	public:
				DialogSystem(void);
		void	update(void);
		void	render(void);
		void	processInput(const GameInput &input);

		//Quick helper functions that use default values internally (mostly debugging)
		void	openText(const char*const str);
		//void openTextf(const char*const tex, ...)
		void	openTextAt(const char*const str, const float worldX, const float worldY);
		void	openTextFrom(const char*const str, const Entity*const entity);

		//Used for segmented/dynamic dialog
		void	openTextScreen(DialogText& screen);
		void	openPromptScreen(DialogPrompt& screen);

		void	startDialog(Dialog*const dialog, EventMonitor*const monitor=nullptr);
		void	pauseDialog(void);   //stops, but keeps state information to resume later
		void	closeDialog(void);

	   // unsigned getActiveScreenCount();
		void	setEnabled(const bool val);
		Dialog* getDialog(void) const;
		bool isOpen(void) const;






#if 0

		void Openf(bool modal, const char *text, ...);

		void Open(Entity& entity);
		void Open(Conversation& convo);
		void Open(ConversationDialog& dialog);
		void Open(bool modal, const char* text);
		void Close(void);
#endif

		//void setTexture();
	};

}


#endif
