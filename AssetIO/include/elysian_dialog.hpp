#ifndef ELYSIAN_DIALOG_HPP
#define ELYSIAN_DIALOG_HPP

#include <gyro_matrix_api.h>
#include <vector>

#define ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE			500
#define ELYSIAN_DIALOG_PROMPT_OPTION_TEXT_BUFFER_SIZE	100
#define ELYSIAN_DIALOG_PROMPT_MAX_OPTION_COUNT			6

namespace elysian {

	class Entity;

	class DialogScreen {
	public:
		enum BUBBLE_TYPE { BUBBLE_SPEECH, BUBBLE_THOUGHT };
		enum SCREEN_TYPE { SCREEN_TEXT, SCREEN_PROMPT };
	private:
		char _text[ELYSIAN_DIALOG_SCREEN_TEXT_BUFFER_SIZE];
		GYVector2		_pos;
		Entity*			_srcEntity	= nullptr; //array later
		BUBBLE_TYPE		_bubbleType	= BUBBLE_SPEECH;
		SCREEN_TYPE		_screenType;
		int				_index		= -1;
		bool			_trigger	= false;

	public:
					DialogScreen(const SCREEN_TYPE scrType);
	   void			setText(const char*const text);
	   void			setBubbleType(const BUBBLE_TYPE bubType);
	   void			addSourceEntity(Entity*const entity);        //supports multiple
	   void			addSourcePos(const GYVector2& pos);          //for non-entity
	   unsigned		getIndex(void) const;					            //location in overall dialog
	   void			setTrigger(const bool val);					//Behavior::onDialog(unsigned screen, int option)
	   SCREEN_TYPE	getScreenType(void) const;
	};


	class DialogText: public DialogScreen {
	public:
	   enum INPUT_TYPE { INPUT_MODAL, INPUT_ADVANCE, INPUT_NONE };
	private:
		bool		_persist = false;
		float		_waitTime;
		INPUT_TYPE	_inputType = INPUT_ADVANCE;
		int			_nextIndex = -1;
	public:
				DialogText(void);
				DialogText(const char*const text);
				DialogText(const char*const text, Entity*const entity);
		bool	setPersist(const bool val);               //stay onscreen even with next Index
		void	setWaitTime(const float ms);            //only for INPUT_NONE type
		void	setInputType(const INPUT_TYPE type);
		void	setNextIndex(const unsigned index);
	};

	class DialogPrompt: public DialogScreen {
	private:
		struct DialogOption {
			char text[ELYSIAN_DIALOG_PROMPT_OPTION_TEXT_BUFFER_SIZE];
			unsigned nextIndex;
		} _options[ELYSIAN_DIALOG_PROMPT_MAX_OPTION_COUNT];

		unsigned _optionCount = 0;

	public:
				DialogPrompt(void);
				DialogPrompt(const char*const text);
				DialogPrompt(const char*const  text, Entity*const entity);
		void	addOption(const char*const text, const unsigned nextIndex);

	};


	class Dialog {
	private:
		std::vector<DialogScreen*> _screens;

	public:
						~Dialog(void);
		DialogText*		addTextScreen(const char*const text); //THESE NEED TO SET EACH SCREEN'S INDEX WHEN THEY'RE ADDED!!
		DialogPrompt*	addPromptScreen(const char*const text);
		unsigned		getScreenCount(void) const;

	};





}

#endif
