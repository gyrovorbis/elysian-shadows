#include "gui/elysian_widget.hpp"
#include "gui/elysian_layout.hpp"
#include <gyro_matrix_api.h>
#include <cfloat>

namespace elysian {

    Widget::~Widget(void) {
        delete _layout;
    }

    void Widget::setLayout(Layout *layout) {
        _layout = layout;
        _layout->setParent(this);
    }

    void Widget::setParentLayout(Layout* layout) {
        if(_parentLayout && layout) {
            errprintf("Adding an entity to a layout that is already being used in another layout!");
        }
        _parentLayout = layout;
    }

    void Widget::render(void) const {
        if(_layout) {
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

    Widget* Widget::getChildWidget(unsigned index) {
        GY_ASSERT(_layout && index < _layout->getWidgetCount());
        return _layout->getWidget(index);
    }

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

    void Widget::keyboardEvent(const Keyboard& keybd) {
        if(_layout) {
            _layout->keyboardEvent(keybd);
        }
    }

    void Widget::mouseEvent(const Mouse& mouse) {
        if(_layout) {
            _layout->mouseEvent(mouse);
        }
    }

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

        if(policy & (growFlag | expandFlag)) {
            maxDelta = getMaxSize(axis) - getSize(axis);
            if(maxDelta > 0.0f) {
                if(_parentLayout) {
                    const float parentDelta = _parentLayout->getParent()->getMaximizeDelta(axis);
                    if(parentDelta < maxDelta) maxDelta = parentDelta;
                }
            }
        }

        return maxDelta;
    }

    float Widget::getMinimizeDelta(AXIS axis) const {
        float maxDelta = 0.0f;

        if(getSizePolicy(axis) & (shrinkFlag)) {
            maxDelta = getSize(axis) - getMinSize(axis);
            if(maxDelta > 0.0f) {
                if(_layout) {
                    float layoutDelta = 0.0f;
                    for(auto&& widget : _layout->getWidgets()) {
                        layoutDelta += widget->getSize(axis);
                    }

                    if(layoutDelta < getSize(axis)) {
                        layoutDelta = getSize(axis)-layoutDelta;
                    } else layoutDelta = 0.0f;

                    float minChildDelta = 0.0f;
                    const auto& widgets = _layout->getWidgets();
                    for(auto&& widget : widgets) {
                        const float childDelta = widget->getMinimizeDelta(axis);
                        if(childDelta < minChildDelta) minChildDelta = childDelta;
                    }
                    layoutDelta += minChildDelta;
                    if(layoutDelta < maxDelta) maxDelta = layoutDelta;
                }
            }
        }

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
    const Layout* layout = getParentLayout();
    if(layout) return layout->getParent();
    else return nullptr;
}

}
