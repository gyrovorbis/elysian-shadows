/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/gui/elysian_widget.hpp>
#include <ESGamma/gui/elysian_layout.hpp>
#include <libGyro/gyro_matrix_api.h>
#include <cfloat>

namespace elysian {

    Widget::~Widget(void) {
        delete _layout;
    }

    void Widget::setMouseTracking(bool enabled) {
        if(_mouseTracking && !enabled) {
            System::getEventSys()->removeEventObserver(this, EVENT_MOUSE_BUTTON);
            System::getEventSys()->removeEventObserver(this, EVENT_MOUSE_MOVED);
            System::getEventSys()->removeEventObserver(this, EVENT_MOUSE_SCROLLED);
        } else if(!_mouseTracking && enabled) {
            System::getEventSys()->addEventObserver(this, EVENT_MOUSE_BUTTON);
            System::getEventSys()->addEventObserver(this, EVENT_MOUSE_MOVED);
            System::getEventSys()->addEventObserver(this, EVENT_MOUSE_SCROLLED);
        }
        _mouseTracking = enabled;
    }

    void Widget::setLayout(TableLayout *layout) {
        _layout = layout;
        _layout->setParent(this);
    }

    void Widget::setParentLayout(TableLayout* layout) {
        if(_parentLayout && layout) {
            errprintf("Adding an entity to a layout that is already being used in another layout!");
        }
        _parentLayout = layout;
    }

    void Widget::render(void) const {
        if(_layout) {
           // if(_scissorRect) gyVidScissorRect(..)!!!
            _layout->render();
        }
    }

    void Widget::update(void) {
        if(_layout) {
            _layout->update();
        }
    }

    Widget* Widget::findChildWidget(const char *name) {
        if(_layout) return _layout->findWidget(name);
        else return nullptr;
    }
/*
    Widget* Widget::getChildWidget(unsigned index) {
        GY_ASSERT(_layout && index < _layout->getWidgetCount());
        return _layout->getWidget(index);
    }
*/
    inline void Widget::activate(void) {
        if(_layout) {
            _layout->activate();
        }
    }

    inline void Widget::deactivate(void) {
        if(_layout) {
            _layout->deactivate();
        }
    }

    void Widget::controllerEvent(const Controller& cont) {
        if(_layout) {
            _layout->controllerEvent(cont);
        }
    }
#if 0
    void Widget::keyboardEvent(const Keyboard& keybd) {
        if(_layout) {
            _layout->keyboardEvent(keybd);
        }
    }
#endif


    void Widget::touchEvent(const TouchDevice& touch) {
        if(_layout) {
            _layout->touchEvent(touch);
        }
    }

    void Widget::virtualDPadEvent(const VirtualDPad& dpad) {
        if(_layout) {
            _layout->virtualDPadEvent(dpad);
        }
    }

    void Widget::maximize(AXIS axis) {
        if(getSizePolicy(axis) == MAXIMUM) {
            setSize(axis, getSizeHint(axis));
        }

    }

    void Widget::minimize(AXIS axis) {
        if(getSizePolicy(axis) == MINIMUM || getSizePolicy(axis) == MINIMUM_EXPANDING) {
            setSize(axis, getSizeHint(axis));
        }
    }

    void Widget::expand(AXIS axis, float deltaSize) {
        uint16_t policy = getSizePolicy(axis);
        if(policy & (growFlag | expandFlag | ignoreFlag)) {
            const float maxSize = (policy == MAXIMUM)? getSizeHint(axis) : FLT_MAX;
            float newSize = getSize(axis) + deltaSize;
            if(maxSize < newSize) newSize = maxSize;
            setSize(axis, newSize);
        } //else qDebug() << "CANT EXPAND NONEXPANDING WIDGET!!!";
    }

    void Widget::shrink(AXIS axis, float deltaSize) {
        uint16_t policy = getSizePolicy(axis);
        if(policy & shrinkFlag) {
            const float minSize = (policy == MINIMUM || policy == MINIMUM_EXPANDING)? getSizeHint(axis) : 1.0f;
            float newSize = getSize(axis) - deltaSize;
            if(minSize > newSize) newSize = minSize;
            setSize(axis, newSize);
        } //else qDebug() << "CANT SHRINK NONSHRINKING WIDGET!";
    }

    float Widget::getMaximizeDelta(AXIS axis) const {
        float maxDelta = 0.0f;
        uint16_t policy = getSizePolicy(axis);

        if((policy & (growFlag | expandFlag)) || policy == MAXIMUM) {
            maxDelta = getMaxSize(axis) - getSize(axis);
            if(maxDelta > 0.0f) {
                if(_parentLayout) {
                    const float parentDelta = _parentLayout->getMaximizeDelta((TableLayout::AXIS)axis);
                    if(parentDelta < maxDelta) maxDelta = parentDelta;
                }
            }
        }

        return maxDelta;
    }

    float Widget::getMinimizeDelta(AXIS axis) const {
        float maxDelta = 0.0f;
#if 0
        if(getSizePolicy(axis) & (shrinkFlag)) {
            maxDelta = getSize(axis) - getMinSize(axis);    //maximum amount a parent can theoretically shrink
            //If parent can technically shrink in size
            if(maxDelta > 0.0f) {
                //If parent has children, have to take their sizes into account as well
                if(_layout) {
                    //get total size of largest row/col
                    float layoutDelta = 0.0f;

                    for(int j = 0; j < _layout->getDimensionSize(TableLayout::getSecondaryAxis(axis)); ++j) {
                        float currentDelta = 0.0f;
                        for(int i = 0; i < _layout->getDimensionSize(axis); ++i) {
                            Widget* w = _layout->getWidget(TableLayout::toRowIndex(axis, i, j), TableLayout::toColIndex(axis, i, j));
                            if(w) currentDelta += w->getSize(axis);
                        }
                        if(currentDelta > layoutDelta) layoutDelta = currentDelta;
                    }

#if 0
                    for(auto&& widget : _layout->getWidgets()) {
                        layoutDelta += widget->getSize(axis);
                    }

#endif
                    //Check if children don't take up all of parent's space
                    if(layoutDelta < getSize(axis)) {               //Parent can shrink to size of children
                        layoutDelta = getSize(axis)-layoutDelta;
                    } else layoutDelta = 0.0f;                      //Parent cannot shrink anymore, children won't allow it.

                    //Try to figure out the largest amount we can shrink a child to give parent room to shrink
                    float minChildDelta = 0.0f;
                    const auto& widgets = _layout->getWidgets();
                    for(auto&& widget : widgets) {
                        const float childDelta = widget->getMinimizeDelta(axis);
                        if(childDelta < minChildDelta) minChildDelta = childDelta; //Keep track of child that can shrink the most
                    }
                    //add the most shrinkable child's shrink delta to amount of room parent has to shrink
                    layoutDelta += minChildDelta;

                    //Make sure we don't shrink the parent beyond what it's minimum size will allow.
                    if(layoutDelta < maxDelta) maxDelta = layoutDelta;
                }
            }
        }
#endif
        return maxDelta;
    }

Vector2 Widget::getMaxSize(void) const {
    return Vector2(getMaxSize(X), getMaxSize(Y));
}

float Widget::getMaxSize(AXIS axis) const {
    uint16_t policy = getSizePolicy(axis);
    if(policy == MAXIMUM || policy == FIXED) return getSizeHint(axis);
    else return FLT_MAX;
}


void Widget::setMaxSize(Vector2 size) {
    setSizeHint(size);
    setSizePolicy(MAXIMUM, MAXIMUM);
}

Vector2 Widget::getMinSize(void) const {
    return Vector2(getMinSize(X), getMinSize(Y));
}

float Widget::getMinSize(AXIS axis) const {
    uint16_t policy = getSizePolicy(axis);
    if(policy == MINIMUM || policy == MINIMUM_EXPANDING || policy == FIXED) return getSizeHint(axis);
    else return 0.0f;
}
void Widget::setMinSize(Vector2 size) {
    setSizeHint(size);
    setSizePolicy(MINIMUM, MINIMUM);
}

Widget* Widget::getParentWidget(void) const {
    const TableLayout* layout = getParentLayout();
    if(layout) return layout->getParent();
    else return nullptr;
}

void Widget::onReceiveEvent(Event* event) {
    switch(event->getEventType()) {
    case EVENT_WIDGET_ACTIVE:

    default:
    return;

    }
}

void Widget::onObserveEvent(Event* event, EventTarget* target) {
#if 0
    if(!target) {

        auto* mouseEvent = static_cast<MouseEvent*>(event);
        switch(event->getEventType()) {

        case EVENT_MOUSE_BUTTON:
            onMouseButtonEvent(*mouseEvent->getMouse());
            break;
        case EVENT_MOUSE_MOVED:
            onMouseMoveEvent(*mouseEvent->getMouse());
            break;
        case EVENT_MOUSE_SCROLLED:
            onMouseWheelEvent(*mouseEvent->getMouse());
            break;
        }
    } else {
        KeyEventObserver::onObserveEvent(event, target);
    }
#endif
}

void Widget::onMouseButtonEvent(const Mouse &mouse) {
}

void Widget::onMouseMoveEvent(const Mouse &mouse) {
}

void Widget::onMouseWheelEvent(const Mouse &mouse) {
}

void Widget::setFocus(bool enabled) {
    _focus = enabled;
}

}
