#include "gui/elysian_widget.hpp"
#include "gui/elysian_layout.hpp"
#include <gyro_matrix_api.h>

namespace elysian {

    Widget::~Widget(void) {
        delete _layout;
    }

    void Widget::setLayout(Layout *layout) {
        _layout = layout;
        _layout->setParent(this);
    }

    void Widget::setParentLayout(Layout* layout) {
        _parentLayout = layout;
    }

    void Widget::renderLayout(void) const {
        if(_layout) {
            gyMatPush();
            gyMatTranslate(0.0f, 0.0f, ELYSIAN_WIDGET_Z_OFFSET);
            _layout->render();
            gyMatPop();
        }
    }

    void Widget::maximize(AXIS axis) {
        if(getSizePolicy(axis) == EXPANDING || getSizePolicy(axis) == EXPANDING_MINIMIZING) {
            setSize(axis, getMaxSize(axis));
        }

    }

    void Widget::minimize(AXIS axis) {
        if(getSizePolicy(axis) == MINIMIZING || getSizePolicy(axis) == EXPANDING_MINIMIZING) {
            setSize(axis, getMinSize(axis));
        }
    }

    void Widget::expand(AXIS axis, int deltaSize) {
        if(getSizePolicy(axis) == EXPANDING || getSizePolicy(axis) == EXPANDING_MINIMIZING) {
            const int maxSize = getMaxSize(axis);
            int newSize = getSize(axis) + deltaSize;
            if(maxSize < newSize) newSize = maxSize;
            setSize(axis, newSize);
        }
    }

    void Widget::shrink(AXIS axis, int deltaSize) {
        if(getSizePolicy(axis) == MINIMIZING || getSizePolicy(axis) == EXPANDING_MINIMIZING) {
            const int minSize = getMinSize(axis);
            int newSize = getSize(axis) - deltaSize;
            if(minSize > newSize) newSize = minSize;
            setSize(axis, newSize);
        }
    }

    int Widget::getMaximizeDelta(AXIS axis) const {
        int maxDelta = 0;

        if(getSizePolicy(axis) == EXPANDING || getSizePolicy(axis) == EXPANDING_MINIMIZING) {
            maxDelta = getMaxSize(axis) - getSize(axis);
            if(maxDelta > 0) {
                if(_parentLayout) {
                    const int parentDelta = _parentLayout->getParent()->getMaximizeDelta(axis);
                    if(parentDelta < maxDelta) maxDelta = parentDelta;
                }
            }
        }

        return maxDelta;
    }

    int Widget::getMinimizeDelta(AXIS axis) const {
        int maxDelta = 0;

        if(getSizePolicy(axis) == MINIMIZING || getSizePolicy(axis) == EXPANDING_MINIMIZING) {
            maxDelta = getSize(axis) - getMinSize(axis);
            if(maxDelta > 0) {
                if(_layout) {
                    const auto& widgets = _layout->getWidgets();
                    for(auto&& widget : widgets) {
                        const int childDelta = widget->getMinimizeDelta(axis);
                        if(childDelta < maxDelta) maxDelta = childDelta;

                    }
                }
            }
        }

        return maxDelta;
    }
}
