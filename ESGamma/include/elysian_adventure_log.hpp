#ifndef ELYSIAN_ADVENTURE_LOG_HPP
#define ELYSIAN_ADVENTURE_LOG_HPP

#include <cstring>
#include "elysian_system.hpp"

namespace elysian {

class Font;

#define ELYSIAN_ADV_LOG_LINES 100
#define ELYSIAN_ADV_LOG_CHARS_PER_LINE 1000
#define ELYSIAN_ADV_LOG_MAX_LINES_ONSCREEN 2
#define ELYSIAN_ADV_LOG_FRAMES_PER_NEWLINE 15

#define ELYSIAN_ADV_LOG_X 409.0f4
#define ELYSIAN_ADV_LOG_Y 27.0f
#define ELYSIAN_ADV_LOG_Z 600.0f
#define ELYSIAN_ADV_LOG_W 430.0f
#define ELYSIAN_ADV_LOG_H 22.0f

#define ELYSIAN_ADV_LOG_LABEL_LENGTH    20
#define ELYSIAN_ADV_LOG_LABEL_DEFAULT   "ADVENTURE LOG"
	/*
	 Master class for the Adventure Log. Call AddLine to add your line
	 to the logfile. Update and Draw are expecting to be called once
	 per frame. This is meant to be managed by GuiSystem.
	 */
	class AdventureLog: public System {
	public:
		//Types of strings that may be pushed into the log
		enum TEXT_TYPE  { TEXT_DEBUG, TEXT_LUA_DEBUG, TEXT_NORMAL, TEXT_NOTICE, TEXT_ERROR, TEXT_MAX };
		//States that the log can be in
		enum LOG_STATE { WAIT, FADE, SCROLL };
		//Exit states
		enum LOG_EXIT { EXIT_ACTIVE, EXIT_FADE_OUT, EXIT_DISABLED, EXIT_FADE_IN };
		
		void update(void);
        void render(void);
		

		void addLine(const TEXT_TYPE lineType, const char* line);
        void setEnabled(const bool val);
        bool isEnabled(void) const;

        void setLabel(const char*label);
        void setLabelFont(Font*const font);
        void setTextFont(Font*const font);

	private:
		static const float _exitFadeTime;
		
        char        _log[ELYSIAN_ADV_LOG_LINES][ELYSIAN_ADV_LOG_CHARS_PER_LINE];
        TEXT_TYPE   _type[ELYSIAN_ADV_LOG_LINES];
        char        _label[ELYSIAN_ADV_LOG_LABEL_LENGTH]
                                    = ELYSIAN_ADV_LOG_LABEL_DEFAULT;

        LOG_STATE   _state			= WAIT;
        LOG_EXIT    _exitState		= EXIT_ACTIVE;
        float       _baseAlpha		= 1.0f;
        int         _startIndex		= 0;
        int         _endIndex		= 0;
        float       _yOffset		= 0;
        float       _waitCounter	= 0.0f;
        float       _fadeCounter	= 0.0f;
        Font*       _labelFont      = nullptr;
        Font*       _textFont       = nullptr;
        unsigned    _scrW;
        unsigned    _scrH;
		//float _waitCounter;
		//unsigned int _maxLinesOnscreen;
		
	}; 






    // ===== INLINEZ ======
    inline void AdventureLog::setLabelFont(Font * const font) { _labelFont = font; }
    inline void AdventureLog::setTextFont(Font * const font) { _textFont = font; }
    inline void AdventureLog::setLabel(const char *label) { strcpy(_label, label); }


	inline bool AdventureLog::isEnabled(void) const {
		return (_exitState == EXIT_ACTIVE || _exitState == EXIT_FADE_IN)? true : false;
	}

    inline void AdventureLog::setEnabled(const bool val) {
        _exitState = val? EXIT_FADE_IN : EXIT_FADE_OUT;
    }
    
}

#endif
