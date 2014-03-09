#ifndef ELYSIAN_DIALOG_SYSTEM_HPP
#define ELYSIAN_DIALOG_SYSTEM_HPP

#include "elysian_system.hpp"

namespace elysian {


	class DialogSystem: public System {
	public:
		enum POS_STATE { POS_BOTTOM, POS_TOP, POS_MOVE_UP, POS_MOVE_DOWN };
		enum BOX_STATE { BOX_OUTPUT, BOX_OUTPUT_END, BOX_INPUT };
	private:
		static const GYVector2 _maxNonmodalDist;
		static const float _nextCharTime;
		int _currentScreen;
		unsigned int _currentLine;
		unsigned int _currentCharacter;
		int _currentOption;
		float _characterTimer;
		Conversation* _currentConversation;
		ConversationDialog* _currentConversationDialog;
		PartySystem* _partySystem;
		GuiSystem* _guiSystem;
		bool _dynamicDialog;
		POS_STATE _posState;
		BOX_STATE _boxState;

		void NextScreen(void);
		void CheckModal(void) const;
		void FacePlayer(void) const;

		// ==== GUI SHIT ====
		QuadSprite _mainCanvas;
		Rect _optionBox;
		Rect _border[2];
		Rect _cursor;

	public:
		DialogSystem(void);
		void update(void);
		void render(void);
		void processInput(const GameInput &input);


		//Quick helper functions that use default values internally (mostly debugging)
		void	openText(const char*const str);
		//void openTextf(const char*const tex, ...)
		void	openTextAt(const char*const str, const float worldX, const float worldY);
		void	openTextFrom(str, Entity);

		//Used for segmented/dynamic dialog
		void	openTextScreen(DialogText&);
		void	openPromptScreen(DialogPrompt&);

		void	startDialog(Dialog&);
		void	pauseDialog(Dialog&);   //stops, but keeps state information to resume later

	   // unsigned getActiveScreenCount();
		void       setEnabled(const bool val);









		void Openf(bool modal, const char *text, ...);

		void Open(Entity& entity);
		void Open(Conversation& convo);
		void Open(ConversationDialog& dialog);
		void Open(bool modal, const char* text);
		void Close(void);

		Conversation* getConversation(void) const;
		bool isOpen(void) const;
		//void setTexture();
	};

}


#endif
