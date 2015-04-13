#ifndef ELYSIAN_DIALOG_HPP
#define ELYSIAN_DIALOG_HPP

#include <gyro_matrix_api.h>
#include <vector>
#include <elysian_asset_typedefs.hpp>

#define ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE			500
#define ELYSIAN_DIALOG_PROMPT_OPTION_TEXT_BUFFER_SIZE	100
#define ELYSIAN_DIALOG_PROMPT_MAX_OPTION_COUNT			6

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

    class DialogScreen {
    public:
        enum BUBBLE_TYPE { BUBBLE_SPEECH, BUBBLE_THOUGHT };
        enum SCREEN_TYPE { SCREEN_TEXT, SCREEN_PROMPT };
    private:
        char _text[ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE];
        GYVector2		_pos;
        Entity*			_srcEntity      = nullptr; //array later
        //BUBBLE_TYPE		_bubbleType	= BUBBLE_SPEECH;
        SCREEN_TYPE		_screenType;
        int				_index          = -1;
        bool			_trigger        = false;

    protected:
                    DialogScreen(const SCREEN_TYPE scrType, const char*const text);

    public:

    //   void			setText(const char*const text);
       //void			setBubbleType(const BUBBLE_TYPE bubType);
       const char*  getText(void) const;
       void			addSourceEntity(Entity*const entity);        //supports multiple
       void			addSourcePos(const GYVector2& pos);          //for non-entity
       unsigned		getIndex(void) const;					            //location in overall dialog
       void			setIndex(const unsigned index);
       void			setTrigger(const bool val);					//Behavior::onDialog(unsigned screen, int option)
       SCREEN_TYPE	getScreenType(void) const;
       Entity*		getEntity(void) const;
       bool			isTrigger(void) const;
    };


    class DialogText: public DialogScreen {
    public:
       enum INPUT_TYPE { INPUT_MODAL, INPUT_ADVANCE, INPUT_NONE };
    private:
        //bool		_persist = false;
        //float		_waitTime;
        //INPUT_TYPE	_inputType = INPUT_ADVANCE;
        int			_nextIndex = -1;
    public:
                DialogText(const char*const text);
                //DialogText(const char*const text, Entity*const entity);
        //bool	setPersist(const bool val);               //stay onscreen even with next Index
        //void	setWaitTime(const float ms);            //only for INPUT_NONE type
        //void	setInputType(const INPUT_TYPE type);
        void	setNextIndex(const int index);
        int		getNextIndex(void) const;
    };

    class DialogPrompt: public DialogScreen {
    private:
        struct DialogOption {
            char text[ELYSIAN_DIALOG_PROMPT_OPTION_TEXT_BUFFER_SIZE];
            int nextIndex;
        } _options[ELYSIAN_DIALOG_PROMPT_MAX_OPTION_COUNT];

        unsigned _optionCount = 0;

    public:
                    DialogPrompt(const char*const text);
                    //DialogPrompt(const char*const  text, Entity*const entity);
        void		addOption(const char*const text, const int nextIndex);
        unsigned	getOptionCount(void) const;
        const char* getOptionText(const unsigned index) const;
        int			getOptionNextIndex(const unsigned index) const;

    };


    class Dialog {
    private:
        std::vector<DialogScreen*>	_screens;
        Entity*						_triggerEntity  = nullptr;
        int                         _startScreen    = 0;

    public:
                        ~Dialog(void);
        DialogText*		addTextScreen(const char*const text); //THESE NEED TO SET EACH SCREEN'S INDEX WHEN THEY'RE ADDED!!
        DialogPrompt*	addPromptScreen(const char*const text);
        unsigned		getScreenCount(void) const;
        DialogScreen*	getScreen(const unsigned index) const;
        void			setTriggerEntity(Entity*const entity);
        Entity*			getTriggerEntity(void) const;
        int             getStartScreen(void) const;
        void            setStartScreen(const int index);
    };


    //==== DIALOGSCREEN INLINEZ ====
    inline void DialogScreen::addSourceEntity(Entity*const entity) { _srcEntity = entity; }
    inline void DialogScreen::addSourcePos(const GYVector2& pos) { _pos = pos; }
    inline unsigned DialogScreen::getIndex(void) const { return _index; }
    inline void DialogScreen::setIndex(const unsigned index) { _index = index; }
    inline void DialogScreen::setTrigger(const bool val) { _trigger = val; }
    inline auto DialogScreen::getScreenType(void) const -> SCREEN_TYPE { return _screenType; }
    inline Entity* DialogScreen::getEntity(void) const { return _srcEntity; }
    inline const char* DialogScreen::getText(void) const { return _text; }
    inline bool DialogScreen::isTrigger(void) const { return _trigger; }


    //==== DIALOGTEXT INLINEZ ====
    inline void	DialogText::setNextIndex(const int index) { _nextIndex = index; }
    inline int	DialogText::getNextIndex(void) const { return _nextIndex; }

    //==== DIALOGPROMPT INLINEZ =====
    inline unsigned DialogPrompt::getOptionCount(void) const { return _optionCount; }
    inline const char* DialogPrompt::getOptionText(const unsigned index) const { return _options[index].text; }
    inline int DialogPrompt::getOptionNextIndex(const unsigned index) const { return _options[index].nextIndex; }

    //==== DIALOG INLINEZ =====
    inline int Dialog::getStartScreen(void) const { return _startScreen; }
    inline void Dialog::setStartScreen(const int index) { _startScreen = index; }
    inline unsigned Dialog::getScreenCount(void) const { return _screens.size(); }
    inline DialogScreen* Dialog::getScreen(const unsigned index) const { return _screens[index]; }
    inline void Dialog::setTriggerEntity(Entity * const entity) { _triggerEntity = entity; }
    inline Entity* Dialog::getTriggerEntity(void) const { return _triggerEntity; }





}

#endif
