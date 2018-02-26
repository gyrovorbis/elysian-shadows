#ifndef ELYSIAN_WIDGET_HPP
#define ELYSIAN_WIDGET_HPP

#include <primitive/elysian_instance_transform.hpp>
#include <crtp/elysian_input_system.hpp>

#define ELYSIAN_SIZE_POLICY_GROW_FLAG_POS   0x0
#define ELYSIAN_SIZE_POLICY_SHRINK_FLAG_POS 0x1
#define ELYSIAN_SIZE_POLICY_EXPAND_FLAG_POS 0x2
#define ELYSIAN_SIZE_POLICY_IGNORE_FLAG_POS 0x3

#define ELYSIAN_FRAME_WIDGET_LAYOUT_TYPE_DEFAULT elysian::HorizontalLayout

namespace elysian {

    class Controller;
    class Keyboard;
    class Mouse;
    class VirtualDPad;
    class TouchDevice;
    class Layout;

    //maybe want alpha values/states so all menu shit can fade out?
    class Widget: public EventTarget, public KeyboardEventObserver {
    public:

        static const constexpr uint16_t growFlag   = (1u<<ELYSIAN_SIZE_POLICY_GROW_FLAG_POS);
        static const constexpr uint16_t shrinkFlag = (1u<<ELYSIAN_SIZE_POLICY_SHRINK_FLAG_POS);
        static const constexpr uint16_t expandFlag = (1u<<ELYSIAN_SIZE_POLICY_EXPAND_FLAG_POS);
        static const constexpr uint16_t ignoreFlag = (1u<<ELYSIAN_SIZE_POLICY_IGNORE_FLAG_POS);

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
        uint16_t        _sizePolicy[AXIS_COUNT] = { EXPANDING, EXPANDING };
        Layout*         _layout                 = nullptr;
        Layout*         _parentLayout           = nullptr;
        bool            _enabled                = true;
        bool            _focus                  = false;
        bool            _mouseTracking          = false;


    public:

                        Widget(void) = default;
                        Widget(Layout* layout);
        virtual         ~Widget(void);

        Vector2         getPosition(void) const;
        void            setPosition(Vector2 pos);

        Vector2         getSize(void) const;
        void            setSize(Vector2 size);

        Vector2         getSizeHint(void) const;
        void            setSizeHint(Vector2 size);
        float           getSizeHint(AXIS axis) const;

        Vector2         getMaxSize(void) const;
        float           getMaxSize(AXIS axis) const;
        void            setMaxSize(Vector2 size);

        Vector2         getMinSize(void) const;
        float           getMinSize(AXIS const) const;
        void            setMinSize(Vector2 size);

        uint16_t        getHorizontalSizePolicy(void) const;
        uint16_t        getVerticalSizePolicy(void) const;
        uint16_t        getSizePolicy(AXIS axis) const;
        void            setSizePolicy(AXIS axis, uint16_t policy);
        void            setHorizontalSizePolicy(uint16_t policy);
        void            setVerticalSizePolicy(uint16_t policy);
        void            setSizePolicy(uint16_t horizontal, uint16_t vertical);


        Vector2         getMarginSize(void) const;
        Vector2         getMarginSize(void);
        float           getMarginSize(AXIS axis) const;
        void            setMarginSize(Vector2 size);

        Layout*         getLayout(void) const;
        void            setLayout(Layout* layout);
        Layout*         getParentLayout(void) const;
        void            setParentLayout(Layout* layout);
        Widget*         findChildWidget(const char* name);
        Widget*         getChildWidget(unsigned index);
        Widget*         getParentWidget(void) const;

        bool            isEnabled(void) const;
        void            setEnabled(bool enabled=true);
        void            enable(void);
        void            disable(void);
        bool            hasFocus(void) const;
        void            setFocus(bool enabled=true);
        void            setMouseTracking(bool enabled);
        bool            hasMouseTracking(void) const;

        float           getMaximizeDelta(AXIS axis) const;
        float           getMinimizeDelta(AXIS axis) const;

        void            expand(AXIS axis, float delta);
        void            shrink(AXIS axis, float delta);
        void            maximize(AXIS axis);
        void            maximize(void);
        void            minimize(AXIS axis);
        void            minimize(void);

        //overridden from EventReceiver
        virtual void    onReceiveEvent(Event* event) override;

        //overridden from EventObserver
        virtual void    onObserveEvent(Event *event, EventTarget* target) override;

        //Overriddabe by child widgets
        virtual void    activate(void);
        virtual void    deactivate(void);

        virtual void    update(void);
        virtual void    render(void) const;

        virtual void    onMouseMoveEvent(const Mouse& mouse);
        virtual void    onMouseWheelEvent(const Mouse& mouse);
        virtual void    onMouseButtonEvent(const Mouse& mouse);

        virtual void    controllerEvent(const Controller& cont);

        virtual void    touchEvent(const TouchDevice& touch);
        virtual void    virtualDPadEvent(const VirtualDPad& dpad);

        virtual float   getPosition(AXIS axis) const        = 0;
        virtual void    setPosition(AXIS axis, float pos)   = 0;

        virtual float   getPositionZ(void) const            = 0;
        virtual void    setPositionZ(float pos)             = 0;

        virtual float   getSize(AXIS axis) const            = 0;
        virtual void    setSize(AXIS axis, float size)      = 0;

    };



    //A completely blank, nongraphic widget just used for grouping other widgets with its layout
    class GroupWidget: public Widget {
    private:
        Vector3         _pos;
        Vector2         _size;
    public:
                        GroupWidget(void) = default;
                        GroupWidget(Layout* layout);

        virtual float   getPosition(AXIS axis) const override;
        virtual void    setPosition(AXIS axis, float pos) override;
        virtual float   getPositionZ(void) const override;
        virtual void    setPositionZ(float pos) override;

        virtual float   getSize(AXIS axis) const override;
        virtual void    setSize(AXIS axis, float size) override;

    };

    class FrameWidget: public GroupWidget {
    public:
                FrameWidget(void) = default;
                FrameWidget(Layout* layout);
                FrameWidget(Widget* centralWidget);
                FrameWidget(std::nullptr_t);
        void    setCentralWidget(Widget* centralWidget);
        Widget* getCentralWidget(void) const;
    };






    //========== INLINEZ =============
    inline Widget::Widget(Layout *layout) { setLayout(layout); }
    inline Layout* Widget::getLayout(void) const { return _layout; }
    inline Layout* Widget::getParentLayout(void) const { return _parentLayout; }
    inline Vector2 Widget::getPosition(void) const { return Vector2(getPosition(X), getPosition(Y)); }
    inline void Widget::setPosition(Vector2 pos) { setPosition(X, pos.getX()); setPosition(Y, pos.getY()); }
    inline Vector2 Widget::getSize(void) const { return Vector2(getSize(X), getSize(Y)); }
    inline void Widget::setSize(Vector2 size) { setSize(X, size.getX()); setSize(Y, size.getY()); }
    inline Vector2 Widget::getSizeHint(void) const { return _sizeHint; }
    inline void Widget::setSizeHint(Vector2 size) { _sizeHint = size; }
    inline float Widget::getSizeHint(AXIS axis) const { return _sizeHint[axis]; }
    inline Vector2 Widget::getMarginSize(void) const { return _margins; }
    inline Vector2 Widget::getMarginSize(void) { return Vector2(&_margins); }
    inline float Widget::getMarginSize(AXIS axis) const { return (axis == X)? _margins.x : _margins.y; }
    inline void Widget::setMarginSize(Vector2 size) { _margins = size; }
    inline uint16_t Widget::getSizePolicy(AXIS axis) const { return _sizePolicy[axis]; }
    inline void Widget::setSizePolicy(AXIS axis, uint16_t policy) { _sizePolicy[axis] = policy; }
    inline uint16_t Widget::getHorizontalSizePolicy(void) const { return _sizePolicy[X]; }
    inline uint16_t Widget::getVerticalSizePolicy(void) const { return _sizePolicy[Y]; }
    inline void Widget::setHorizontalSizePolicy(uint16_t policy) { _sizePolicy[X] = policy; }
    inline void Widget::setVerticalSizePolicy(uint16_t policy) { _sizePolicy[Y] = policy; }
    inline void Widget::setSizePolicy(uint16_t horizontal, uint16_t vertical) { _sizePolicy[X] = horizontal; _sizePolicy[Y] = vertical; }
    inline bool Widget::isEnabled(void) const { return _enabled; }
    inline void Widget::setEnabled(bool enabled) { _enabled = enabled; }
    inline void Widget::disable(void) { _enabled = false; }
    inline void Widget::enable(void) { _enabled = true; }
    inline bool Widget::hasFocus(void) const { return _focus; }
    inline bool Widget::hasMouseTracking(void) const { return _mouseTracking; }

    inline void Widget::maximize(void) { maximize(X); maximize(Y); }
    inline void Widget::minimize(void) { minimize(X); minimize(Y); }

    inline GroupWidget::GroupWidget(Layout *layout): Widget(layout) {}
    inline float GroupWidget::getPosition(AXIS axis) const { return _pos[axis]; }
    inline void GroupWidget::setPosition(AXIS axis, float pos) { _pos[axis] = pos; }
    inline float GroupWidget::getSize(AXIS axis) const { return _size[axis]; }
    inline void GroupWidget::setSize(AXIS axis, float size) { _size[axis] = size; }
    inline void GroupWidget::setPositionZ(float pos) { _pos.z = pos; }
    inline float GroupWidget::getPositionZ(void) const { return _pos.z; }



}

#endif // ELYSIAN_WIDGET_HPP
