#ifndef ELYSIAN_WIDGET_HPP
#define ELYSIAN_WIDGET_HPP

namespace elysian {

    class Layout;

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
        int             _sizePolicy[AXIS_COUNT]         = { FIXED };
        Layout*         _layout                         = nullptr;

    public:

        virtual         ~Widget(void);

        Layout*         getLayout(void) const;
        void            setLayout(Layout* layout);

        int             getMaxSize(AXIS axis) const;
        void            setMaxSize(AXIS axis, int size);

        int             getMinSize(AXIS axis) const;
        void            setMinSize(AXIS axis, int size);

        virtual int     getPosition(AXIS axis) const    = 0;
        virtual int     setPosition(AXIS axis)          = 0;

        virtual int     getSize(AXIS axis) const        = 0;
        virtual void    setSize(AXIS axis, int size)    = 0;

    };







    //========== INLINEZ =============
    inline Layout* Widget::getLayout(void) const { return _layout; }
    inline void Widget::setLayout(Layout *layout) { _layout = layout; }
    inline int Widget::getMaxSize(AXIS axis) const { return _maxSize[axis]; }
    inline void Widget::setMaxSize(AXIS axis, int size) { _maxSize[axis] = size; }
    inline int Widget::getMinSize(AXIS axis) const { return _minSize[axis]; }
    inline void Widget::setMinSize(AXIS axis, int size) { _minSize[axis] = size; }


}

#endif // ELYSIAN_WIDGET_HPP
