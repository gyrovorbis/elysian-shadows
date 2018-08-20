#include <gui/elysian_menu_widget.hpp>
#include <crtp/elysian_debug.hpp>
#include <gui/elysian_layout.hpp>
#include <crtp/elysian_input_system.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_layout_proxy.hpp>
#include <proxy/elysian_widget_proxy.hpp>
#include <gui/elysian_item_delegate.hpp>

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
      //  GY_ASSERT(_currentIndex[axis] != index);
        if(!next) GY_ASSERT(index == InvalidIndex); //shit better be synchronized!

        _currentIndex[axis] = index;
        auto* prev = _currentWidget;
        _currentWidget = next;
        _onCurrentChange(prev, next);
    }
}

int MenuWidget::_toWrapIndex(int index) const {
    const int   count       = _layout->getWidgetCount();
    int         newIndex    = index;

    //Only check if we actually HAVE widgets and a valid index
    if(count) {
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
    } else newIndex = InvalidIndex;
    return newIndex;
}

Widget* MenuWidget::_getWidgetFromIndex(int index) {
    GY_ASSERT(_layout);
    const int count = _layout->getWidgetCount();
    if(index < 0 || index >= count) return nullptr;
    else return getLayout<SingleAxisLayout>()->getWidget(index);
}

int MenuWidget::_getIndexFromWidget(const Widget *widget) {
    GY_ASSERT(_layout);
    const unsigned widgetCount = _layout->getWidgetCount();

    for(unsigned i = 0; i < widgetCount; ++i) {
        if(getLayout<SingleAxisLayout>()->getWidget(i) == widget)
            return i;
    }
    return InvalidIndex;
}

void MenuWidget::activate(void) {
    GY_ASSERT(_layout);
    const auto groupingAxis = _layout->getGroupingAxis();
    if(_flags & ELYSIAN_MENU_WIDGET_FLAGS_RESET_CURRENT || _currentIndex[groupingAxis] == InvalidIndex) {
        if(_layout->getWidgetCount() > 1) {
            setCurrentIndex(0);
        }
    }
    GroupWidget::activate();
}


void MenuWidget::render(void) const {
    GroupWidget::render();
}


void MenuWidget::onCurrentChange(Widget*, Widget*) {

    //default implementation doesn't do shit!?
}
void MenuWidget::onSelectionChange(const MenuSelection* sel) {
    //default implementation doesn't do shit either!?!
    dbgprintf("SELECTION CHANGE - %d", sel->getEntry(0)->second.index);
}

void MenuWidget::onCancel(void) {
}

void MenuWidget::_onCurrentChange(Widget* prevSel, Widget* nextSel) {
    System::getEventSys()->sendEvent(MenuCurrentItemChangedEvent(prevSel, nextSel), this);
}

void MenuWidget::_onSelectionChange(MenuSelection*) {
    System::getEventSys()->sendEvent(Event(EVENT_WIDGET_MENU_SELECTION), this);
}

void MenuWidget::_onCancel(void) {
    System::getEventSys()->sendEvent(Event(EVENT_WIDGET_MENU_CANCEL), this);
}

void MenuWidget::onReceiveEvent(Event* event) {
    switch(event->getEventType()) {
    case EVENT_WIDGET_MENU_CANCEL:
        onCancel();
        return;
    case EVENT_WIDGET_MENU_CURRENT: {
        auto* menuEvent = static_cast<MenuCurrentItemChangedEvent*>(event);
        onCurrentChange(menuEvent->getPrevWidget(), menuEvent->getNextWidget());
        return;
    }
    case EVENT_WIDGET_MENU_SELECTION:
        onSelectionChange(getSelection());
        return;
    default:
        Widget::onReceiveEvent(event);
        return;
    }
}

void MenuWidget::controllerEvent(const Controller &cont) {
    GY_ASSERT(_layout);
    const unsigned count    = _layout->getWidgetCount();

    if(!count) { //no options
        setCurrentIndex(InvalidIndex);
    } else { //available options
        const auto axis = _layout->getGroupingAxis();

        //check for selecting option
        MenuSelection sel;
        MenuSelection::Entry entry(_currentWidget, MenuSelectionData{_currentIndex[axis]});
        sel.addEntry(entry);
        if(cont.getButtonState(GY_CONTROLLER_BUT_X) == GY_BUT_STATE_TAPPED) setSelection(sel);

        //check for going backwards
        if(cont.getButtonState(GY_CONTROLLER_BUT_CIRCLE) == GY_BUT_STATE_TAPPED) {
            _enabled = false;
            _onCancel();
        }
    }

    GroupWidget::controllerEvent(cont);

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
    GroupWidget::virtualDPadEvent(dpad);
}

void MenuWidget::setSelection(MenuSelection selection) {
    _selection = std::move(selection);
    _onSelectionChange(&_selection);
}

int MenuWidget::getCurrentIndex(void) const {
    const auto axis = _layout->getGroupingAxis();
    return _currentIndex[axis];
}


//=========== MENUITEMVIEW =============
void MenuItemView::update(void) {
    MenuWidget::update();

    _layout->clearWidgets();

    _bottomRightItem = _topLeftItem;
    for(int a = X; a < AXIS_COUNT; ++a) {
        float requiredSize = 0;

        if(getModel()->getAxisCount((AXIS)a)) do {
            requiredSize += getDelegate()->getDisplayWidget(_bottomRightItem)->getSize((AXIS)a);

            if(requiredSize > getSize((AXIS)a) + getMaximizeDelta((AXIS)a)) break;
            ++_bottomRightItem.indices[a];
        } while(_bottomRightItem.getAxisIndex((AXIS)a) < getModel()->getAxisCount((AXIS)a));

        if(_bottomRightItem.indices[a]) --_bottomRightItem.indices[a];
    }

    for(unsigned r = _topLeftItem.getRow(); r <= _bottomRightItem.getRow(); ++r) {
        for(unsigned c = _topLeftItem.getCol(); c <= _bottomRightItem.getCol(); ++c) {
            _layout->insertWidget(r - _topLeftItem.getRow(), c - _topLeftItem.getCol(), _delegate->getDisplayWidget(ItemModelIndex(r, c)));
        }
    }
}


void MenuItemView::onCurrentChange(Widget* prev, Widget* next) {
    if(_delegate) _delegate->onCurrentChanged(prev, next);
}

void MenuItemView::setCurrentIndex(int index) {
    if(index < 0) {
        if(_topLeftItem.getRow() > 0) _topLeftItem.indices[Y]--;
         MenuWidget::setCurrentIndex(0);
    } else if(index >= _layout->getWidgetCount()) {
        if(_bottomRightItem.getRow() < _model->getRowCount() - 1) _topLeftItem.indices[Y]++;
        MenuWidget::setCurrentIndex(_layout->getWidgetCount()-1);
    } else {
        MenuWidget::setCurrentIndex(index);
    }
}

MenuItemView::~MenuItemView(void) {
    delete _model;
    delete _delegate;
}

}
