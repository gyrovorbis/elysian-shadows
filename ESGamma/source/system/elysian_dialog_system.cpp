#include "elysian_dialog_system.hpp"
#include <crtp/elysian_entity.hpp>
#include <level/elysian_dialog.hpp>
#include "elysian_game_input.hpp"
#include "elysian_debug.hpp"
#include <crtp/elysian_collidable.hpp>
#include <crtp/elysian_entity.hpp>
#include "elysian_party_system.hpp"
#include <crtp/elysian_area.hpp>
#include "elysian_gui_system.hpp"
#include <crtp/elysian_camera_system.hpp>
#include "elysian_timer_system.hpp"
#include <crtp/elysian_character.hpp>
#include "elysian_behavior_system.hpp"
#include "elysian_audio_system.hpp"

#include <cmath>
#include <gyro_audio_api.h>

namespace elysian {

//const static members
const float DialogSystem::_maxNonmodalDist = ELYSIAN_DIALOG_SYSTEM_MAX_NONMODAL_DIST;
const float DialogSystem::_nextCharTime = ELSYIAN_DIALOG_SYSTEM_CHAR_APPEAR_TIME;

DialogSystem::DialogSystem(void) {
	/*
	 Set the initial values for main text canvas.4
	*/
    static const GYColor4 canvColor1 = { 0, 0, 0, 255 };
    static const GYColor4 canvColor2 = { 0, 0, 0, 255 };
    static const GYColor4 canvColor3 = { 0, 0, 0, 255 };
    static const GYColor4 canvColor4 = { 0, 0, 0, 255 };
    static const GYColor4 optBoxColor = { 0, 0, 0, 255 };
    static const GYColor4 cursorColor = { 0, 180, 180, 100 };
    static const GYColor4 borderColor = { 100, 100, 100, 50 };

	unsigned width, height;
	getGuiSys()->getResolution(width, height);

    _mainCanvas._transform.setSize(ELYSIAN_DIALOG_SYSTEM_WIDTH, ELYSIAN_DIALOG_SYSTEM_HEIGHT);
    //_mainCanvas._transform.setPos(ELYSIAN_DIALOG_SYSTEM_WIDTH*0.5f, ELYSIAN_DIALOG_SYSTEM_HEIGHT*0.5f, 6.0f);
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

    _border[0].setVertexColor(0, borderColor);
    _border[0].setVertexColor(1, borderColor);
    _border[0].setVertexColor(2, borderColor);
    _border[0]._transform.setSize(ELYSIAN_DIALOG_SYSTEM_WIDTH, 2.0f);
    _border[0]._transform._pos.x = ELYSIAN_DIALOG_SYSTEM_WIDTH*0.5f;
    _border[0].setVertexColor(3, borderColor);

    _border[1].setVertexColor(0, borderColor);
    _border[1].setVertexColor(1, borderColor);
    _border[1].setVertexColor(2, borderColor);
    _border[1]._transform.setSize(2.0f, ELYSIAN_DIALOG_SYSTEM_HEIGHT+2.0f);
    _border[1]._transform._pos.x = ELYSIAN_DIALOG_SYSTEM_WIDTH+1.0f;
    _border[1].setVertexColor(3, borderColor);

    _curScreen.screen = nullptr;
}

DialogSystem::~DialogSystem(void) {
    gyAudWavDestroy(_scrollDownSFX);
    gyAudWavDestroy(_scrollUpSFX);

}

bool DialogSystem::loadAssets(void) {
	bool success = true;

	_scrollDownSFX = gyAudWavCreate(ELYSIAN_DIALOG_SYSTEM_SCROLL_DOWN_SFX);
	if(!_scrollDownSFX) Debug::printf(Debug::LOG_WARNING, "Could not load - %s", ELYSIAN_DIALOG_SYSTEM_SCROLL_DOWN_SFX);
	_scrollUpSFX = gyAudWavCreate(ELYSIAN_DIALOG_SYSTEM_SCROLL_UP_SFX);
	if(!_scrollUpSFX) Debug::printf(Debug::LOG_WARNING, "Could not load - %s", ELYSIAN_DIALOG_SYSTEM_SCROLL_UP_SFX);

	return success;
}

//MAKE THESE POSITIONS RELATIVE
void DialogSystem::render(void) {
	char buffer[ELYSIAN_DIALOG_PROMPT_OPTION_TEXT_BUFFER_SIZE];
    unsigned width, height;
    getGuiSys()->getResolution(width, height);

    auto& font = getGuiSys()->pragmata18;
    auto& font2 = getGuiSys()->pragmata22;

	if(!_curDialog) return;

    font.setColor(255, 255, 255, 255);

    //getGuiSys()->getResolution(width, height);
    //_mainCanvas._transform._pos.x = width*0.5f;

    GYVector3 basePos = _mainCanvas._transform._pos;

    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);

	//There is an ongoing conversation.
	float alpha = fabs(basePos.y-240.0f)/110.0f;
	if(alpha > 1.0f) alpha = 1.0f;
	else if (alpha < 0.0f) alpha = 0.0f;
		//Render canvas
		_mainCanvas.render();
        _border[0].render();
        if(width > 640) _border[1].render();
		//Render Text
		for(unsigned int i = 0; i <= _currentLine; ++i) {
            font.setAlpha(255*alpha);
            font._transform.setPos(basePos.x-(_mainCanvas._transform._size.x/2.0f)+40.0f, basePos.y-(_mainCanvas._transform._size.y/2.0f)+35.0f+25.0f*i, _mainCanvas._transform._pos.z+1);
            if(i != _currentLine) font.drawString(Font::L_ALIGN, _textLines[i]);
			else {
				strncpy(buffer, _textLines[i], _currentCharacter);
				buffer[_currentCharacter] = '\0';
                font.drawString(Font::L_ALIGN, buffer);
				//getGuiSys()->twcen.drawStringf(Font::NO_FORMAT, Font::L_ALIGN, "fuuuuuck", buffer);
			}
		}

		//Render Nametag
		if(_curDialog && _boxState != BOX_INPUT && _curScreen.screen->getEntity()) {//Check top or bottom
            //GYVector3 screenPos;
            //getCameraSys()->transform(&screenPos, &_curScreen.screen->getEntity()->_transform._pos);
            font2.setColor(255, 255, 255, alpha*255);
			switch(_posState) {
				case POS_BOTTOM:
                    font2._transform.setPos(25.0f, basePos.y-0.5f*ELYSIAN_DIALOG_SYSTEM_HEIGHT, _mainCanvas._transform._pos.z+1);
                    font2.drawString(Font::L_ALIGN, _curScreen.screen->getEntity()->getDispName());
					break;
				case POS_MOVE_DOWN:
					break;
				case POS_TOP:
                   // getGuiSys()->verdana._transform.setPos(screenPos.x, basePos.y+(_mainCanvas._transform._size.y/2.0f)+5.0f, _mainCanvas._transform._pos.z+1);
                //	getGuiSys()->verdana.drawStringf(Font::NO_FORMAT, Font::C_ALIGN, "%s", _curScreen.screen->getEntity()->getName());
					break;
				case POS_MOVE_UP:
					break;

			}

		}

	//Render prompt
	if(_boxState == BOX_INPUT && _posState != POS_MOVE_UP && _posState != POS_MOVE_DOWN) {
		unsigned int maxLen = 0;

		for(unsigned int i = 0; i < _curScreen.prompt->getOptionCount(); ++i) {
            if(font.getStringPixelWidth(_curScreen.prompt->getOptionText(i)) > maxLen)
                maxLen = font.getStringPixelWidth(_curScreen.prompt->getOptionText(i));
		}

        _optionBox._transform.setSize(maxLen+10.0f, _curScreen.prompt->getOptionCount()*25.0f+10.0f);
		if(_posState == POS_TOP)
        _optionBox._transform.setPos((4.0f/5.0f)*_mainCanvas._transform._size.x-0.5f*_optionBox._transform._size.x, _mainCanvas._transform._pos.y+0.5*_optionBox._transform._size.y);
		else
        _optionBox._transform.setPos((4.0f/5.0f)*_mainCanvas._transform._size.x-0.5f*_optionBox._transform._size.x, _mainCanvas._transform._pos.y-0.5*_optionBox._transform._size.y);

        _optionBox._transform._pos.z = _mainCanvas._transform._pos.z+2;
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
        font._transform._pos.z += 3.0f;
		for(unsigned int i = 0; i < _curScreen.prompt->getOptionCount(); ++i) {
		//	if(_posState == POS_TOP)
            font._transform.setPos(_optionBox._transform._pos.x-0.5*_optionBox._transform._size.x+15.0f, _optionBox._transform._pos.y-0.5*_optionBox._transform._size.y + 25.0+ i*25.0f);
		//	else
			//guiSystem->twcen.transform.setPosition(_optionBox.transform.getPosition().x-0.5*_optionBox.transform.getSize().x+15.0f, _optionBox.transform.getPosition().y-0.5*_optionBox.transform.getSize().y + 25.0+ i*25.0f);
            font.drawString(Font::L_ALIGN, _curScreen.prompt->getOptionText(i));
		}
		//Render cursor
        _cursor._transform._pos.z = _mainCanvas._transform._pos.z+3;
        _cursor._transform.setSize(_optionBox._transform._size.x, 25.0f);
        _cursor._transform.setPos(_optionBox._transform._pos.x-0.5f*_optionBox._transform._size.x+0.5f*_cursor._transform._size.x, _optionBox._transform._pos.y-0.5f*_optionBox._transform._size.y+25.0f*_currentOption+0.5f*_cursor._transform._size.y+5.0f);
		_cursor.render();

	}
}


void DialogSystem::update(void) {
	if(!_curScreen.screen) return;

    unsigned width, height;
    getGuiSys()->getResolution(width, height);

    _mainCanvas._transform.setPos(ELYSIAN_DIALOG_SYSTEM_WIDTH*0.5f, (float)height-ELYSIAN_DIALOG_SYSTEM_HEIGHT*0.5f, 6.0f);
    _border[0]._transform._pos.y = (float)height-ELYSIAN_DIALOG_SYSTEM_HEIGHT-_border[0]._transform._size.y;
    _border[1]._transform._pos.y = height-ELYSIAN_DIALOG_SYSTEM_HEIGHT*0.5f-1.0f;

	//IF WE'RE SPEAKING TO AN ENTITY ----FUCK
	if(_curScreen.screen && _curScreen.screen->getEntity()) {
		FacePlayer();

        const float dist = gyVec3Dist(&getPartySys()->getCharacterPartyMember(0u)->_transform._pos, &_curScreen.screen->getEntity()->_transform._pos);

        if(dist > _maxNonmodalDist) {
			_prevScreen.screen = _curScreen.screen;
			_curScreen.screen = nullptr;
			closeDialog();
			return;
		}
#if 0
/*
        _mainCanvas._transform._pos.x = curPos.x;
        if(_mainCanvas._transform._pos.x- _mainCanvas._transform._size.x*0.5f < 0)
            _mainCanvas._transform._pos.x = _mainCanvas._transform._size.x*0.5f;
        else if (_mainCanvas._transform._pos.x + _mainCanvas._transform._size.x*0.5f > width)
            _mainCanvas._transform._pos.x = width - _mainCanvas._transform._size.x*0.5f;
*/
		const float yEntityTop = (float)height*ELYSIAN_DIALOG_SYSTEM_DIST_MARGIN_SIZE;
		const float yEntityBottom = (float)height;// - (float)height*ELYSIAN_DIALOG_SYSTEM_DIST_MARGIN_SIZE;

		const float yTop = (float)height*ELYSIAN_DIALOG_SYSTEM_MARGIN_SIZE;
		const float yBottom = (float)height;// - (float)height*ELYSIAN_DIALOG_SYSTEM_MARGIN_SIZE;

		switch(_posState) {
			case POS_BOTTOM:
                //if(curPos.y > yEntityBottom) _posState = POS_MOVE_UP;
				break;
			case POS_TOP:
				if(curPos.y < yEntityTop) _posState = POS_MOVE_DOWN;
				break;
			case POS_MOVE_UP:
                if(_mainCanvas._transform._pos.y <= yTop) {
                    //_mainCanvas._transform.setPos(_mainCanvas._transform._pos.x, yTop);
					_posState = POS_TOP;
				}
				else if(curPos.y < yTop) _posState = POS_MOVE_DOWN;
				else {
                    float alpha = fabs(_mainCanvas._transform._pos.y-240.0f)/600.0f;// - 0.8f;
					if(alpha < 0.0f) alpha = 0.0f;
                    _mainCanvas._transform.translate(0.0f, -(350.0f-130.0f)*getTimerSys()->getDeltaTime()*2.0f);
					_mainCanvas.setVerticesAlpha(alpha*255, alpha*255, alpha*255, alpha*255);
				}
				break;
			case POS_MOVE_DOWN:
                if(_mainCanvas._transform._pos.y >= yBottom) {
                    //_mainCanvas._transform.setPos(_mainCanvas._transform._pos.x, yBottom);
					_posState = POS_BOTTOM;
				}
                //else if(curPos.y > yBottom) _posState = POS_MOVE_UP;
				else {
                    float alpha = fabs(_mainCanvas._transform._pos.y-240.0f)/600.0f;// - 0.8f;
					if(alpha < 0.0f) alpha = 0.0f;
                    _mainCanvas._transform.translate(0.0f, (350.0f-130.0f)*getTimerSys()->getDeltaTime()*2.0f);
					_mainCanvas.setVerticesAlpha(alpha*255, alpha*255, alpha*255, alpha*255);
				}
				break;
		}
#endif

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
					Debug::printf(Debug::LOG_VERBOSE, "Start the dialog with the %s, fucker!\n", _curArea->conversations[i]->getEntity()->getName());
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
					if(_currentOption > (int)_curScreen.prompt->getOptionCount()-1) {
						_currentOption = (int)_curScreen.prompt->getOptionCount()-1;
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

	getAudioSys()->playSfxChannel(-1, *_scrollDownSFX);

	switch(_boxState) {
		case BOX_OUTPUT_END:
			nextIndex = _curScreen.text->getNextIndex();
			break;
		case BOX_INPUT:
			nextIndex = _curScreen.prompt->getOptionNextIndex(_currentOption); //_currentConversationDialog->getScreen(_currentScreen)->getOption(_currentOption)->resultScreen;
			break;
		default:
			nextIndex = -1; //shit's reested.
		break;
	}

	//Invoke Lua trigger
	if(_curScreen.screen->isTrigger() && _curDialog->getTriggerEntity()) {
		getBehavSys()->onDialog(_curDialog->getTriggerEntity(),
								_curScreen.screen->getIndex(),
								_currentOption);

	}

	_prevScreen = _curScreen;
	if(nextIndex == -1) {
		_curScreen.screen = nullptr;
		closeDialog();
	} else {
		_curScreen.screen = _curDialog->getScreen(nextIndex);
		_boxState = BOX_OUTPUT;
		_currentLine = _currentCharacter = 0;
		_characterTimer = 0.0f;
		_updateCamera();
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
	if(_monitor) _monitor->onFinish();
	_updateCamera();
	//if(_dynamicDialog) delete _currentConversationDialog;
	_curDialog = nullptr;
	_curScreen.screen = nullptr;
	_prevScreen.screen = nullptr;
	_monitor = nullptr;
	_currentScreen = _currentLine = _currentCharacter = _currentOption = 0;
	_characterTimer = 0.0f;
	//_dynamicDialog = false;
	_boxState = BOX_OUTPUT;
	setEngineState(System::ENGINE_NORMAL);
	_posState = POS_BOTTOM;
	//_mainCanvas.transform.setPosition(_mainCanvas.transform._pos.x, 430.0f);
	//_mainCanvas.transform._pos.y = 430.0f;
    //getGuiSys()->adventureLog.setEnabled(_advLogWasEnabled);
    getGuiSys()->fadeFgLayer(1.0f, true);
}

void DialogSystem::_updateCamera(void) {



	if(_curScreen.screen && _curScreen.screen->getEntity()) {
		getCameraSys()->setBaseScale(2.5f);
		getCameraSys()->addEntity(_curScreen.screen->getEntity());
    } else if(_prevScreen.screen && _prevScreen.screen->getEntity()) {
        getCameraSys()->setBaseScale(1.5f);
        getCameraSys()->removeEntity(_prevScreen.screen->getEntity());
    }

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

void DialogSystem::startDialog(Dialog *const dialog, EventMonitor*const monitor) {
	closeDialog();
	_monitor = monitor;
	_curDialog = dialog;
	_curScreen.screen = dialog->getScreen(0);
	getAudioSys()->playSfxChannel(-1, *_scrollUpSFX);
	_updateCamera();
	FacePlayer();
	_splitText();
	CheckModal();
    _advLogWasEnabled = getGuiSys()->adventureLog.isEnabled();
    //getGuiSys()->adventureLog.setEnabled(false);
    getGuiSys()->fadeFgLayer(1.0f, false);
}


#if 0
void DialogSystem::Open(Entity &entity) {
	Close();
	Conversation *convo = entity.getConversation();
	if(!convo) {
		Debug::printf(Debug::LOG_WARNING, "Entity '%s' has no conversation component!\n", entity.getName());
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
