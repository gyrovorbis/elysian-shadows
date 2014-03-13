#include "elysian_dialog_system.hpp"
#include "elysian_entity.hpp"
#include "elysian_dialog.hpp"
#include "elysian_game_input.hpp"
#include "elysian_debug.hpp"
#include "elysian_collidable.hpp"
#include "elysian_entity.hpp"
#include "elysian_party_system.hpp"
#include "elysian_area.hpp"
#include "elysian_gui_system.hpp"
#include "elysian_camera_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_character.hpp"
#include "elysian_behavior_system.hpp"

#include <cmath>

namespace elysian {

//const static members
const GYVector2 DialogSystem::_maxNonmodalDist = { 200.0f, 200.0f };
const float DialogSystem::_nextCharTime = ELSYIAN_DIALOG_SYSTEM_CHAR_APPEAR_TIME;

DialogSystem::DialogSystem(void) {
	/*
	 Set the initial values for main text canvas.
	*/
	static GYColor4 canvColor1 = { 0, 0, 150, 255 };
	static GYColor4 canvColor2 = { 0, 0, 50, 255 };
	static GYColor4 canvColor3 = { 0, 0, 50, 255 };
	static GYColor4 canvColor4 = { 0, 0, 50, 255 };
	static GYColor4 optBoxColor = { 0, 0, 50, 255 };
	static GYColor4 cursorColor = { 0, 180, 180, 255 };

	_mainCanvas.transform.setSize(640.0f, 100.0f);
	_mainCanvas.transform.setPos(320.0f, 480.0f-50.0f, 200.0f);
	_mainCanvas.setVertexColor(0, canvColor1);
	_mainCanvas.setVertexColor(1, canvColor2);
	_mainCanvas.setVertexColor(2, canvColor3);
	_mainCanvas.setVertexColor(3, canvColor4);
	_mainCanvas.setVerticesAlpha(100, 100, 100, 100);

	_optionBox.setVertexColor(0, optBoxColor);
	_optionBox.setVertexColor(1, optBoxColor);
	_optionBox.setVertexColor(2, optBoxColor);
	_optionBox.setVertexColor(3, optBoxColor);
	_cursor.setVertexColor(0, cursorColor);
	_cursor.setVertexColor(1, cursorColor);
	_cursor.setVertexColor(2, cursorColor);
	_cursor.setVertexColor(3, cursorColor);
}

//MAKE THESE POSITIONS RELATIVE
void DialogSystem::render(void) {
	char buffer[ELYSIAN_DIALOG_PROMPT_OPTION_TEXT_BUFFER_SIZE];

	if(!_curDialog) return;

	GYVector3 basePos = _mainCanvas.transform._pos;

	//There is an ongoing conversation.
	float alpha = fabs(basePos.y-240.0f)/110.0f;
	if(alpha > 1.0f) alpha = 1.0f;
	else if (alpha < 0.0f) alpha = 0.0f;
		//Render canvas
		_mainCanvas.render();
		//Render Text
		for(unsigned int i = 0; i <= _currentLine; ++i) {
			getGuiSys()->twcen.setAlpha(255*alpha);
			getGuiSys()->twcen.transform.setPos(basePos.x-(_mainCanvas.transform._size.x/2.0f)+30.0f, basePos.y-(_mainCanvas.transform._size.y/2.0f)+35.0f+25.0f*i, _mainCanvas.transform._pos.z+1);
			if(i != _currentLine) getGuiSys()->twcen.drawStringf(Font::NO_FORMAT, Font::L_ALIGN, "%s", _textLines[i]);
			else {
				strncpy(buffer, _textLines[i], _currentCharacter);
				buffer[_currentCharacter] = '\0';
				getGuiSys()->twcen.drawStringf(Font::NO_FORMAT, Font::L_ALIGN, "%s", buffer);
				//getGuiSys()->twcen.drawStringf(Font::NO_FORMAT, Font::L_ALIGN, "fuuuuuck", buffer);
			}
		}

		//Render Nametag
		if(_curDialog && _boxState != BOX_INPUT && _curScreen.screen->getEntity()) {//Check top or bottom
			GYVector3 screenPos;
			getCameraSys()->transform(&screenPos, &_curScreen.screen->getEntity()->transform._pos);
			getGuiSys()->verdana.setAlpha(alpha*255);
			switch(_posState) {
				case POS_BOTTOM:
					getGuiSys()->verdana.transform.setPos(screenPos.x, basePos.y-(_mainCanvas.transform._size.y/2.0f)+5.0f, _mainCanvas.transform._pos.z+1);
					getGuiSys()->verdana.drawStringf(Font::NO_FORMAT, Font::C_ALIGN, "%s", _curScreen.screen->getEntity()->getName());
					break;
				case POS_MOVE_DOWN:
					break;
				case POS_TOP:
					getGuiSys()->verdana.transform.setPos(screenPos.x, basePos.y+(_mainCanvas.transform._size.y/2.0f)+5.0f, _mainCanvas.transform._pos.z+1);
					getGuiSys()->verdana.drawStringf(Font::NO_FORMAT, Font::C_ALIGN, "%s", _curScreen.screen->getEntity()->getName());
					break;
				case POS_MOVE_UP:
					break;

			}

		}

	//Render prompt
	if(_boxState == BOX_INPUT && _posState != POS_MOVE_UP && _posState != POS_MOVE_DOWN) {
		unsigned int maxLen = 0;

		for(unsigned int i = 0; i < _curScreen.prompt->getOptionCount(); ++i) {
			if(getGuiSys()->twcen.getStringPixelWidth(_curScreen.prompt->getOptionText(i)) > maxLen)
				maxLen = getGuiSys()->twcen.getStringPixelWidth(_curScreen.prompt->getOptionText(i));
		}

		_optionBox.transform.setSize(maxLen+10.0f, _curScreen.prompt->getOptionCount()*25.0f+10.0f);
		if(_posState == POS_TOP)
		_optionBox.transform.setPos((4.0f/5.0f)*_mainCanvas.transform._size.x-0.5f*_optionBox.transform._size.x, _mainCanvas.transform._pos.y+0.5*_optionBox.transform._size.y);
		else
		_optionBox.transform.setPos((4.0f/5.0f)*_mainCanvas.transform._size.x-0.5f*_optionBox.transform._size.x, _mainCanvas.transform._pos.y-0.5*_optionBox.transform._size.y);

		_optionBox.transform._pos.z = _mainCanvas.transform._pos.z+2;
		/*

		switch(_posState) {
			case POS_TOP:
				//_optionBox.transform.setPosition(
			case POS_BOTTOM:
				_optionBox.transform.setPosition(500.0f, 240.0f);
				break;
		}*/

		//Render option box
		//_optionBox.transform.setSize(200.0f, 150.0f);
		//_optionBox.setAlpha(0.5f, 0.5f, 0.5f, 0.5f);
		_optionBox.render();
		//Render options
		getGuiSys()->twcen.transform._pos.z += 3.0f;
		for(unsigned int i = 0; i < _curScreen.prompt->getOptionCount(); ++i) {
		//	if(_posState == POS_TOP)
			getGuiSys()->twcen.transform.setPos(_optionBox.transform._pos.x-0.5*_optionBox.transform._size.x+15.0f, _optionBox.transform._pos.y-0.5*_optionBox.transform._size.y + 25.0+ i*25.0f);
		//	else
			//guiSystem->twcen.transform.setPosition(_optionBox.transform.getPosition().x-0.5*_optionBox.transform.getSize().x+15.0f, _optionBox.transform.getPosition().y-0.5*_optionBox.transform.getSize().y + 25.0+ i*25.0f);
			getGuiSys()->twcen.drawStringf(Font::NO_FORMAT, Font::L_ALIGN, "%s", _curScreen.prompt->getOptionText(i));
		}
		//Render cursor
		_cursor.transform._pos.z = _mainCanvas.transform._pos.z+3;
		_cursor.transform.setSize(_optionBox.transform._size.x, 25.0f);
		_cursor.transform.setPos(_optionBox.transform._pos.x-0.5f*_optionBox.transform._size.x+0.5f*_cursor.transform._size.x, _optionBox.transform._pos.y-0.5f*_optionBox.transform._size.y+25.0f*_currentOption+0.5f*_cursor.transform._size.y+5.0f);
		_cursor.render();

	}
}


void DialogSystem::update(void) {
	if(!_curScreen.screen) return;

	//IF WE'RE SPEAKING TO AN ENTITY ----FUCK
	if(_curScreen.screen && _curScreen.screen->getEntity()) {
		FacePlayer();

		GYVector3 curPos;
		getCameraSys()->transform(&curPos, &_curScreen.screen->getEntity()->transform._pos);

		GYVector3 dist;
		gyVec3Sub(&dist, &getPartySys()->getCharacterPartyMember(0u)->transform._pos, &_curScreen.screen->getEntity()->transform._pos);
		if(fabs(dist.x) > _maxNonmodalDist.x || fabs(dist.y) > _maxNonmodalDist.y) {
			closeDialog();
			return;
		}

		switch(_posState) {
			case POS_BOTTOM:
				if(curPos.y > 350.0f) _posState = POS_MOVE_UP;
				break;
			case POS_TOP:
				if(curPos.y < 130.0f) _posState = POS_MOVE_DOWN;
				break;
			case POS_MOVE_UP:
				if(_mainCanvas.transform._pos.y <= 50.0f) {
					_mainCanvas.transform.setPos(_mainCanvas.transform._pos.x, 50.0f);
					_posState = POS_TOP;
				}
				else if(curPos.y < 130.0f) _posState = POS_MOVE_DOWN;
				else {
					float alpha = fabs(_mainCanvas.transform._pos.y-240.0f)/600.0f;// - 0.8f;
					if(alpha < 0.0f) alpha = 0.0f;
					_mainCanvas.transform.translate(0.0f, -(350.0f-130.0f)*getTimerSys()->getDeltaTime()*2.0f);
					_mainCanvas.setVerticesAlpha(alpha*255, alpha*255, alpha*255, alpha*255);
				}
				break;
			case POS_MOVE_DOWN:
				if(_mainCanvas.transform._pos.y >= 430.0f) {
					_mainCanvas.transform.setPos(_mainCanvas.transform._pos.x, 430.0f);
					_posState = POS_BOTTOM;
				}
				else if(curPos.y > 350.0f) _posState = POS_MOVE_UP;
				else {
					float alpha = fabs(_mainCanvas.transform._pos.y-240.0f)/600.0f;// - 0.8f;
					if(alpha < 0.0f) alpha = 0.0f;
					_mainCanvas.transform.translate(0.0f, (350.0f-130.0f)*getTimerSys()->getDeltaTime()*2.0f);
					_mainCanvas.setVerticesAlpha(alpha*255, alpha*255, alpha*255, alpha*255);
				}
				break;
		}

	}

	//UPDATE CHARACTER-BY-CHARACTER LOGIC
	switch(_boxState) {
		case BOX_OUTPUT:
			_characterTimer += getTimerSys()->getDeltaTime();
			if(_characterTimer >= _nextCharTime) {
				_currentCharacter += (unsigned int)(_characterTimer/_nextCharTime);
				_characterTimer = 0.0f;
			}
			if(_currentCharacter >= strlen(_textLines[_currentLine])) {
				++_currentLine;
				_currentCharacter = 0;
			}
			if(_currentLine > ELYSIAN_DIALOG_SYSTEM_MAX_LINES-1) {
				_currentLine = ELYSIAN_DIALOG_SYSTEM_MAX_LINES-1;
				_currentCharacter = strlen(_textLines[_currentLine]);
				_boxState = BOX_OUTPUT_END;
			}
			break;
		case BOX_INPUT:
			break;
		case BOX_OUTPUT_END:
			break;
	}
}

void DialogSystem::processInput(const GameInput &input) {

	//No active dialog
	if(!_curDialog) {
#if 0 //we aren't doing this polling anymore -- examinablesystem
		if(input.getTapped(GameInput::BUT_J)) {
			Entity *player = getPartySys()->getPlayablePartyMember(0u);

			for(unsigned int i = 0; i < _curArea->conversations.size(); ++i) {
				/*if(player->getCollidable()->isCollidingWith(*_curArea->conversations[i]->getEntity()->getCollidable())) {
					Debug::printf(Debug::VERBOSE, "Start the dialog with the %s, fucker!\n", _curArea->conversations[i]->getEntity()->getName());
					Open(*_curArea->conversations[i]);
				}*/
			}
		}
	#endif
	}
	//active dialog
	else {
		switch(_boxState) {
			case BOX_OUTPUT:
				if(input.getDown(GameInput::BUT_J)) {
					++_currentCharacter;
				}
				break;
			case BOX_OUTPUT_END:
				if(input.getTapped(GameInput::BUT_J)) {
					//input prompt
					if(_curScreen.screen->getScreenType() == DialogScreen::SCREEN_PROMPT) {
						_boxState = BOX_INPUT;
						setEngineState(ENGINE_MODAL_CONVERSATION);
						//cache lengths and shit of shit.
					}
					//no options
					else {
						NextScreen();
					}
				}
				break;
			case BOX_INPUT:
				//move down
				if(input.getTapped(GameInput::BUT_DPAD_D)) {
					++_currentOption;
					if(_currentOption > _curScreen.prompt->getOptionCount()-1) {
						_currentOption = _curScreen.prompt->getOptionCount()-1;
					}
				}
				//move up
				else if(input.getTapped(GameInput::BUT_DPAD_U)) {
					--_currentOption;
					if(_currentOption < 0) {
						_currentOption = 0;
					}
				}
				//select option
				if(input.getTapped(GameInput::BUT_J)) {
					NextScreen();
				}
				break;
		}
	}
}

void DialogSystem::NextScreen(void) {
	int nextIndex;

	switch(_boxState) {
		case BOX_OUTPUT_END:
			nextIndex = _curScreen.text->getNextIndex();
			break;
		case BOX_INPUT:
			nextIndex = _curScreen.prompt->getOptionNextIndex(_currentOption); //_currentConversationDialog->getScreen(_currentScreen)->getOption(_currentOption)->resultScreen;
			break;
	}

	//Invoke Lua trigger
	if(_curScreen.screen->isTrigger() && _curDialog->getTriggerEntity()) {
		getBehavSys()->onDialog(_curDialog->getTriggerEntity(),
								_curScreen.screen->getIndex(),
								_currentOption);

	}

	if(nextIndex == -1) {
		closeDialog();
	} else {
		_curScreen.screen = _curDialog->getScreen(nextIndex);
		_boxState = BOX_OUTPUT;
		_currentLine = _currentCharacter = 0;
		_characterTimer = 0.0f;
		_splitText();
		CheckModal();
	}
}

void DialogSystem::CheckModal(void) const {
	setEngineState((_curScreen.screen->getScreenType() == DialogScreen::SCREEN_PROMPT)? System::ENGINE_MODAL_CONVERSATION : System::ENGINE_CONVERSATION);
}

void DialogSystem::FacePlayer(void) const {
	if(!_curScreen.screen->getEntity() || !_curScreen.screen->getEntity()->getCharacter()) return;
	_curScreen.screen->getEntity()->getCharacter()->faceEntity(*getPartySys()->getCharacterPartyMember(0u));
}

void DialogSystem::closeDialog(void) {
	//if(_dynamicDialog) delete _currentConversationDialog;
	_curDialog = nullptr;
	_curScreen.screen = nullptr;
	_currentScreen = _currentLine = _currentCharacter = _currentOption = 0;
	_characterTimer = 0.0f;
	//_dynamicDialog = false;
	_boxState = BOX_OUTPUT;
	setEngineState(System::ENGINE_NORMAL);
	_posState = POS_BOTTOM;
	//_mainCanvas.transform.setPosition(_mainCanvas.transform._pos.x, 430.0f);
	_mainCanvas.transform._pos.y = 430.0f;
	getGuiSys()->adventureLog.enable();
}

void DialogSystem::_splitText(void) {
	char buffer[ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE];
	unsigned lineCount = 0;

	strcpy(buffer, _curScreen.screen->getText());
	const char *str = strtok(buffer, "\n");

	//Divide string into individual lines
	while(str) {
		strcpy(_textLines[lineCount], str);
		str = strtok(nullptr, "\n");
		++lineCount;
		assert(lineCount <= ELYSIAN_DIALOG_SYSTEM_MAX_LINES);
	}

	//NULL out empty lines
	for(; lineCount < ELYSIAN_DIALOG_SYSTEM_MAX_LINES; ++lineCount) _textLines[lineCount][0] = 0;
}

void DialogSystem::startDialog(Dialog *const dialog) {
	closeDialog();
	_curDialog = dialog;
	_curScreen.screen = dialog->getScreen(0);
	FacePlayer();
	_splitText();
	CheckModal();
	getGuiSys()->adventureLog.disable();

}


#if 0
void DialogSystem::Open(Entity &entity) {
	Close();
	Conversation *convo = entity.getConversation();
	if(!convo) {
		Debug::printf(Debug::CRITICAL, "Entity '%s' has no conversation component!\n", entity.getName());
		_currentConversation = NULL;
		return;
	}

	_currentConversation = convo;
	_currentConversationDialog = convo->getDialog();
	FacePlayer();
	CheckModal();
	getGuiSys()->adventureLog.disable();

}

void ConversationSystem::Open(Conversation &convo) {
	Close();
	_currentConversation = &convo;
	_currentConversationDialog = convo.getDialog();
	FacePlayer();
	CheckModal();
	getGuiSys()->adventureLog.disable();
}

void ConversationSystem::Open(ConversationDialog &dialog) {
	Close();
	_currentConversationDialog = &dialog;
	CheckModal();
	getGuiSys()->adventureLog.disable();
}

void ConversationSystem::Openf(bool modal, const char *text, ...) {
	va_list argptr;
	char buffer[ELYSIAN_CONV_SCREEN_MAX_CHARS];
	va_start(argptr,text);
	vsprintf(buffer,text,argptr);
	va_end(argptr);
	Open(modal, buffer);
}

void ConversationSystem::Open(bool modal, const char *buffer) {
	ConversationScreen *screen;
	Close();
	_currentConversationDialog = new ConversationDialog(1);
	screen = _currentConversationDialog->getScreen(1);
	screen->_modal = modal;
	strcpy(screen->_line[0], buffer);
	_dynamicDialog = true;

	setEngineState((modal)? ENGINE_MODAL_CONVERSATION : ENGINE_CONVERSATION);
	getGuiSys()->adventureLog.disable();
}

void ConversationSystem::Close(void) {
	if(_dynamicDialog) delete _currentConversationDialog;
	_currentConversation = NULL;
	_currentConversationDialog = NULL;
	_currentScreen = _currentLine = _currentCharacter = _currentOption = 0;
	_characterTimer = 0.0f;
	_dynamicDialog = false;
	_boxState = BOX_OUTPUT;
	setEngineState(System::ENGINE_NORMAL);
	_posState = POS_BOTTOM;
	//_mainCanvas.transform.setPosition(_mainCanvas.transform._pos.x, 430.0f);
	_mainCanvas.transform._pos.y = 430.0f;
	getGuiSys()->adventureLog.enable();
}

#endif

Dialog *DialogSystem::getDialog(void) const {
	return _curDialog;
}

bool DialogSystem::isOpen(void) const {
	return _curDialog? true : false;
}


}
