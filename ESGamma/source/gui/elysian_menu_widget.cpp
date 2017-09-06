#include <gui/elysian_menu_widget.hpp>
#include <crtp/elysian_debug.hpp>
#include <gui/elysian_layout.hpp>
#include <crtp/elysian_input_system.hpp>

namespace elysian {

//static members
const constexpr int MenuWidget::InvalidIndex;


//instance members
MenuWidget::~MenuWidget(void) {
    delete _cursor;
}

void MenuWidget::_setCurrent(Widget *next, int index) {
    GY_ASSERT(_layout);
    const auto axis = _layout->getGroupingAxis();
    if(next != _currentWidget) {
        GY_ASSERT(_currentIndex[axis] != index);
        if(!next) GY_ASSERT(index == InvalidIndex); //shit better be synchronized!

        _currentIndex[axis] = index;
        auto* prev = _currentWidget;
        _currentWidget = next;
        _onCurrentChange(prev, next);
    }
}

int MenuWidget::_toWrapIndex(int index) const {
    const int   count       = _layout->getWidgetCount();
    int         newIndex    = InvalidIndex;

    //Only check if we actually HAVE widgets and a valid index
    if(count && index != InvalidIndex) {
        int topIndex = count-1;

        //Apply upper boundary
        if(index > topIndex) {
            newIndex = (_flags & ELYSIAN_MENU_WIDGET_FLAGS_SCROLL_WRAP)?
                        index % count : topIndex;
        //Apply lower boundary
        } else if(index < 0) {
            newIndex = (_flags & ELYSIAN_MENU_WIDGET_FLAGS_SCROLL_WRAP)?
                        count - (abs(index) % count) : 0;
        }
    }
    return newIndex;
}

Widget* MenuWidget::_getWidgetFromIndex(int index) {
    GY_ASSERT(_layout);
    const int count = _layout->getWidgetCount();
    if(index < 0 || index >= count) return nullptr;
    else return _layout->getWidget(index);
}

int MenuWidget::_getIndexFromWidget(const Widget *widget) {
    GY_ASSERT(_layout);
    const unsigned widgetCount = _layout->getWidgetCount();

    for(unsigned i = 0; i < widgetCount; ++i) {
        if(_layout->getWidget(i) == widget)
            return i;
    }
    return InvalidIndex;
}

void MenuWidget::activate(void) {
    if(_flags & ELYSIAN_MENU_WIDGET_FLAGS_RESET_CURRENT) {
        if(_layout->getWidgetCount() > 1) {
            setCurrentIndex(0);
        }
    }
}


void MenuWidget::render(void) const {
    GroupWidget::render();
}


void MenuWidget::onCurrentChange(Widget*, Widget*) {
    //default implementation doesn't do shit!?
}
void MenuWidget::onSelectionChange(MenuSelection*) {
    //default implementation doesn't do shit either!?!
}

void MenuWidget::onCancel(void) {
}

#if 0
void MenuWidget::onFocusChange(bool) {
    //default implementation doesn't do fuck all in this bitch either...
}
#endif

void MenuWidget::controllerEvent(const Controller &cont) {
    GY_ASSERT(_layout);
    const unsigned count    = _layout->getWidgetCount();

    if(!count) { //no options
        setCurrentIndex(InvalidIndex);
    } else { //available options

        //check for selecting option
        if(cont.getButtonState(GY_CONTROLLER_BUT_X));

        //check for going backwards
        if(cont.getButtonState(GY_CONTROLLER_BUT_CIRCLE)) _enabled = false;
    }

}

void MenuWidget::virtualDPadEvent(const VirtualDPad &dpad) {
    GY_ASSERT(_layout);
    const unsigned count    = _layout->getWidgetCount();

    if(!count) { //no options
        setCurrentIndex(InvalidIndex);
    } else { //available options

        //update option index
        if(count == 1) { //single option
            setCurrentIndex(0);
        } else {
            const auto groupingAxis = _layout->getGroupingAxis();
            switch(groupingAxis) { //multiple options
            case Widget::Y: //Vertical Layout
                if(dpad.isTapped(GY_CONTROLLER_BUT_DPAD_UP))    setCurrentIndex(_currentIndex[groupingAxis]-1);
                if(dpad.isTapped(GY_CONTROLLER_BUT_DPAD_DOWN))  setCurrentIndex(_currentIndex[groupingAxis]+1);
                break;
            case Widget::X: //Horizontal Layout
                if(dpad.isTapped(GY_CONTROLLER_BUT_DPAD_LEFT))  setCurrentIndex(_currentIndex[groupingAxis]-1);
                if(dpad.isTapped(GY_CONTROLLER_BUT_DPAD_RIGHT)) setCurrentIndex(_currentIndex[groupingAxis]+1);
                break;
            default:
                GY_ASSERT(false); //Uh, unknown layout type!
            }
        }
    }
}


}
