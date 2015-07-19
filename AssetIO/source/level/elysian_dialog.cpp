#include <level/elysian_dialog.hpp>
#include <cstring>


namespace elysian {

// ====== DIALOG SCREEN ======
DialogScreen::DialogScreen(const SCREEN_TYPE scrType, const char*const text):
    _screenType(scrType)
{
    strcpy(_text, text);
}


// ====== DIALOG TEXT =======
DialogText::DialogText(const char*const text):
    DialogScreen(DialogScreen::SCREEN_TEXT, text)
{   }


// ====== DIALOG PROMPT =======
DialogPrompt::DialogPrompt(const char*const text):
    DialogScreen(DialogScreen::SCREEN_PROMPT, text)
{	}


void DialogPrompt::addOption(const char*const text, const int nextIndex) {
    assert(_optionCount != ELYSIAN_DIALOG_PROMPT_MAX_OPTION_COUNT);
    strcpy(_options[_optionCount].text, text);
    _options[_optionCount].nextIndex = nextIndex;
    ++_optionCount;
}


// ====== DIALOG =======

Dialog::~Dialog(void) {
    //Clean up our shit!
    for(auto it : _screens) {
        delete it;
    }
}

DialogText*	Dialog::addTextScreen(const char*const text) {
    DialogText* screen = new DialogText(text);
    screen->setIndex(_screens.size());
    _screens.push_back(screen);
    screen->setNextIndex(_screens.size());
    return screen;
}

DialogPrompt* Dialog::addPromptScreen(const char*const text) {
    DialogPrompt* screen = new DialogPrompt(text);
    screen->setIndex(_screens.size());
    _screens.push_back(screen);
    return screen;
}


}


