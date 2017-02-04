#include <cstdlib>
#include "gui/elysian_layout.hpp"
#include "gui/elysian_widget.hpp"

namespace elysian {

    Layout::Layout(AXIS groupingAxis, std::vector<Widget*>&& widgets, Widget* parent):
        _widgets(std::move(widgets)),
        _parent(parent),
        _groupingAxis(groupingAxis)
    {
        for(auto&& widget : _widgets) widget->setParentLayout(this);
    }

    Layout::Layout(AXIS groupingAxis, std::vector<Widget*>& widgets, Widget* parent):
        _widgets(widgets),
        _parent(parent),
        _groupingAxis(groupingAxis)
    {
        for(auto&& widget : _widgets) widget->setParentLayout(this);
    }

    Layout::~Layout(void) {
        for(auto&& w: _widgets)
            delete w;
    }

    void Layout::addWidget(Widget* widget) {
        _widgets.push_back(widget);
        widget->setParentLayout(this);
    }

    void Layout::removeWidget(Widget* widget) {
        for(auto it = _widgets.begin(); it != _widgets.end(); ++it) {
            if(*it == widget) {
                _widgets.erase(it);
                widget->setParentLayout(nullptr);
                break;
            }
        }
    }

    void Layout::render(void) const {
        for(auto&& widget: _widgets) widget->render();
    }

    //===== PRIVATE API =======

    void Layout::_updateSizes(AXIS axis_) {
        const Widget::AXIS axis = (Widget::AXIS)axis_;
        const int parentSize    = _parent->getSize(axis);

        if(axis_ == _groupingAxis) {
            int overflow = 0;

            for(auto&& widget : _widgets) {
                const int   size = widget->getSize(axis);
                int         delta = parentSize - size;

                if(delta > 0) { //Expand widget to match parent size (if expandable)
                    int maxDelta = widget->getMaximizeDelta(axis);
                    if(maxDelta < delta) delta = maxDelta;
                    widget->expand(axis, delta);

                } else if(delta < 0) {
                    //Attempt to shrink widgets to match parent size (if shrinkable)
                    delta = std::abs(delta);
                    int minDelta = widget->getMinimizeDelta(axis);
                    if(minDelta < delta) delta = minDelta;
                    widget->shrink(axis, delta);
                    const int newSize = size - delta;
                    //Record any leftover size that could not be shrunk
                    if(newSize > parentSize) {
                        if(newSize - parentSize > overflow)
                            overflow = newSize - parentSize;
                    }
                }
            }

            //Resize parent to accomodate child widgets (which cannot be shrunk any further)
            if(overflow) {
                const int maxDelta = _parent->getMaximizeDelta(axis);
                if(maxDelta < overflow) overflow = maxDelta; //We're basically fucked. Can't fit widget. Don't try.
                _parent->expand(axis, overflow);
            }

        } else {
            int curSize = 0;

            for(auto&& widget: _widgets) curSize += widget->getSize(axis);

            //Check if layout size is smaller than parent
            if(curSize < parentSize) {
                int delta = parentSize - curSize;

                //Try to evenly distribute the difference to expandable widgets
                while(delta) {
                    bool didExpand = false;

                    for(auto&& widget: _widgets) {
                        if(widget->getSizePolicy(axis) == Widget::EXPANDING ||
                           widget->getSizePolicy(axis) == Widget::EXPANDING_MINIMIZING) {
                            const int widgetSize = widget->getSize(axis);

                            if(widgetSize < widget->getMaxSize(axis)) {
                                widget->setSize(axis, widgetSize+1);
                                didExpand = true;
                                if(--delta == 0) break;
                            }
                        }
                    }
                    if(!didExpand) break;
                }

            //Check if layout size is larger than parent
            } else if(curSize > parentSize) {
                //Make widgets smaller first?

                //Make parent bigger
                int delta = curSize - parentSize;
                const int parentMaxDelta = _parent->getMaximizeDelta(axis);
                if(parentMaxDelta < delta) delta = parentMaxDelta;
                _parent->expand(axis, delta);

                //Request for the parent widget to expand to accomodate the child widgets


            }
        }

    }

    void Layout::_updatePositions(AXIS axis_) {
        const Widget::AXIS axis = (Widget::AXIS)axis_;
        const int parentPos     = _parent->getPosition(axis);

        if(_groupingAxis != axis_) {
            for(auto&& widget : _widgets) {
                widget->setPosition(axis, parentPos);
            }
        } else {

            int startPos = parentPos - 0.5f*_parent->getSize(axis);
            int endPos;

            for(auto&& widget : _widgets) {
                endPos = startPos + widget->getSize(axis);
                widget->setPosition(axis, (endPos-startPos)*0.5f);
                startPos = endPos;
            }

            //Positional over/underflow
            //int delta = (parentPos + 0.5f*_parent->getSize(axis)) - endPos

        }

    }


}
