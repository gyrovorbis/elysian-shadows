#include "elysian_adventure_log.hpp"
#include "elysian_system.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_quad_sprite.hpp"
#include "elysian_font.hpp"
#include <string.h>
#include <stdint.h>

namespace elysian {

const float AdventureLog::_exitFadeTime = 1.0f;

/*
 Does a good deal more than just copy the line to the Adventure Log. 
 Due to the complexity of the Update and Draw functions, error checking
 is handled here: Full log and string is too big for a single line.
 */
void AdventureLog::addLine(const TEXT_TYPE lineType, const char *line) {
    if(_endIndex == ELYSIAN_ADV_LOG_LINES-1) { //The entire log is COMPLETELY full. Problem.
		
        //Manually add this to the beginning to prevent bitchy recursion:
        strcpy(_log[0], "Log file overflow! Resetting...");
        _type[0] = TEXT_ERROR;
        
        /* Push everything visible at the bottom of the log to the
		 top+1 and start overwriting everything already there. */
        for(int i = _startIndex; i <= _endIndex; ++i) { 
            //Copy the end of the file to the beginning.
            strcpy(_log[i-_startIndex+1], _log[i]);
			_type[i-_startIndex+1] = _type[i];	//FIXME THIS LINE VERY PROBLEMATIC
			//FIXME cont used to be commented but I uncommented it just to see and no problems yet --mar
        }
        //Move our indices back to the beginning of the logfile.
        _endIndex = _endIndex-_startIndex;
        _startIndex = 0;
    }
	//Hack as fuck. FIXME!!!
	if(_endIndex - _startIndex > 5) {
		_startIndex += (_endIndex-_startIndex)-5;
	}
    const size_t length = strlen(line);
    if(length >= ELYSIAN_ADV_LOG_CHARS_PER_LINE) { //Shit, we got a problem!
        //Break the bastard down into multiple lines.
		for(int i = 0; i < (int)length; i += ELYSIAN_ADV_LOG_CHARS_PER_LINE-1) {
            //Add this chunk as its own line.
			if (_endIndex >= ELYSIAN_ADV_LOG_LINES)	//Prevent overflow
				_endIndex -= ELYSIAN_ADV_LOG_LINES;
            strncpy(_log[++_endIndex], line+i, ELYSIAN_ADV_LOG_CHARS_PER_LINE-1);
            _type[_endIndex] = lineType;  
        }
    }
    else { //Fits on a single line, we good!
		if (_endIndex >= ELYSIAN_ADV_LOG_LINES)	//Prevent overflow
			_endIndex -= ELYSIAN_ADV_LOG_LINES;
        strcpy(_log[_endIndex], line);
        _type[_endIndex++] = lineType;    
    }
}

/*
 
 */
void AdventureLog::update(void) {
	if(_exitState == EXIT_DISABLED) return;

	float deltaTime = getTimerSys()->getDeltaTime();
	
    if(_endIndex - _startIndex > ELYSIAN_ADV_LOG_MAX_LINES_ONSCREEN) { //Too many lines onscreen 
        
        switch(_state) {
            case WAIT: //We're waiting a few frames before we get rid of the old stuff.
                _waitCounter += deltaTime;
                if(_waitCounter >= 0.5f/*ADV_LOG_FRAMES_PER_NEWLINE*/) { //Time to get rid of top line
					_state = FADE;
					//fadeCounter = 0;
                }
                break;
			case FADE:
				_fadeCounter += deltaTime*3.0f;
				if(_fadeCounter >= 1.0f) {
					_state = SCROLL;
					_fadeCounter = 1.0f;
					//yOffset = 0;
				}
				break;
            case SCROLL: //Move all the lines up to fill in where the previous line disappeared.
                _yOffset += deltaTime*75.0f;
                if(_yOffset >= 22.0f) { //We've moved the next line up
					_state = WAIT; //Get back to waiting.
					_waitCounter = 0;
					++_startIndex;
					_yOffset = 0;
					_fadeCounter = 0.0f;
                } 
                break;
			default:
				break;
        }
    }
	
	switch(_exitState) {
		case EXIT_ACTIVE:
			break;
		case EXIT_DISABLED:
			break;
		case EXIT_FADE_OUT:
			_baseAlpha -= _exitFadeTime*deltaTime;
			if(_baseAlpha <= 0.0f) {
				_baseAlpha = 0.0f;
				_exitState = EXIT_DISABLED;
			}
			break;
		case EXIT_FADE_IN:
			_baseAlpha += _exitFadeTime*deltaTime;
			if(_baseAlpha >= 1.0f) {
				_baseAlpha = 1.0f;
				_exitState = EXIT_ACTIVE;
			}
			break;
	}
}

/*
 Draws all onscreen entries into the Adventure Log (startIndex through endIndex).
 yOffset is used to move everything up and resize the box when the top line
 disappears. Depending on "type[index]" is which color the text is rendered as.
 */
void AdventureLog::render(Font &font) {
	if(_exitState == EXIT_DISABLED) return;
	
    gyVidAlphaLevel(GY_ALPHA_FULL);

	static QuadSprite quad;
	static GYColor4 topColor = { 0, 0, 255, 255 };
	static GYColor4 bottomColor = { 0, 155, 255, 255 };
	static GYColor4 canvasColor = { 0, 0, 0, 0 };
    
    //Draw Canvas for log strings
    quad._transform.setPos(ELYSIAN_ADV_LOG_X, ELYSIAN_ADV_LOG_Y+11+0.5f*(22*(_endIndex-_startIndex+1)-_yOffset-22), ELYSIAN_ADV_LOG_Z);
    quad._transform.setSize(430.0f, 22.0f*(_endIndex-_startIndex+1)-_yOffset-22.0f);
	for(unsigned i = 0; i < 4; ++i) quad.setVertexColor(i, canvasColor.r, canvasColor.g, canvasColor.b, canvasColor.a);
	quad.setVerticesAlpha(0.8f*_baseAlpha*255.0f, 0.8f*_baseAlpha*255.0f, 0.3f*_baseAlpha*255.0f, 0.3f*_baseAlpha*255.0f);
    quad.render();
	//renderer.drawTransparentQuad(ADV_LOG_X, ADV_LOG_Y+22, ADV_LOG_Z+1, 430, 22*(endIndex-startIndex+1)+yOffset-22, 0.8f, 0, 0, 0, 0.8f, 0, 0, 0, 0.3f, 0, 0, 0, 0.3f, 0, 0, 0);
    
	const float X_COORD		= 210;//ADV_LOG_X + 10;		//x coordinate to draw the string at (doesn't change between lines)
	const float Y_COORD1	= ELYSIAN_ADV_LOG_Y + 22 - _yOffset + 8;	//this is the *PART* of the y coord that doesn't change between lines
	const float Z_COORD		= ELYSIAN_ADV_LOG_Z + 1;		//same...z
	
	int yCoord2;	//this is the part of the y coord that changes depending on the line
	unsigned char r, g, b;	//colors to draw in (since this is the only thing that changes btwn states)
	/* NOTE could make r and g ints (or even bools!) and cast them
	 at the call at the cost of reability. this is because r and g
	 only ever get the values of 1 or 0. */
	
	//Draw all strings between the start and ending indices.
    for(int i = _startIndex; i <= _endIndex-1; ++i) {
		yCoord2 = 22 * (i-_startIndex);
        switch(_type[i]) {
            case TEXT_DEBUG:
				r = 255;
				g = 255;
				b = 255;
                break; 
			case TEXT_LUA_DEBUG:
				r = 150;
				g = 255;
				b = 255;
				break;
            case TEXT_NORMAL:
				r = g = b = 255;
                break; 
            case TEXT_ERROR:
				r = 255;
				g = b = 0;
                break; 
            case TEXT_NOTICE:
				r = 200;
				g = b = 255;
                break;
			default:
				break;
		}
		
        font._transform.setPos(X_COORD, Y_COORD1+ yCoord2, Z_COORD);
		font.setColor(r, g, b, 255);
		if(i == _startIndex) {
			float tempAlpha = 1.0f - _fadeCounter;	
			font.setAlpha(tempAlpha*_baseAlpha*255.0f);
		}
		else font.setAlpha(_baseAlpha*255.0f);
        font.drawString(Font::NO_FORMAT, Font::L_ALIGN, _log[i]);
    }
    
    quad._transform.setPos(ELYSIAN_ADV_LOG_X, ELYSIAN_ADV_LOG_Y, ELYSIAN_ADV_LOG_Z+2);
    quad._transform.setSize(ELYSIAN_ADV_LOG_W, ELYSIAN_ADV_LOG_H /3.0f );
	quad.setVertexColor(0, topColor.r, topColor.g, topColor.b, topColor.a); quad.setVertexColor(1, topColor.r, topColor.g, topColor.b, topColor.a);
	quad.setVertexColor(3, bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.a); quad.setVertexColor(2, bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.a);
	//quad.setColorVertical(0.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f);
	quad.setVerticesAlpha(_baseAlpha*255.0f, _baseAlpha*255.0f, _baseAlpha*255.0f, _baseAlpha*255.0f);
    quad.render();
	
    //renderer.drawTransparentQuad(ADV_LOG_X, ADV_LOG_Y, ADV_LOG_Z+2, 430, 22, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	//font.setPosition(ADV_LOG_X+15, ADV_LOG_Y);
	//font.setAlpha(1.0f, 1.0f,1.0f, 1.0f);
    font._transform.setPos(225, ELYSIAN_ADV_LOG_Y+7, ELYSIAN_ADV_LOG_Z+3);
	//font.setAlpha(_baseAlpha);
	font.setColor(255, 255, 255, 255);
	font.setAlpha(_baseAlpha*255.0);
	font.drawString(Font::NO_FORMAT, Font::L_ALIGN, "Adventure Log");
	//font.DrawString(0, ADV_LOG_X+15, ADV_LOG_Y, ADV_LOG_Z+3, 1, 1, 1, L_ALIGN, "Adventure Log");
}



}
