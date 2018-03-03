#include <cstdlib>
#include <gui/elysian_widget.hpp>
#include <gui/elysian_layout.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_layout_proxy.hpp>
#include <proxy/elysian_widget_proxy.hpp>

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
        for(auto& w: _widgets)
            delete w;
    }

    Widget* Layout::findWidget(const char *name) const {
        Widget* child = nullptr;
        for(auto& w: _widgets) {
            if((child = w->findChildWidget(name))) {
                return child;
            }
        }
        return nullptr;
    }

    void Layout::update(void) {
        if(!getWidgetCount()) return;
        for(auto& widget: _widgets) widget->update();

        _updateSizes();
        _updatePositions();
        _updatePositionsZ();
    }

    void Layout::_updatePositionsZ(void) {
        GY_ASSERT(_parent);
        float zCoord = _parent->getPositionZ() + ELYSIAN_GUI_LAYOUT_Z_COORD_OFFSET;
        for(auto&& widget : _widgets) {
            widget->setPositionZ(zCoord);
        }
    }

    void Layout::addWidget(Widget* widget) {
        for(auto&& it : _widgets) {
            if(it == widget) return;
        }
        _widgets.push_back(widget);
        widget->setParentLayout(this);
    }

    Widget* Layout::removeWidget(Widget* widget) {
        for(auto it = _widgets.begin(); it != _widgets.end(); ++it) {
            if(*it == widget) {
                _widgets.erase(it);
                widget->setParentLayout(nullptr);
                break;
            }
        }
        return widget;
    }

    void Layout::render(void) const {
        for(auto& widget: _widgets) widget->render();
    }

    void Layout::activate(void) const {
        for(auto& widget: _widgets) widget->activate();
    }

    void Layout::deactivate(void) const {
        for(auto& widget: _widgets) widget->deactivate();
    }

    void Layout::controllerEvent(const Controller &cont) {
        for(auto& widget: _widgets) if(widget->hasFocus()) widget->controllerEvent(cont);
    }

    void Layout::touchEvent(const TouchDevice &touch) {
        for(auto& widget: _widgets) widget->touchEvent(touch);
    }

    void Layout::virtualDPadEvent(const VirtualDPad &vdpad) {
        for(auto& widget: _widgets) if(widget->hasFocus()) widget->virtualDPadEvent(vdpad);
    }

    void Layout::clearWidgets(void) {
        for(auto& widget: _widgets) widget->setParentLayout(nullptr);
        _widgets.clear();
    }

    //===== PRIVATE API =======

    void Layout::_updateSizes(AXIS axis_) {
        GY_ASSERT(_parent);
        const Widget::AXIS axis = (Widget::AXIS)axis_;
        const float parentSize    = _parent->getSize(axis);

        if(axis_ != _groupingAxis) {
            float overflow = 0.0f;
            float underflow = 0.0f;

            for(auto&& widget : _widgets) {
                float         size = widget->getSize(axis);
                float         delta = parentSize - size;

                //Check whether the difference is even big enough to give a shit or not.
                if(fabs(delta) > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON) {

                    if(delta > 0) { //Expand widget to match parent size (if expandable)
                        float maxDelta = widget->getMaximizeDelta(axis);
                        if(maxDelta < delta) delta = maxDelta;
                        if(delta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON) {
                            widget->expand(axis, delta);
                            size = widget->getSize(axis);
                        }
                        if(parentSize > size) {
                            if(parentSize - size > underflow)
                                underflow = parentSize - size;
                        }


                    } else if(delta < 0) {
                        //Attempt to shrink widgets to match parent size (if shrinkable)
                        delta = fabs(delta);
                        float minDelta = widget->getMinimizeDelta(axis);
                        if(minDelta < delta) delta = minDelta;
                        if(delta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON) {
                            widget->shrink(axis, delta);
                            size = widget->getSize(axis);
                        }
                        //Record any leftover size that could not be shrunk
                        if(size > parentSize) {
                            if(size - parentSize > overflow)
                                overflow = size - parentSize;
                        }

                    }

                }
            }

            float finalDelta = overflow - underflow;

            //Resize parent to accomodate child widgets (which cannot be shrunk any further)
            if(finalDelta > 0.0f) {
                const float maxDelta = _parent->getMaximizeDelta(axis);
                if(maxDelta < finalDelta) finalDelta = maxDelta; //We're basically fucked. Can't fit widget. Don't try.
                if(finalDelta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON)
                    _parent->expand(axis, finalDelta);
            } else if(finalDelta < 0.0f) {
                finalDelta = fabs(finalDelta);
                const float minDelta = _parent->getMinimizeDelta(axis);
                if(minDelta < finalDelta) finalDelta = minDelta;
                if(finalDelta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON)
                    _parent->shrink(axis, finalDelta);
            }

        } else {
            float curSize = 0.0f;

            for(auto& widget: _widgets) curSize += widget->getSize(axis);

            float delta = fabs(parentSize - curSize);

            //Do we give a shit about the size discrepency...
            if(delta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON) {

                //Check if layout size is smaller than parent
                if(curSize < parentSize) {

                    //Try to evenly distribute the difference to expandable widgets
                    int intDelta = (int)delta;
                    while(intDelta) {
                        bool didExpand = false;

                        for(auto&& widget: _widgets) {
                            if(widget->getSizePolicy(axis) & Widget::growFlag) {
                                const float widgetSize = widget->getSize(axis);

                                if(widgetSize+1.0f <= widget->getMaxSize(axis)) {
                                    widget->setSize(axis, widgetSize+1.0f);
                                    didExpand = true;
                                    if(--intDelta == 0) break;
                                }
                            }
                        }
                        if(!didExpand) break;
                    }
#if 0
                    //Try to shrink parent instead
                    if(intDelta) {
                        float minDelta = _parent->getMinimizeDelta(axis);
                        if((float)intDelta < minDelta) minDelta = (float)intDelta;
                        if(minDelta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON)
                            _parent->shrink(axis, minDelta);
                    }
#endif

                //Check if layout size is larger than parent
                } else if(curSize > parentSize) {
                    //Make widgets smaller first?
                    //Make parent bigger
                    const float parentMaxDelta = _parent->getMaximizeDelta(axis);
                    if(parentMaxDelta < delta) delta = parentMaxDelta;
                    if(delta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON)
                        _parent->expand(axis, delta);

                    //Request for the parent widget to expand to accomodate the child widgets


                }
            }
        }

    }

    void Layout::_updatePositions(AXIS axis_) {
        GY_ASSERT(_parent);
        const Widget::AXIS axis = (Widget::AXIS)axis_;
        const float parentPos     = _parent->getPosition(axis);

        if(_groupingAxis != axis_) {
            for(auto& widget : _widgets) {
                widget->setPosition(axis, parentPos);
            }
        } else {

            float startPos = parentPos - 0.5f*_parent->getSize(axis);
            float endPos;

            for(auto& widget : _widgets) {
                endPos = startPos + widget->getSize(axis);
                widget->setPosition(axis, (endPos+startPos)*0.5f);
                startPos = endPos;
            }

            //Positional over/underflow
            //int delta = (parentPos + 0.5f*_parent->getSize(axis)) - endPos

        }

    }

    void Layout::setWidgetsLua(LuaVariant array) {
        _widgets.clear();
        for(int i = 1; i <= array.getLength(); ++i) {
            _widgets.push_back(array[i]->getValue<Widget*>());
        }
    }

    LuaVariant Layout::getWidgetsLua(void) const {
        LuaVariant array = {};
        for(size_t i = 0; i < _widgets.size(); ++i) {
            array[(int)i+1].setVariant(_widgets[i]);
        }
        return array;
    }

    void Layout::insertWidget(unsigned index, Widget *widget) {
        int i = 0;
        //Check if we already contain the bitch at some other index...
        for(; i < (int)_widgets.size(); ++i) {
            if(_widgets[i] == widget) break;
        }

        if(i < (int)_widgets.size()) {
            if(i == (int)index) return; //It's already where you wanted it, asswipe.
            else _widgets.erase(_widgets.begin() + i);
        }

        if (index < (unsigned)_widgets.size()) {
            _widgets.insert(_widgets.begin() + index, widget);
        } else { //Either this is what you wanted, or you're a dumbass and that index is too large, so we'll just append it...
            _widgets.push_back(widget);
        }
    }

    void GridLayout::_updateSizes(AXIS axis) {


    }



}
