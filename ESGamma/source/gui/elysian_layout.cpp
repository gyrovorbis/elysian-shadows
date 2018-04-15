#include <cstdlib>
#include <float.h>
#include <gui/elysian_widget.hpp>
#include <gui/elysian_layout.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_layout_proxy.hpp>
#include <proxy/elysian_widget_proxy.hpp>

namespace elysian {

    TableLayout::~TableLayout(void) {
        for(auto&& it: _widgets)
            delete it.second;
    }

    void TableLayout::insertWidget(unsigned row, unsigned col, Widget *widget) {
        _widgets[std::make_pair(row, col)] = widget;
        if(widget) widget->setParentLayout(this);
        _updateDimensions();
    }

    Widget* TableLayout::removeWidget(unsigned row, unsigned col) {
        auto key = std::make_pair(row, col);
        Widget* widget = _widgets[key];
        _widgets.erase(key);
        widget->setParentLayout(nullptr);
        _updateDimensions();
        return widget;
    }

    Widget* TableLayout::removeWidget(Widget* widget) {
        for(auto it = _widgets.begin(); it != _widgets.end(); ++it) {
            if((*it).second == widget) {
                _widgets.erase(it);
                widget->setParentLayout(nullptr);
                _updateDimensions();
                return widget;
            }
        }
        return nullptr;
    }

    void TableLayout::clearWidgets(void) {
        for(auto&& it: _widgets) it.second->setParentLayout(nullptr);
        _widgets.clear();
        _updateDimensions();
    }

    Widget* TableLayout::findWidget(const char *name) const {
        Widget* child = nullptr;
        for(auto&& it: _widgets) {
            if((child = it.second->findChildWidget(name))) {
                return child;
            }
        }
        return nullptr;
    }

    void TableLayout::render(void) const {
        for(auto&& it: _widgets) it.second->render();
    }

    void TableLayout::activate(void) const {
        for(auto&& it: _widgets) it.second->activate();
    }

    void TableLayout::deactivate(void) const {
        for(auto&& it: _widgets) it.second->deactivate();
    }

    void TableLayout::controllerEvent(const Controller &cont) {
        for(auto&& it: _widgets) if(it.second->hasFocus()) it.second->controllerEvent(cont);
    }

    void TableLayout::touchEvent(const TouchDevice &touch) {
        for(auto&& it: _widgets) it.second->touchEvent(touch);
    }

    void TableLayout::virtualDPadEvent(const VirtualDPad &vdpad) {
        for(auto&& it: _widgets) if(it.second->hasFocus()) it.second->virtualDPadEvent(vdpad);
    }

    void TableLayout::update(void) {
        for(auto& widget: _widgets) widget.second->update();

        _updateSizes(X);
        _updateSizes(Y);

        _updatePositions(X);
        _updatePositions(Y);
        _updatePositionsZ();
    }

    void TableLayout::_updatePositions(AXIS axis_) {
        GY_ASSERT(_parent);
        auto toRow = [&](unsigned i, unsigned j) {
            return (axis_ == X)? j : i;
        };
        auto toCol = [&](unsigned i, unsigned j) {
            return (axis_ == X)? i : j;
        };

        const Widget::AXIS axis = (Widget::AXIS)axis_;
        const float parentPos     = _parent->getPosition(axis);

        for(unsigned j = 0; j < getDimensionSize(getSecondaryAxis(axis_)); ++j) {
            float startPos = parentPos - 0.5f*_parent->getSize(axis);
            float endPos;

            for(unsigned i = 0; i < getDimensionSize(axis_); ++i) {
                Widget* widget = getWidget(toRow(i, j), toCol(i, j));
                endPos = startPos + widget->getSize(axis);
                widget->setPosition(axis, (endPos+startPos)*0.5f);
                startPos = endPos;
            }

        }
    }

    void TableLayout::_updatePositionsZ(void) {
        GY_ASSERT(_parent);
        float zCoord = _parent->getPositionZ() + ELYSIAN_GUI_LAYOUT_Z_COORD_OFFSET;
        for(auto&& widget : _widgets) {
            widget.second->setPositionZ(zCoord);
        }
    }

    void TableLayout::_updateSizes(AXIS axis) {
        GY_ASSERT(_parent);
        auto toRow = [&](unsigned i, unsigned j) {
            return (axis == X)? j : i;
        };
        auto toCol = [&](unsigned i, unsigned j) {
            return (axis == X)? i : j;
        };

        float totalSize                                           = 0.0f;
        float totalShrink                                         = 0.0f;
        float totalGrowth                                         = 0.0f;
        float largestSize[ELYSIAN_GRID_LAYOUT_DIM_MAX]            = { 0.0f };   //pendingSize
        float minSize[ELYSIAN_GRID_LAYOUT_DIM_MAX]                = { 0.0f };
        float maxSize[ELYSIAN_GRID_LAYOUT_DIM_MAX];
        for(unsigned i = 0; i < ELYSIAN_GRID_LAYOUT_DIM_MAX; ++i) maxSize[i] = FLT_MAX;

        int primarySize = getDimensionSize(axis);
        int secondarySize = getDimensionSize(getSecondaryAxis(axis));

        //Find sizing properties for each row/column and accumulate total sizes
        for(unsigned i = 0; i < primarySize; ++i) {
            for(unsigned j = 0; j < secondarySize; ++j) {
                Widget* w = getWidget(toRow(i, j), toCol(i, j));
                if(!w) continue;
                float size = w->getSize((Widget::AXIS)axis);
                if(size > largestSize[i]) largestSize[i] = size;
                float min = w->getMinSize((Widget::AXIS)axis);
                if(min > minSize[i]) minSize[i] = min;
                float max = w->getMaxSize((Widget::AXIS)axis);
                if(max < maxSize[i]) maxSize[i] = max;
            }
            totalSize   += largestSize[i];
            totalShrink += (largestSize[i] - minSize[i]);
            totalGrowth += (maxSize[i] - largestSize[i]);
        }

        float parentSize = _parent->getSize((Widget::AXIS)axis);
        float sizeDelta = fabs(parentSize - totalSize);

        //Check if there's a descrepency between total col/row sizes and parent size
        //(and whether we actually give a fuck or not)
        if(sizeDelta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON) {
            //Row/column size is larger than parents' size
            if(totalSize > parentSize) {
                float shrinkDelta       = min(totalShrink, sizeDelta);
                int intDelta = (int)(shrinkDelta+0.5f);

                //Shink rows/columns to match parent's size
                while(intDelta) {
                    bool didShrink = false;
                    for(unsigned i = 0; i < getDimensionSize(axis); ++i) {
                        if(1.0f < (largestSize[i] - minSize[i])) {
                            largestSize[i] -= 1.0f;
                            didShrink = true;
                            if(--intDelta <= 0) break;

                        }
                    }
                    if(!didShrink) break;
                }

                //Try to grow the parent by any remaining delta to match rows/cols
                const float parentMaxDelta = _parent->getMaximizeDelta((Widget::AXIS)axis);
                int growDelta = (intDelta < parentMaxDelta)? intDelta : parentMaxDelta;
                if(growDelta) {
                    _parent->expand((Widget::AXIS)axis, growDelta);
                }


            } else {

                float growDelta       = min(totalGrowth, sizeDelta);
                int intDelta = (int)(growDelta+0.5f);

                //Grow rows/columns to match parent's size.
                while(intDelta) {
                    bool didGrow = false;
                    for(unsigned i = 0; i < getDimensionSize(axis); ++i) {
                        if(1.0f < (maxSize[i] - largestSize[i])) {
                            largestSize[i] += 1.0f;
                            didGrow = true;
                            if(--intDelta <= 0) break;
                        }
                    }
                    if(!didGrow) break;
                }
#if 0
                //Try to shrink parent by any remaining delta to match rows/cols
                const float parentMinDelta = _parent->getMinimizeDelta((Widget::AXIS)axis);
                int shrinkDelta = (intDelta < parentMinDelta)? intDelta : parentMinDelta;
                if(shrinkDelta) {
                    _parent->shrink((Widget::AXIS)axis, shrinkDelta);
                }
#endif
            }

            //Actually apply new sizes to rows/cols
#if 1
            for(unsigned i = 0; i < getDimensionSize(axis); ++i) {
                for(unsigned j = 0; j < getDimensionSize(getSecondaryAxis(axis)); ++j) {
                    Widget* w = getWidget(toRow(i, j), toCol(i, j));

                    float size = w->getSize((Widget::AXIS)axis);
                    if(size < largestSize[i]) {
                        w->expand((Widget::AXIS)axis, largestSize[i]-size);
                    } else if(size > largestSize[i]) {
                        w->shrink((Widget::AXIS)axis, size-largestSize[i]);
                    }
                }
            }
#endif
        }

    }


    SingleAxisLayout::SingleAxisLayout(AXIS groupingAxis, const std::vector<Widget*>& widgets, Widget* parent):
        TableLayout(groupingAxis, parent)
    {
        setWidgets(widgets);
    }

    void SingleAxisLayout::insertWidget(unsigned index, Widget *widget) {
        if(getWidget(index) == widget) return; //already at that location, dickwisp.

        auto pair = _indexToRowColPair(index);

        bool relocate = false;
        for(auto&& it : _widgets) {
            if(it.second == widget) {
                relocate = true;
                break;
            }
        }

        if(relocate) {
            removeWidget(widget);

            int i = getWidgetCount() - 1;
            for(; i >= (int)index; --i) {
                auto newPair = _indexToRowColPair(i+1);
                TableLayout::insertWidget(newPair.first, newPair.second, getWidget(i));
            }
        }
        TableLayout::insertWidget(pair.first, pair.second, widget);
    }

    Widget* SingleAxisLayout::removeWidget(Widget* widget) {
        unsigned count = getWidgetCount();

        for(unsigned i = 0; i < count; ++i) {
            Widget* w = getWidget(i);
            if(w == widget) {
                for(unsigned j = i; j < count-1; ++j) {
                    auto pair = _indexToRowColPair(j);
                    TableLayout::insertWidget(pair.first, pair.second, getWidget(j+1));
                }
                auto lastPair = _indexToRowColPair(count-1);
                TableLayout::removeWidget(lastPair.first, lastPair.second);
                w->setParentLayout(nullptr);
                return w;
            }
        }

        return nullptr;
    }

    void SingleAxisLayout::setWidgets(const std::vector<Widget*>& widgets) {
        clearWidgets();
        for(size_t i = 0; i < widgets.size(); ++i) {
            auto pair = _indexToRowColPair(i);
            TableLayout::insertWidget(pair.first, pair.second, widgets[i]);
        }
    }

    auto SingleAxisLayout::getWidgets(void) const -> std::vector<Widget*> {
        std::vector<Widget*> widgets;
        for(size_t i = 0; i < _dimSize[getGroupingAxis()]; ++i) {
            widgets.push_back(getWidget(i));
        }
        return widgets;
    }

    void SingleAxisLayout::setWidgetsLua(LuaVariant array) {
        _widgets.clear();
        for(int i = 1; i <= array.getLength(); ++i) {
            auto pair = _indexToRowColPair(i);
            TableLayout::insertWidget(pair.first, pair.second, array[i]->getValue<Widget*>());
        }
    }

    LuaVariant SingleAxisLayout::getWidgetsLua(void) const {
        LuaVariant array = {};
        for(size_t i = 0; i < _dimSize[getGroupingAxis()]; ++i) {
            array[(int)i+1].setVariant(getWidget(i));
        }
        return array;
    }

    void SingleAxisLayout::_updateSizes(AXIS axis_) {
        GY_ASSERT(_parent);
        const Widget::AXIS axis = (Widget::AXIS)axis_;
        const float parentSize    = _parent->getSize(axis);

        if(axis_ != _groupingAxis) {
            float overflow = 0.0f;
            float underflow = 0.0f;

            for(int i = 0; i < getWidgetCount(); ++i) {
                Widget*       widget = getWidget(i);
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

            for(int i = 0; i < getWidgetCount(); ++i) {
                Widget*       widget = getWidget(i);
                curSize += widget->getSize(axis);
            }

            float delta = fabs(parentSize - curSize);

            //Do we give a shit about the size discrepency...
            if(delta > ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON) {

                //Check if layout size is smaller than parent
                if(curSize < parentSize) {

                    //Try to evenly distribute the difference to expandable widgets
                    int intDelta = (int)(delta + 0.5f);
                    while(intDelta) {
                        bool didExpand = false;

                        for(int i = 0; i < getWidgetCount(); ++i) {
                            Widget*       widget = getWidget(i);
                            if(widget->getSizePolicy(axis) & Widget::expandFlag) {
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

    void SingleAxisLayout::_updatePositions(AXIS axis_) {
        GY_ASSERT(_parent);
        const Widget::AXIS axis = (Widget::AXIS)axis_;
        const float parentPos     = _parent->getPosition(axis);

        if(_groupingAxis != axis_) {
            for(int i = 0; i < getWidgetCount(); ++i) {
                Widget*       widget = getWidget(i);
                widget->setPosition(axis, parentPos);
            }
        } else {

            float startPos = parentPos - 0.5f*_parent->getSize(axis);
            float endPos;

            for(int i = 0; i < getWidgetCount(); ++i) {
                Widget*       widget = getWidget(i);
                endPos = startPos + widget->getSize(axis);
                widget->setPosition(axis, (endPos+startPos)*0.5f);
                startPos = endPos;
            }

            //Positional over/underflow
            //int delta = (parentPos + 0.5f*_parent->getSize(axis)) - endPos

        }

    }





}
