#ifndef ELYSIAN_ADVENTURE_LOG_HPP
#define ELYSIAN_ADVENTURE_LOG_HPP

#include <cstring>
#include <ESGamma/system/elysian_system.hpp>

namespace elysian {

class Font;

#define ELYSIAN_ADV_LOG_LINES                   100
#define ELYSIAN_ADV_LOG_CHARS_PER_LINE          1000
#define ELYSIAN_ADV_LOG_MAX_LINES_ONSCREEN      0
#define ELYSIAN_ADV_LOG_MAX_LINES_DISPLAYABLE   2
#define ELYSIAN_ADV_LOG_FRAMES_PER_NEWLINE      15
#define ELYISAN_ADV_LOG_PAUSE_TIME              10.0f
#define ELYSIAN_ADV_LOG_DEFAULT_FADE_TIME       1.0f

#define ELYSIAN_ADV_LOG_X                       409.0f4
#define ELYSIAN_ADV_LOG_Y                       27.0f
#define ELYSIAN_ADV_LOG_Z                       600.0f
#define ELYSIAN_ADV_LOG_W                       430.0f
#define ELYSIAN_ADV_LOG_H                       22.0f

#define ELYSIAN_ADV_LOG_LABEL_LENGTH    20
#define ELYSIAN_ADV_LOG_LABEL_DEFAULT   "COMBAT LOG"
	/*
	 Master class for the Adventure Log. Call AddLine to add your line
	 to the logfile. Update and Draw are expecting to be called once
	 per frame. This is meant to be managed by GuiSystem.
	 */
	class AdventureLog: public System {
	public:
		//Types of strings that may be pushed into the log
		enum TEXT_TYPE  { TEXT_DEBUG, TEXT_LUA_DEBUG, TEXT_NORMAL, TEXT_NOTICE, TEXT_LOG_ERROR, TEXT_MAX };
		//States that the log can be in
		enum LOG_STATE { WAIT, FADE, SCROLL };
		//Exit states
		enum LOG_EXIT { EXIT_ACTIVE, EXIT_FADE_OUT, EXIT_DISABLED, EXIT_FADE_IN };
		
		void update(void);
        void render(void);

        void printf(const char* str, ...);
		void addLine(const TEXT_TYPE lineType, const char* line);
        void addLine(const char* line);
        void clear(void);

        bool isEnabled(void) const;
        void fade(const float time, const bool enabled=false);

        void setLabel(const char*label);
        void setLabelFont(Font*const font);
        void setTextFont(Font*const font);

        void setStaticText(const bool val=true);
        bool hasStaticText(void) const;

	private:
        //static const float _exitFadeTime;
		
        char        _log[ELYSIAN_ADV_LOG_LINES][ELYSIAN_ADV_LOG_CHARS_PER_LINE];
        TEXT_TYPE   _type[ELYSIAN_ADV_LOG_LINES];
        char        _label[ELYSIAN_ADV_LOG_LABEL_LENGTH] = ELYSIAN_ADV_LOG_LABEL_DEFAULT;

        LOG_STATE   _state			= WAIT;
        LOG_EXIT    _exitState		= EXIT_DISABLED;
        float       _baseAlpha		= 1.0f;
        int         _startIndex		= 0;
        int         _endIndex		= 0;
        float       _yOffset		= 0;
        float       _waitCounter	= 0.0f;
        float       _fadeCounter	= 0.0f;
        float       _exitFadeTime   = 0.0f;
        Font*       _labelFont      = nullptr;
        Font*       _textFont       = nullptr;
        unsigned    _scrW           = 0;
        unsigned    _scrH           = 0;
        bool        _staticText     = false;
		//float _waitCounter;
		//unsigned int _maxLinesOnscreen;
		
	}; 





    // ===== INLINEZ ======
    inline void AdventureLog::setLabelFont(Font * const font) { _labelFont = font; }
    inline void AdventureLog::setTextFont(Font * const font) { _textFont = font; }
    inline void AdventureLog::setLabel(const char *label) { strcpy(_label, label); }

    inline void AdventureLog::clear(void) { _startIndex = _endIndex; }

	inline bool AdventureLog::isEnabled(void) const {
		return (_exitState == EXIT_ACTIVE || _exitState == EXIT_FADE_IN)? true : false;
	}

    inline void AdventureLog::fade(const float time, const bool enabled) {
        _exitState = enabled? EXIT_FADE_IN : EXIT_FADE_OUT;
        _exitFadeTime = time;
    }

    inline bool AdventureLog::hasStaticText(void) const { return _staticText; }

    inline void AdventureLog::setStaticText(const bool val) { _staticText = val; }

    inline void AdventureLog::addLine(const char* string) { addLine(TEXT_NORMAL, string); }
    
}

#endif
