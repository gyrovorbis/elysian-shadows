#ifndef ELYSIAN_WIDGET_HPP
#define ELYSIAN_WIDGET_HPP

namespace elysian {

    class Layout;

    //maybe want alpha values/states so all menu shit can fade out?
    class Widget {
    public:
        enum AXIS {
            X, Y, AXIS_COUNT
        };

        enum SIZE_POLICY {
            FIXED, EXPANDING, MINIMIZING, EXPANDING_MINIMIZING
        };

    protected:
        int             _minSize[AXIS_COUNT];
        int             _maxSize[AXIS_COUNT];
        SIZE_POLICY     _sizePolicy[AXIS_COUNT]         = { FIXED };
        int             _margins[AXIS_COUNT]            = { 0 };
        Layout*         _layout                         = nullptr;
        Layout*         _parentLayout                   = nullptr;
        bool            _active                         = true;

    public:

        virtual         ~Widget(void);

        int             getMaxSize(AXIS axis) const;
        void            setMaxSize(AXIS axis, int size);
        int             getMaxWidth(void) const;
        int             getMaxHeight(void) const;
        void            setMaxWidth(int width);
        void            setMaxHeight(int height);
        void            setMaxSize(int width, int height);

        int             getMinSize(AXIS axis) const;
        void            setMinSize(AXIS axis, int size);
        int             getMinWidth(void) const;
        int             getMinHeight(void) const;
        void            setMinWidth(int width);
        void            setMinHeight(int height);
        void            setMinSize(int width, int height);

        SIZE_POLICY     getSizePolicy(AXIS axis) const;
        void            setSizePolicy(AXIS axis, SIZE_POLICY policy);
        SIZE_POLICY     getHorizontalSizePolicy(void) const;
        SIZE_POLICY     getVerticalSizePolicy(void) const;
        void            setHorizontalSizePolicy(SIZE_POLICY policy);
        void            setVerticalSizePolicy(SIZE_POLICY policy);
        void            setSizePolicy(SIZE_POLICY horizontal, SIZE_POLICY vertical);

        int             getMarginSize(AXIS axis) const;
        void            setMarginSize(AXIS axis, int size);
        int             getMarginWidth(void) const;
        int             getMarginHeight(void) const;
        void            setMarginWidth(int width);
        void            setMarginHeight(int height);
        void            setMarginSize(int width, int height);

        Layout*         getLayout(void) const;
        void            setLayout(Layout* layout);
        Layout*         getParentLayout(void) const;
        void            setParentLayout(Layout* layout);

        bool            isActive(void) const;
        void            setActive(bool active);

        int             getMaximizeDelta(AXIS axis) const;
        int             getMinimizeDelta(AXIS axis) const;

        void            expand(AXIS axis, int delta);
        void            shrink(AXIS axis, int delta);
        void            maximize(AXIS axis);
        void            maximize(void);
        void            minimize(AXIS axis);
        void            minimize(void);

        void            renderLayout(void) const;

        virtual int     getPosition(AXIS axis) const    = 0;
        virtual void    setPosition(AXIS axis, int pos) = 0;

        virtual int     getSize(AXIS axis) const        = 0;
        virtual void    setSize(AXIS axis, int size)    = 0;

        virtual void    render(void) const              = 0;

    };







    //========== INLINEZ =============
    inline Layout* Widget::getLayout(void) const { return _layout; }
    inline int Widget::getMaxSize(AXIS axis) const { return _maxSize[axis]; }
    inline void Widget::setMaxSize(AXIS axis, int size) { _maxSize[axis] = size; }
    inline int Widget::getMaxWidth(void) const { return _maxSize[X]; }
    inline int Widget::getMaxHeight(void) const { return _maxSize[Y]; }
    inline void Widget::setMaxWidth(int width) { _maxSize[X] = width; }
    inline void Widget::setMaxHeight(int height) { _maxSize[Y] = height; }
    inline void Widget::setMaxSize(int width, int height) { _maxSize[X] = width; _maxSize[Y] = height; }
    inline int Widget::getMinSize(AXIS axis) const { return _minSize[axis]; }
    inline void Widget::setMinSize(AXIS axis, int size) { _minSize[axis] = size; }
    inline int Widget::getMinWidth(void) const { return _minSize[X]; }
    inline int Widget::getMinHeight(void) const { return _minSize[Y]; }
    inline void Widget::setMinWidth(int width) { _minSize[X] = width; }
    inline void Widget::setMinHeight(int height) { _minSize[Y] = height; }
    inline void Widget::setMinSize(int width, int height) { _minSize[X] = width; _minSize[Y] = height; }
    inline int Widget::getMarginSize(AXIS axis) const { return _margins[axis]; }
    inline void Widget::setMarginSize(AXIS axis, int size) { _margins[axis] = size; }
    inline int Widget::getMarginWidth(void) const { return _margins[X]; }
    inline int Widget::getMarginHeight(void) const { return _margins[Y]; }
    inline void Widget::setMarginWidth(int width) { _margins[X] = width; }
    inline void Widget::setMarginHeight(int height) { _margins[Y] = height; }
    inline void Widget::setMarginSize(int width, int height) { _margins[X] = width; _margins[Y] = height; }
    inline bool Widget::isActive(void) const { return _active; }
    inline void Widget::setActive(bool active) { _active = active; }
    inline void Widget::maximize(void) { maximize(X); maximize(Y); }
    inline void Widget::minimize(void) { minimize(X); minimize(Y); }
    inline auto Widget::getSizePolicy(AXIS axis) const -> SIZE_POLICY { return _sizePolicy[axis]; }
    inline void Widget::setSizePolicy(AXIS axis, SIZE_POLICY policy) { _sizePolicy[axis] = policy; }
    inline auto Widget::getHorizontalSizePolicy(void) const -> SIZE_POLICY { return _sizePolicy[X]; }
    inline auto Widget::getVerticalSizePolicy(void) const -> SIZE_POLICY { return _sizePolicy[Y]; }
    inline void Widget::setHorizontalSizePolicy(SIZE_POLICY policy) { _sizePolicy[X] = policy; }
    inline void Widget::setVerticalSizePolicy(SIZE_POLICY policy) { _sizePolicy[Y] = policy; }
    inline void Widget::setSizePolicy(SIZE_POLICY horizontal, SIZE_POLICY vertical) { _sizePolicy[X] = horizontal; _sizePolicy[Y] = vertical; }


}

#endif // ELYSIAN_WIDGET_HPP
