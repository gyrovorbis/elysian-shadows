#ifndef ELYSIAN_MENU_WIDGET_HPP
#define ELYSIAN_MENU_WIDGET_HPP

#include <gui/elysian_widget.hpp>
#include <utility/elysian_associative_aggregate_array.hpp>
#include <utility/elysian_cpp_lua_callback.hpp>
#include <system/elysian_event_system.hpp>
#include <gui/elysian_item_model.hpp>

#define ELYSIAN_MENU_WIDGET_INPUT_INDEX 0 //holy fuck, fix me!!!
#define ELYSIAN_MENU_WIDGET_SCROLL_BUTTON_HELD_START_TIME 1.0f//scroll fast after holding dpad for one sec?
#define ELYSIAN_MENU_WIDGET_INVALID_INDEX       -2

#define ELYSIAN_MENU_WIDGET_FLAGS_SELECTABLE    0x01
#define ELYSIAN_MENU_WIDGET_FLAGS_MULTI_SELECT  0x02
#define ELYSIAN_MENU_WIDGET_FLAGS_SCROLL_WRAP   0x04
#define ELYSIAN_MENU_WIDGET_FLAGS_SCROLL_HOLD   0x08
#define ELYSIAN_MENU_WIDGET_FLAGS_RESET_CURRENT 0x10
#define ELYSIAN_MENU_WIDGET_FLAGS_CANCEL_BUTTON 0x20


#define ELYSIAN_MENU_WIDGET_FLAGS_DEFAULT (ELYSIAN_MENU_WIDGET_FLAGS_SELECTABLE/* | ELYSIAN_MENU_WIDGET_FLAGS_SCROLL_WRAP*/)

namespace elysian {

    class ItemDelegate;

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

    class MenuCurrentItemChangedEvent: public elysian::Event {
    private:
        Widget* _prev;
        Widget* _next;
    public:
                MenuCurrentItemChangedEvent(Widget* prev, Widget* next);
        Widget* getPrevWidget(void) const;
        Widget* getNextWidget(void) const;
    };

    /* TODO: SCROLLBAR SUPPORT!!!111??!
     *
     */
    class MenuWidget: public GroupWidget {
    public:
        using CurrentChangeCb       = CppLuaCallback<void, Widget*, Widget*>; //FUCKING FIXME!!!!!
        using SelectionChangeCb     = CppLuaCallback<void, void*, void*>;
        using CancelCb              = CppLuaCallback<void, Widget*>;

        const static int    InvalidIndex = ELYSIAN_MENU_WIDGET_INVALID_INDEX;

    private:
        MenuSelection       _selection;
        SelectionChangeCb   _selectionChangeCb;
        CurrentChangeCb     _currentChangeCb;
        CancelCb            _cancelCb;
        Widget*             _cursor                     = nullptr;
        Widget*             _currentWidget              = nullptr;
        int                 _currentIndex[AXIS_COUNT]   = { InvalidIndex };
        uint32_t            _flags                      = ELYSIAN_MENU_WIDGET_FLAGS_DEFAULT;

        int             _getIndexFromWidget(const Widget* widget);
        Widget*         _getWidgetFromIndex(int index);
        void            _setCurrent(Widget* next, int index);
        void            _onCurrentChange(Widget* prev, Widget* next);
        void            _onSelectionChange(MenuSelection* sel);
        void            _onCancel(void);
        int             _toWrapIndex(int index) const;

    protected:

        //Overriddable within children
        virtual void    onSelectionChange(const MenuSelection* sel);
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
        int             getCurrentIndex(void) const;

        auto            getSelection(void) -> const MenuSelection*;
        auto            getSelection(void) const -> const MenuSelection*;
        void            setSelection(MenuSelection selection);
        void            clearSelection(void);

        void            setSelectionChangeCb(SelectionChangeCb cb);
        void            setCurrentChangeCb(CurrentChangeCb cb);
        void            setCancelCb(CancelCb cb);

        //Overridden from GroupWidget
        virtual void    render(void) const override;
        virtual void    controllerEvent(const Controller& cont) override;
        virtual void    virtualDPadEvent(const VirtualDPad& dpad) override;
        virtual void    activate(void) override;

        //Overridden from EventRecevier
        virtual void    onReceiveEvent(Event* event);

    };

    class MenuItemView: public MenuWidget {
    private:
        ItemModel*      _model              = nullptr;
        ItemDelegate*   _delegate           = nullptr; //use builtin delegate as default instead of nullptr later
        ItemModelIndex  _topLeftItem        = ItemModelIndex(0, 0);
        ItemModelIndex  _bottomRightItem    = ItemModelIndex(0, 0);

    public:
                        MenuItemView(ItemModel* model=nullptr, ItemDelegate* delegate=nullptr);
    virtual             ~MenuItemView(void) override;

    ItemModel*          getModel(void) const;
    void                setModel(ItemModel* model);
    ItemDelegate*       getDelegate(void) const;
    void                setDelegate(ItemDelegate* delegate);
    ItemModelIndex      getCurrentItemIndex(void) const;
    void                setCurrentItemIndex(const ItemModelIndex& index);
    unsigned            getVisibleRowCount(void) const;
    unsigned            getVisibleColCount(void) const;
    ItemModelIndex      getTopLeftModelIndex(void) const;
    ItemModelIndex      getBottomRightModelIndex(void) const;

    //Widget overrides
    virtual void        update(void) override;


    };



//========== INLINEZ ==========

inline MenuCurrentItemChangedEvent::MenuCurrentItemChangedEvent(Widget* prev, Widget* next):
    Event(EVENT_WIDGET_MENU_CURRENT),
    _prev(prev),
    _next(next)
{}

inline Widget* MenuCurrentItemChangedEvent::getPrevWidget(void) const { return _prev; }
inline Widget* MenuCurrentItemChangedEvent::getNextWidget(void) const { return _next; }

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

inline void MenuWidget::clearSelection(void) { _selection.clear(); }

inline Widget* MenuWidget::getCursor(void) const { return _cursor; }


inline uint32_t MenuWidget::getFlags(void) const { return _flags; }
inline void MenuWidget::setFlags(uint32_t flags) { _flags = flags; }

inline void MenuWidget::setSelectionChangeCb(SelectionChangeCb) {
  //  _selectionChangeCb = cb;
}
inline void MenuWidget::setCurrentChangeCb(CurrentChangeCb) {
    //_currentChangeCb = cb;
}
inline void MenuWidget::setCancelCb(CancelCb) {
    //_cancelCb = cb;
}

//===== MenuItemView ======
inline MenuItemView::MenuItemView(ItemModel* model, ItemDelegate* delegate):
    _model(model),
    _delegate(delegate) {}

inline MenuItemView::~MenuItemView(void) {
    delete _model;
    delete _delegate;
}

inline ItemDelegate* MenuItemView::getDelegate(void) const {
    return _delegate;
}

inline ItemModel* MenuItemView::getModel(void) const {
    return _model;
}

}

#endif // ELYSIAN_MENU_WIDGET_HPP
