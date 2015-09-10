#include <crtp/elysian_dialog.hpp>
#include <cstring>


namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogTextBase)
    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogOptionBase)
    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogPromptBase)
    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogBase)

// ====== DIALOG SCREEN ======



// ====== DIALOG TEXT =======


// ====== DIALOG PROMPT =======
void DialogPromptBase::addOption(const char*const text, const int nextIndex) { _options.push_back(new DialogOption(text, nextIndex)); }
void DialogPromptBase::setOption(const unsigned index, DialogOption* option) {
    if(index >= _options.size()) {
        _options.push_back(option);
    } else {
        if(_options[index]) {
            delete _options[index];
            _options[index] = option;
        }
    }
}
inline DialogPromptBase::~DialogPromptBase(void) { for(auto it: _options) delete it; }
// ====== DIALOG =======



DialogText*	DialogBase::addTextScreen(const char*const text) {
    DialogText* screen = new DialogText(text);
    screen->setIndex(_screens.size());
    _screens.push_back(screen);
    screen->setNextIndex(_screens.size());
    return screen;
}

DialogPrompt* DialogBase::addPromptScreen(const char*const text) {
    DialogPrompt* screen = new DialogPrompt(text);
    screen->setIndex(_screens.size());
    _screens.push_back(screen);
    return screen;
}


}


