#ifndef ELYSIAN_LAYOUT_HPP
#define ELYSIAN_LAYOUT_HPP

#include <vector>
#include <crtp/elysian_lua_variant.hpp>

#define ELYSIAN_GUI_LAYOUT_Z_COORD_OFFSET   0.1f
#define ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON 0.1f

namespace elysian {

    class Widget;
    class Controller;
    class Mouse;
    class TouchDevice;
    class VirtualDPad;

    class Layout {
    public:
        enum AXIS {
            X, Y, AXIS_COUNT
        };

        enum ALIGNMENT {
            ALIGN_NEGATIVE,
            ALIGN_CENTER,
            ALIGN_POSITIVE
        };

    protected:
        std::vector<Widget*>    _widgets;
        Widget*                 _parent                 = nullptr;
        int                     _spacing[AXIS_COUNT]    = { 0 };
        ALIGNMENT               _alignment[AXIS_COUNT]  = { ALIGN_CENTER };
        const AXIS              _groupingAxis;

        void        _updateSizes(AXIS axis);
        void        _updateSizes(void);
        void        _updatePositions(AXIS axis);
        void        _updatePositions(void);
        void        _updatePositionsZ(void);

    public:

                    Layout(AXIS groupingAxis, Widget* parent=nullptr);
                    Layout(AXIS groupingAxis, std::vector<Widget*>&& widgets, Widget* parent=nullptr);
                    Layout(AXIS groupingAxis, std::vector<Widget*>& widgets, Widget* parent=nullptr);

                    ~Layout(void);

        void        addWidget(Widget* widget);
        Widget*     removeWidget(Widget* widget);   //Returns widget given as argument (for owernship change in Lua API).
        void        clearWidgets(void);
        void        setWidget(unsigned index, Widget* widget);
        void        setWidgets(std::vector<Widget*> widgets);
        void        setWidgetsLua(LuaVariant array);
        auto        getWidgets(void) const -> const std::vector<Widget*>&;
        auto        getWidgets(void) -> std::vector<Widget*>&;
        LuaVariant  getWidgetsLua(void) const;
        unsigned    getWidgetCount(void) const;
        Widget*     getWidget(unsigned index) const;
        Widget*     findWidget(const char* name) const;

        AXIS        getGroupingAxis(void) const;
        Widget*     getParent(void) const;
        void        setParent(Widget* parent);

        int         getSpacing(AXIS axis) const;
        void        setSpacing(AXIS axis, int spacing);
        int         getHorizontalSpacing(void) const;
        int         getVerticalSpacing(void) const;
        void        setHorizontalSpacing(int spacing);
        void        setVerticalSpacing(int spacing);
        void        setSpacing(int horizontal, int vertical);

        ALIGNMENT   getAlignment(AXIS axis) const;
        void        setAlignment(AXIS axis, ALIGNMENT align);
        ALIGNMENT   getHorizontalAlignment(void) const;
        ALIGNMENT   getVerticalAlignment(void) const;
        void        setHorizontalAlignment(ALIGNMENT align);
        void        setVerticalAlignment(ALIGNMENT align);
        void        setAlignment(ALIGNMENT horizontal, ALIGNMENT vertical);

        void        activate(void) const;
        void        deactivate(void) const;
        void        update(void);
        void        render(void) const;

        void        controllerEvent(const Controller& cont);
        void        keyboardEvent(const Keyboard& keybd);
        void        mouseEvent(const Mouse& mouse);
        void        touchEvent(const TouchDevice& touch);
        void        virtualDPadEvent(const VirtualDPad& dpad);
    };


    class HorizontalLayout: public Layout {
    public:

                    HorizontalLayout(Widget* parent=nullptr);
                    HorizontalLayout(std::vector<Widget*>&& widgets, Widget* parent=nullptr);
                    HorizontalLayout(std::vector<Widget*>& widgets, Widget* parent=nullptr);

    };

    class VerticalLayout: public Layout {
    public:
                    VerticalLayout(Widget* parent=nullptr);
                    VerticalLayout(std::vector<Widget*>&& widgets, Widget* parent=nullptr);
                    VerticalLayout(std::vector<Widget*>& widgets, Widget* parent=nullptr);

    };







    //======== INLINEZ =============

    inline Layout::Layout(AXIS groupingAxis, Widget* parent):
        _parent(parent),
        _groupingAxis(groupingAxis){}

    inline Widget* Layout::getWidget(unsigned index) const {
        GY_ASSERT(index < _widgets.size());
        return _widgets[index];
    }
    inline unsigned Layout::getWidgetCount(void) const { return (unsigned)_widgets.size();}
    inline auto Layout::getGroupingAxis(void) const -> AXIS { return _groupingAxis; }
    inline Widget* Layout::getParent(void) const { return _parent; }
    inline void Layout::setParent(Widget* parent) { _parent = parent; }
    inline void Layout::setWidget(unsigned index, Widget *widget) {
        GY_ASSERT(index <= _widgets.size());
        if(index == _widgets.size())  _widgets.push_back(widget);   //Insertion
        else _widgets[index] = widget;                              //Modify existing
    }
    inline void Layout::setWidgets(std::vector<Widget*> widgets) { _widgets = std::move(widgets); }
    inline auto Layout::getWidgets(void) const -> const std::vector<Widget*>& { return _widgets; }
    inline auto Layout::getWidgets(void) -> std::vector<Widget*>& { return _widgets; }
    inline int Layout::getSpacing(AXIS axis) const { return _spacing[axis]; }
    inline void Layout::setSpacing(AXIS axis, int spacing) { _spacing[axis] = spacing; }
    inline int Layout::getHorizontalSpacing(void) const { return _spacing[X]; }
    inline int Layout::getVerticalSpacing(void) const { return _spacing[Y]; }
    inline void Layout::setHorizontalSpacing(int spacing) { _spacing[X] = spacing; }
    inline void Layout::setVerticalSpacing(int spacing) { _spacing[Y] = spacing; }
    inline void Layout::setSpacing(int horizontal, int vertical) { _spacing[X] = horizontal; _spacing[Y] = vertical; }
    inline auto Layout::getAlignment(AXIS axis) const -> ALIGNMENT { return _alignment[axis]; }
    inline void Layout::setAlignment(AXIS axis, ALIGNMENT align) { _alignment[axis] = align; }
    inline auto Layout::getHorizontalAlignment(void) const -> ALIGNMENT { return _alignment[X]; }
    inline auto Layout::getVerticalAlignment(void) const -> ALIGNMENT { return _alignment[Y]; }
    inline void Layout::setHorizontalAlignment(ALIGNMENT align) { _alignment[X] = align; }
    inline void Layout::setVerticalAlignment(ALIGNMENT align) { _alignment[Y] = align; }
    inline void Layout::setAlignment(ALIGNMENT horizontal, ALIGNMENT vertical) { _alignment[X] = horizontal; _alignment[Y] = vertical; }
    inline void Layout::_updatePositions(void) { _updatePositions(X); _updatePositions(Y); }
    inline void Layout::_updateSizes(void) { _updateSizes(X); _updateSizes(Y); }

    inline HorizontalLayout::HorizontalLayout(Widget* parent):
        Layout(X, parent) {}

    inline HorizontalLayout::HorizontalLayout(std::vector<Widget*>&& widgets, Widget* parent):
        Layout(X, std::move(widgets), parent)
        {}

    inline HorizontalLayout::HorizontalLayout(std::vector<Widget*>& widgets, Widget* parent):
        Layout(X, widgets, parent)
        {}

    inline VerticalLayout::VerticalLayout(Widget* parent):
        Layout(Y, parent) {}

    inline VerticalLayout::VerticalLayout(std::vector<Widget*>&& widgets, Widget* parent):
        Layout(Y, std::move(widgets), parent)
        {}

    inline VerticalLayout::VerticalLayout(std::vector<Widget*>& widgets, Widget* parent):
        Layout(Y, widgets, parent)
        {}
}

#endif // ELYSIAN_LAYOUT_HPP
