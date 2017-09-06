#ifndef ELYSIAN_MENU_WIDGET_HPP
#define ELYSIAN_MENU_WIDGET_HPP

#include <gui/elysian_widget.hpp>
#include <utility/elysian_associative_aggregate_array.hpp>
#include <utility/elysian_cpp_lua_callback.hpp>

#define ELYSIAN_MENU_WIDGET_INPUT_INDEX 0 //holy fuck, fix me!!!
#define ELYSIAN_MENU_WIDGET_SCROLL_BUTTON_HELD_START_TIME 1.0f//scroll fast after holding dpad for one sec?
#define ELYSIAN_MENU_WIDGET_INVALID_INDEX       -2

#define ELYSIAN_MENU_WIDGET_FLAGS_SELECTABLE    0x01
#define ELYSIAN_MENU_WIDGET_FLAGS_MULTI_SELECT  0x02
#define ELYSIAN_MENU_WIDGET_FLAGS_SCROLL_WRAP   0x04
#define ELYSIAN_MENU_WIDGET_FLAGS_SCROLL_HOLD   0x08
#define ELYSIAN_MENU_WIDGET_FLAGS_RESET_CURRENT 0x10
#define ELYSIAN_MENU_WIDGET_FLAGS_CANCEL_BUTTON 0x20


#define ELYSIAN_MENU_WIDGET_FLAGS_DEFAULT (ELYSIAN_MENU_WIDGET_FLAGS_SELECTABLE | ELYSIAN_MENU_WIDGET_FLAGS_SCROLL_WRAP)

namespace elysian {

    class MenuWidgetOptionDelegate {
        virtual void renderOptionWidget(Widget* option, bool selected, bool current) = 0;
        //some shit about cursors
    };


    struct MenuSelectionData {
        int index = ELYSIAN_MENU_WIDGET_INVALID_INDEX;
    };


    class MenuSelection:
            public AssociativeAggregateArray<Widget*, MenuSelectionData> {



    };

    /* TODO: SCROLLBAR SUPPORT!!!111??!
     *
     */
    class MenuWidget: public GroupWidget {
    public:
        using CurrentChangeCb       = CppLuaCallback<int, Widget*, Widget*>; //FUCKING FIXME!!!!!
        using SelectionChangeCb     = CppLuaCallback<int, void*, void*>;
        using CancelCb              = CppLuaCallback<int, Widget*>;

        const static int    InvalidIndex = ELYSIAN_MENU_WIDGET_INVALID_INDEX;

    private:
        MenuSelection       _selection;
        SelectionChangeCb   _selectionChangeCb;
        CurrentChangeCb     _currentChangeCb;
        Widget*             _cursor                     = nullptr;
        Widget*             _currentWidget              = nullptr;
        int                 _currentIndex[AXIS_COUNT]   = { InvalidIndex };
        uint32_t            _flags                      = ELYSIAN_MENU_WIDGET_FLAGS_DEFAULT;

        int             _getIndexFromWidget(const Widget* widget);
        Widget*         _getWidgetFromIndex(int index);
        void            _setCurrent(Widget* next, int index);
        void            _onCurrentChange(Widget* prev, Widget* next);
        void            _onSelectionChange(MenuSelection* sel);
        int             _toWrapIndex(int index) const;

    protected:

        //Overriddable within children
        virtual void    onSelectionChange(MenuSelection* sel);
        virtual void    onCurrentChange(Widget* pev, Widget* next);
        virtual void    onCancel(void);

    public:
                        MenuWidget(void) = default;
        virtual         ~MenuWidget(void);

        void            setCursor(Widget* cursor);
        Widget*         getCursor(void) const;

        uint32_t        getFlags(void) const;
        void            setFlags(uint32_t flags);

        Widget*         getCurrentWidget(void) const;
        void            setCurrentWidget(Widget* selection);
        void            setCurrentIndex(int index);

        auto            getSelection(void) -> const MenuSelection*;
        auto            getSelection(void) const -> const MenuSelection*;
        void            setSelection(const MenuSelection& selection);
        void            clearSelection(void);

        //Overridden from GroupWidget
        virtual void    render(void) const override;
        virtual void    controllerEvent(const Controller& cont) override;
        virtual void    virtualDPadEvent(const VirtualDPad& dpad) override;
        virtual void    activate(void) override;

    };







//========== INLINEZ ==========

inline Widget* MenuWidget::getCurrentWidget(void) const {
    return _currentWidget;
}

inline void MenuWidget::setCurrentIndex(int index) {
    int wrapIndex = _toWrapIndex(index);
    _setCurrent(_getWidgetFromIndex(wrapIndex), wrapIndex);
}

inline void MenuWidget::setCurrentWidget(Widget* widget) {
    _setCurrent(widget, _getIndexFromWidget(widget));
}

inline void MenuWidget::setCursor(Widget* cursor) {
    _cursor = cursor; //Do we need to apply it to something or some shit?
}

inline const MenuSelection* MenuWidget::getSelection(void) const {
    return _selection.getCount()? &_selection : nullptr;
}

inline const MenuSelection* MenuWidget::getSelection(void) {
    return _selection.getCount()? &_selection : nullptr;
}

inline void MenuWidget::_onCurrentChange(Widget* prevSel, Widget* nextSel) {
    if(_selectionChangeCb) _selectionChangeCb((void*)prevSel, (void*)nextSel);
    else onCurrentChange(prevSel, nextSel);
}

inline void MenuWidget::clearSelection(void) { _selection.clear(); }

inline Widget* MenuWidget::getCursor(void) const { return _cursor; }


inline uint32_t MenuWidget::getFlags(void) const { return _flags; }
inline void MenuWidget::setFlags(uint32_t flags) { _flags = flags; }


}

#endif // ELYSIAN_MENU_WIDGET_HPP
