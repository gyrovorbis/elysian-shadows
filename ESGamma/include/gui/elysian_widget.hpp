#ifndef ELYSIAN_WIDGET_HPP
#define ELYSIAN_WIDGET_HPP

#include <primitive/elysian_instance_transform.hpp>

#define ELYSIAN_WIDGET_Z_OFFSET             0.1f

#define ELYSIAN_SIZE_POLICY_GROW_FLAG_POS   0x0
#define ELYSIAN_SIZE_POLICY_SHRINK_FLAG_POS 0x1
#define ELYSIAN_SIZE_POLICY_EXPAND_FLAG_POS 0x2
#define ELYSIAN_SIZE_POLICY_IGNORE_FLAG_POS 0x3

namespace elysian {

    class Layout;

    //maybe want alpha values/states so all menu shit can fade out?
    class Widget {
    public:

        static const constexpr uint16_t growFlag   = (1<<ELYSIAN_SIZE_POLICY_GROW_FLAG_POS);
        static const constexpr uint16_t shrinkFlag = (1<<ELYSIAN_SIZE_POLICY_SHRINK_FLAG_POS);
        static const constexpr uint16_t expandFlag = (1<<ELYSIAN_SIZE_POLICY_EXPAND_FLAG_POS);
        static const constexpr uint16_t ignoreFlag = (1<<ELYSIAN_SIZE_POLICY_IGNORE_FLAG_POS);

        enum AXIS {
            X, Y, AXIS_COUNT
        };

        enum SIZE_POLICY {
            FIXED               = 0,
            MINIMUM             = growFlag,
            MAXIMUM             = shrinkFlag,
            PREFERRED           = shrinkFlag | growFlag,
            EXPANDING           = expandFlag | shrinkFlag | growFlag,
            MINIMUM_EXPANDING   = expandFlag | growFlag,
            IGNORE              = ignoreFlag | shrinkFlag | growFlag
        };

    protected:
        Vector2         _sizeHint;
        Vector2         _margins;
        SIZE_POLICY     _sizePolicy[AXIS_COUNT] = { FIXED };
        Layout*         _layout                 = nullptr;
        Layout*         _parentLayout           = nullptr;
        bool            _enabled                = true;

    public:

        virtual         ~Widget(void);

        Vector2         getPosition(void) const;
        void            setPosition(Vector2 pos);

        Vector2         getSize(void) const;
        void            setSize(Vector2 size);

        Vector2         getSizeHint(void) const;
        void            setSizeHint(Vector2 size);
        float           getSizeHint(AXIS axis) const;

        Vector2         getMaxSize(void) const;
        Vector2         getMaxSize(void);
        float           getMaxSize(AXIS axis) const;
        void            setMaxSize(Vector2 size);

        Vector2         getMinSize(void) const;
        Vector2         getMinSize(void);
        float           getMinSize(AXIS const) const;
        void            setMinSize(Vector2 size);

        SIZE_POLICY     getHorizontalSizePolicy(void) const;
        SIZE_POLICY     getVerticalSizePolicy(void) const;
        SIZE_POLICY     getSizePolicy(AXIS axis) const;
        void            setSizePolicy(AXIS axis, SIZE_POLICY policy);
        void            setHorizontalSizePolicy(SIZE_POLICY policy);
        void            setVerticalSizePolicy(SIZE_POLICY policy);
        void            setSizePolicy(SIZE_POLICY horizontal, SIZE_POLICY vertical);

        Vector2         getMarginSize(void) const;
        Vector2         getMarginSize(void);
        float           getMarginSize(AXIS axis) const;
        void            setMarginSize(Vector2 size);

        Layout*         getLayout(void) const;
        void            setLayout(Layout* layout);
        Layout*         getParentLayout(void) const;
        void            setParentLayout(Layout* layout);

        bool            isEnabled(void) const;
        void            setEnabled(bool enabled=true);
        void            enable(void);
        void            disable(void);

        int             getMaximizeDelta(AXIS axis) const;
        int             getMinimizeDelta(AXIS axis) const;

        void            expand(AXIS axis, int delta);
        void            shrink(AXIS axis, int delta);
        void            maximize(AXIS axis);
        void            maximize(void);
        void            minimize(AXIS axis);
        void            minimize(void);

        void            renderLayout(void) const;

        //Must be implemented by each widget
        virtual float   getPosition(AXIS axis) const        = 0;
        virtual void    setPosition(AXIS axis, float pos)   = 0;

        virtual float   getPositionZ(void) const            = 0;
        virtual void    setPositionZ(float pos)             = 0;

        virtual float   getSize(AXIS axis) const            = 0;
        virtual void    setSize(AXIS axis, float size)      = 0;

        virtual void    render(void)                        = 0;

    };



    //A completely blank, nongraphic widget just used for grouping other widgets with its layout
    class GroupWidget: public Widget {
    private:
        Vector3         _pos;
        Vector2         _size;
    public:
        virtual float   getPosition(AXIS axis) const override;
        virtual void    setPosition(AXIS axis, float pos) override;
        virtual float   getPositionZ(void) const override;
        virtual void    setPositionZ(float pos) override;

        virtual float   getSize(AXIS axis) const override;
        virtual void    setSize(AXIS axis, float size) override;

        virtual void    render(void) override;

    };






    //========== INLINEZ =============
    inline Layout* Widget::getLayout(void) const { return _layout; }
    inline Vector2 Widget::getPosition(void) const { return Vector2(getPosition(X), getPosition(Y)); }
    inline void Widget::setPosition(Vector2 pos) { setPosition(X, pos.x); setPosition(Y, pos.y); }
    inline Vector2 Widget::getSize(void) const { return Vector2(getSize(X), getSize(Y)); }
    inline void Widget::setSize(Vector2 size) { setSize(X, size.x); setSize(Y, size.y); }
    inline Vector2 Widget::getSizeHint(void) const { return _sizeHint; }
    inline void Widget::setSizeHint(Vector2 size) { _sizeHint = size; }
    inline float Widget::getSizeHint(AXIS axis) const { return _sizeHint[axis]; }
    inline Vector2 Widget::getMarginSize(void) const { return _margins; }
    inline Vector2 Widget::getMarginSize(void) { return Vector2(&_margins); }
    inline float Widget::getMarginSize(AXIS axis) const { return (axis == X)? _margins.x : _margins.y; }
    inline void Widget::setMarginSize(Vector2 size) { _margins = size; }
    inline auto Widget::getSizePolicy(AXIS axis) const -> SIZE_POLICY { return _sizePolicy[axis]; }
    inline void Widget::setSizePolicy(AXIS axis, SIZE_POLICY policy) { _sizePolicy[axis] = policy; }
    inline auto Widget::getHorizontalSizePolicy(void) const -> SIZE_POLICY { return _sizePolicy[X]; }
    inline auto Widget::getVerticalSizePolicy(void) const -> SIZE_POLICY { return _sizePolicy[Y]; }
    inline void Widget::setHorizontalSizePolicy(SIZE_POLICY policy) { _sizePolicy[X] = policy; }
    inline void Widget::setVerticalSizePolicy(SIZE_POLICY policy) { _sizePolicy[Y] = policy; }
    inline void Widget::setSizePolicy(SIZE_POLICY horizontal, SIZE_POLICY vertical) { _sizePolicy[X] = horizontal; _sizePolicy[Y] = vertical; }
    inline bool Widget::isEnabled(void) const { return _enabled; }
    inline void Widget::setEnabled(bool enabled) { _enabled = enabled; }
    inline void Widget::disable(void) { _enabled = false; }
    inline void Widget::enable(void) { _enabled = true; }
    inline void Widget::maximize(void) { maximize(X); maximize(Y); }
    inline void Widget::minimize(void) { minimize(X); minimize(Y); }

    inline float GroupWidget::getPosition(AXIS axis) const { return _pos[axis]; }
    inline void GroupWidget::setPosition(AXIS axis, float pos) { _pos[axis] = pos; }
    inline float GroupWidget::getSize(AXIS axis) const { return _size[axis]; }
    inline void GroupWidget::setSize(AXIS axis, float size) { _size[axis] = size; }
    inline void GroupWidget::render(void) { renderLayout(); }
    inline void GroupWidget::setPositionZ(float pos) { _pos.z = pos; }
    inline float GroupWidget::getPositionZ(void) const { return _pos.z; }

}

#endif // ELYSIAN_WIDGET_HPP
