#ifndef ELYSIAN_ADVENTURE_LOG_HPP
#define ELYSIAN_ADVENTURE_LOG_HPP

#include "elysian_system.hpp"

namespace elysian {

class Font;

#define ELYSIAN_ADV_LOG_LINES 100
#define ELYSIAN_ADV_LOG_CHARS_PER_LINE 100
#define ELYSIAN_ADV_LOG_MAX_LINES_ONSCREEN 3
#define ELYSIAN_ADV_LOG_FRAMES_PER_NEWLINE 15
#define ELYSIAN_ADV_LOG_X 409.0f
#define ELYSIAN_ADV_LOG_Y 27.0f
#define ELYSIAN_ADV_LOG_Z 700.0f
#define ELYSIAN_ADV_LOG_W 430.0f
#define ELYSIAN_ADV_LOG_H 22.0f
	
	/*
	 Master class for the Adventure Log. Call AddLine to add your line
	 to the logfile. Update and Draw are expecting to be called once
	 per frame. This is meant to be managed by GuiSystem.
	 */
	class AdventureLog: public System {
	public:
		//Types of strings that may be pushed into the log
		enum TEXT_TYPE  { TEXT_DEBUG, TEXT_LUA_DEBUG, TEXT_NORMAL, TEXT_NOTICE, TEXT_ERROR };
		//States that the log can be in
		enum LOG_STATE { WAIT, FADE, SCROLL };
		//Exit states
		enum LOG_EXIT { EXIT_ACTIVE, EXIT_FADE_OUT, EXIT_DISABLED, EXIT_FADE_IN };

		AdventureLog(void);
		
		void update(void);
		void render(Font &font);
		

		void addLine(const TEXT_TYPE lineType, const char* line);
		void disable(void);
		void enable(void);
        bool isEnabled(void) const;

	private:
		static const float _exitFadeTime;
		
		char _log[ELYSIAN_ADV_LOG_LINES][ELYSIAN_ADV_LOG_CHARS_PER_LINE];
		TEXT_TYPE _type[ELYSIAN_ADV_LOG_LINES];
		LOG_STATE _state;
		LOG_EXIT _exitState;
		
		float _baseAlpha;
		int _startIndex, _endIndex;
		float _yOffset;
		float _waitCounter, _fadeCounter; 
		//float _waitCounter;
		//unsigned int _maxLinesOnscreen;
		
	}; 

	inline bool AdventureLog::isEnabled(void) const {
		return (_exitState == EXIT_ACTIVE || _exitState == EXIT_FADE_IN)? true : false;
	}
    
}

#endif
