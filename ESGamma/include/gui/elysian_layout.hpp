#ifndef ELYSIAN_LAYOUT_HPP
#define ELYSIAN_LAYOUT_HPP

#include <vector>
#include <crtp/elysian_lua_variant.hpp>

#define ELYSIAN_GUI_LAYOUT_Z_COORD_OFFSET   0.1f
#define ELYSIAN_GUI_LAYOUT_SIZE_POS_EPSILON 0.1f
#define ELYSIAN_GRID_LAYOUT_DIM_MAX         100

namespace elysian {

    class Widget;
    class Controller;
    class Mouse;
    class TouchDevice;
    class VirtualDPad;

    class TableLayout {
    public:
        typedef std::pair<int, int> RowColPair;

        enum AXIS {
            X, Y, AXIS_COUNT, X_Y
        };

        enum ALIGNMENT {
            ALIGN_NEGATIVE,
            ALIGN_CENTER,
            ALIGN_POSITIVE
        };

    protected:
        const static constexpr int      InvalidIndex            = -1;
        //
        const static constexpr int      DynamicSize             = -1;

        std::map<RowColPair, Widget*>   _widgets;
        unsigned                        _dimSize[AXIS_COUNT]    = { 0 };
        Widget*                         _parent                 = nullptr;
        int                             _spacing[AXIS_COUNT]    = { 0 };
        ALIGNMENT                       _alignment[AXIS_COUNT]  = { ALIGN_CENTER };
        const AXIS                      _groupingAxis;

        virtual void    _updateSizes(AXIS axis);
        virtual void    _updatePositions(AXIS axis);
        void            _updatePositionsZ(void);
        void            _updateDimensions(void);

    protected:
                    TableLayout(AXIS groupingAxis, Widget* parent=nullptr);

    public:

                    TableLayout(Widget* parent=nullptr);
        virtual     ~TableLayout(void);

        void        insertWidget(unsigned row, unsigned col, Widget* widget);
        virtual Widget*     removeWidget(Widget* widget);   //Returns widget given as argument (for owernship change in Lua API).
        Widget*     removeWidget(unsigned r, unsigned c);
        void        clearWidgets(void);
        Widget*     findWidget(const char* name) const;
        Widget*     getWidget(unsigned r, unsigned c) const;


        unsigned    getWidgetCount(void) const;
        unsigned    getRowCount(void) const;
        unsigned    getColumnCount(void) const;
        unsigned    getDimensionSize(AXIS axis) const;
        static auto toRowIndex(AXIS primaryAxis, unsigned primaryIndex, unsigned secondaryIndex) -> unsigned;
        static auto toColIndex(AXIS primaryAxis, unsigned primaryIndex, unsigned secondaryIndex) -> unsigned;

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
        void        touchEvent(const TouchDevice& touch);
        void        virtualDPadEvent(const VirtualDPad& dpad);

        static AXIS getSecondaryAxis(AXIS primary);
    };


    class SingleAxisLayout: public TableLayout {
    protected:

        RowColPair  _indexToRowColPair(int index) const;

        //overridden from TableLayout
        virtual void    _updateSizes(AXIS axis) override;
        virtual void    _updatePositions(AXIS axis) override;

    public:
                        SingleAxisLayout(AXIS groupingAxis, Widget* parent=nullptr);
                        SingleAxisLayout(AXIS groupingAxis, const std::vector<Widget*>& widgets, Widget* parent=nullptr);

        void            addWidget(Widget* widget);
        Widget*         getWidget(unsigned index) const;
        void            insertWidget(unsigned index, Widget* widget);
        Widget*         removeWidget(unsigned index);
        auto            getWidgets(void) const -> std::vector<Widget*>;
        LuaVariant      getWidgetsLua(void) const;
        void            setWidgets(const std::vector<Widget*>& widgets);
        void            setWidgetsLua(LuaVariant array);

        //Overridden from TableLayout
        virtual Widget* removeWidget(Widget* widget) override;   //Returns widget given as argument (for owernship change in Lua API).
    };

    class HorizontalLayout: public SingleAxisLayout {
    public:

                    HorizontalLayout(Widget* parent=nullptr);
                    HorizontalLayout(const std::vector<Widget*>& widgets, Widget* parent=nullptr);

    };

    class VerticalLayout: public SingleAxisLayout {
    public:
                    VerticalLayout(Widget* parent=nullptr);
                    VerticalLayout(const std::vector<Widget*>& widgets, Widget* parent=nullptr);

    };







    //===== INLINEZ ======
    inline TableLayout::TableLayout(AXIS groupingAxis, Widget* parent):
        _parent(parent),
        _groupingAxis(groupingAxis) {}
    inline TableLayout::TableLayout(Widget* parent):
        TableLayout(X_Y, parent) {}
    inline unsigned TableLayout::getWidgetCount(void) const { return (unsigned)_widgets.size(); }
    inline unsigned TableLayout::getRowCount(void) const { return _dimSize[Y]; }
    inline unsigned TableLayout::getColumnCount(void) const { return _dimSize[X]; }
    inline unsigned TableLayout::getDimensionSize(AXIS axis) const { return _dimSize[axis]; }
    inline auto TableLayout::getGroupingAxis(void) const -> AXIS { return _groupingAxis; }
    inline Widget* TableLayout::getParent(void) const { return _parent; }
    inline void TableLayout::setParent(Widget* parent) { _parent = parent; }
    inline int TableLayout::getSpacing(AXIS axis) const { return _spacing[axis]; }
    inline void TableLayout::setSpacing(AXIS axis, int spacing) { _spacing[axis] = spacing; }
    inline int TableLayout::getHorizontalSpacing(void) const { return _spacing[X]; }
    inline int TableLayout::getVerticalSpacing(void) const { return _spacing[Y]; }
    inline void TableLayout::setHorizontalSpacing(int spacing) { _spacing[X] = spacing; }
    inline void TableLayout::setVerticalSpacing(int spacing) { _spacing[Y] = spacing; }
    inline void TableLayout::setSpacing(int horizontal, int vertical) { _spacing[X] = horizontal; _spacing[Y] = vertical; }
    inline auto TableLayout::getAlignment(AXIS axis) const -> ALIGNMENT { return _alignment[axis]; }
    inline void TableLayout::setAlignment(AXIS axis, ALIGNMENT align) { _alignment[axis] = align; }
    inline auto TableLayout::getHorizontalAlignment(void) const -> ALIGNMENT { return _alignment[X]; }
    inline auto TableLayout::getVerticalAlignment(void) const -> ALIGNMENT { return _alignment[Y]; }
    inline void TableLayout::setHorizontalAlignment(ALIGNMENT align) { _alignment[X] = align; }
    inline void TableLayout::setVerticalAlignment(ALIGNMENT align) { _alignment[Y] = align; }
    inline void TableLayout::setAlignment(ALIGNMENT horizontal, ALIGNMENT vertical) { _alignment[X] = horizontal; _alignment[Y] = vertical; }
    inline Widget* TableLayout::getWidget(unsigned row, unsigned col) const {
        auto it = _widgets.find(std::make_pair(row, col));
        return (it != _widgets.end())? (*it).second : nullptr;
    }
    inline void TableLayout::_updateDimensions(void) {
        int rows = -1;
        int cols = -1;
        for(auto&& it: _widgets) {
            if((int)it.first.first > rows) rows = it.first.first;
            if((int)it.first.second > cols) cols = it.first.second;
        }
        _dimSize[Y] = rows + 1;
        _dimSize[X] = cols + 1;
    }
    inline auto TableLayout::getSecondaryAxis(AXIS primary) -> AXIS {
        return (primary == X)? Y : X;
    }
    inline auto TableLayout::toRowIndex(AXIS primaryAxis, unsigned primaryIndex, unsigned secondaryIndex) -> unsigned {
        return (primaryAxis == X)? secondaryIndex: primaryIndex;
    }
    inline auto TableLayout::toColIndex(AXIS primaryAxis, unsigned primaryIndex, unsigned secondaryIndex) -> unsigned {
        return (primaryAxis == X)? primaryIndex : secondaryIndex;
    }



    //==========================================================================
    inline SingleAxisLayout::SingleAxisLayout(AXIS groupingAxis, Widget* parent):
        TableLayout(groupingAxis, parent)
    {}

    inline auto SingleAxisLayout::_indexToRowColPair(int index) const -> RowColPair {
        switch(getGroupingAxis()) {
            case X: return std::make_pair(0, index);
            case Y: return std::make_pair(index, 0);
            default: GY_ASSERT(false); return std::make_pair(InvalidIndex, InvalidIndex);
        }
    }

    inline void SingleAxisLayout::addWidget(Widget* widget) {
        auto pair = _indexToRowColPair(getDimensionSize(getGroupingAxis()));
        TableLayout::insertWidget(pair.first, pair.second, widget);
    }

    inline Widget* SingleAxisLayout::getWidget(unsigned index) const {
        auto pair = _indexToRowColPair(index);
        return TableLayout::getWidget(pair.first, pair.second);
    }

    inline Widget* SingleAxisLayout::removeWidget(unsigned index) {
        auto pair = _indexToRowColPair(index);
        return TableLayout::removeWidget(pair.first, pair.second);
    }

    inline HorizontalLayout::HorizontalLayout(Widget* parent):
        SingleAxisLayout(X, parent) {}

    inline HorizontalLayout::HorizontalLayout(const std::vector<Widget *> &widgets, Widget *parent):
        SingleAxisLayout(X, widgets, parent) {    }

    inline VerticalLayout::VerticalLayout(Widget* parent):
        SingleAxisLayout(Y, parent) {}

    inline VerticalLayout::VerticalLayout(const std::vector<Widget *> &widgets, Widget *parent):
        SingleAxisLayout(Y, widgets, parent) {}





}

#endif // ELYSIAN_LAYOUT_HPP
