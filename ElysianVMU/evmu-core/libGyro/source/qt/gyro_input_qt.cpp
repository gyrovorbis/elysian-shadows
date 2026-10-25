#include <libGyro/gyro_input_api.h>
#include <QApplication>
#include <QKeyEvent>
#include <QGraphicsView>
#include <libGyro/gyro_system_api.h>

extern "C" int  _sdlControllerInit(const struct GYInputInitParams*const  params);
extern "C" int  _sdlControllerUninit(void);
extern "C" void _sdlControllerDump(void);

GYInputInitParams    _initParams;
static bool         _keysChangedThisFrame[GY_KEY_COUNT]                 = { false };
static bool         _mouseButtonsChangedThisFrame[GY_MOUSE_BUT_COUNT]   = { false };
static bool         _mousePosChangedThisFrame                           = false;
static bool         _mouseScrollWheelChangedThisFrame                   = false;

static inline GY_KEY _TranslateKeyFromQt(const int keyCode) {
    switch(keyCode) {
    case Qt::Key_Escape:
        //exit(1);
        return GY_KEY_ESC;
    case Qt::Key_Up:
        return GY_KEY_DPAD_U;
    case Qt::Key_Down:
        return GY_KEY_DPAD_D;
    case Qt::Key_Left:
        return GY_KEY_DPAD_L;
    case Qt::Key_Right:
        return GY_KEY_DPAD_R;
    case Qt::Key_A:
        return GY_KEY_A;
    case Qt::Key_B:
        return GY_KEY_B;
    case Qt::Key_C:
        return GY_KEY_C;
    case Qt::Key_D:
        return GY_KEY_D;
    case Qt::Key_E:
        return GY_KEY_E;
    case Qt::Key_F:
        return GY_KEY_F;
    case Qt::Key_G:
        return GY_KEY_G;
    case Qt::Key_H:
        return GY_KEY_H;
    case Qt::Key_I:
        return GY_KEY_I;
    case Qt::Key_J:
        return GY_KEY_J;
    case Qt::Key_K:
        return GY_KEY_K;
    case Qt::Key_L:
        return GY_KEY_L;
    case Qt::Key_M:
        return GY_KEY_M;
    case Qt::Key_N:
        return GY_KEY_N;
    case Qt::Key_O:
        return GY_KEY_O;
    case Qt::Key_P:
        return GY_KEY_P;
    case Qt::Key_Q:
        return GY_KEY_Q;
    case Qt::Key_R:
        return GY_KEY_R;
    case Qt::Key_S:
        return GY_KEY_S;
    case Qt::Key_T:
        return GY_KEY_T;
    case Qt::Key_U:
        return GY_KEY_U;
    case Qt::Key_V:
        return GY_KEY_V;
    case Qt::Key_W:
        return GY_KEY_W;
    case Qt::Key_X:
        return GY_KEY_X;
    case Qt::Key_Y:
        return GY_KEY_Y;
    case Qt::Key_Z:
        return GY_KEY_Z;
    case Qt::Key_0:
        return GY_KEY_0;
    case Qt::Key_1:
        return GY_KEY_1;
    case Qt::Key_2:
        return GY_KEY_2;
    case Qt::Key_3:
        return GY_KEY_3;
    case Qt::Key_4:
        return GY_KEY_4;
    case Qt::Key_5:
        return GY_KEY_5;
    case Qt::Key_6:
        return GY_KEY_6;
    case Qt::Key_7:
        return GY_KEY_7;
    case Qt::Key_8:
        return GY_KEY_8;
    case Qt::Key_9:
        return GY_KEY_9;
    case Qt::Key_Space:
        return GY_KEY_SPACE;
    case Qt::Key_Return:
        return GY_KEY_RETURN;
    case Qt::Key_Shift:
        return GY_KEY_LSHIFT;
    //case Qt::Key_Shift:
    //    return GY_KEY_RSHIFT;
    case Qt::Key_Control:
        return GY_KEY_LCTRL;
   // case Qt::Key_Control:
       // return GY_KEY_RCTRL;
    case Qt::Key_Alt:
        return GY_KEY_LALT;
    //case Qt::Key_Alt:
      //  return GY_KEY_RALT;
    case Qt::Key_Back:
        return GY_KEY_BACKQUOTE;
    case Qt::Key_Backslash:
        return GY_KEY_BACKSLASH;
    case Qt::Key_Slash:
        return GY_KEY_FORWARDSLASH;
    default:
        return GY_KEY_NONE;
    }
}

static inline GY_MOUSE_BUTTON _TranslateMouseButtonFromQt(Qt::MouseButton button) {
    switch(button) {
        case Qt::LeftButton:
            return GY_MOUSE_BUT_LEFT;
        case Qt::RightButton:
            return GY_MOUSE_BUT_RIGHT;
        case Qt::MiddleButton:
            return GY_MOUSE_BUT_MIDDLE;
        case Qt::BackButton:
            return GY_MOUSE_BUT_BACK;
        default: return GY_MOUSE_BUT_NONE;
    }
}

static inline void _setKeyDown(GYKeyboard *const keybd, const GY_KEY key) {
    unsigned newState = keybd->keys[key] | 0x1;
    keybd->keys[key] = (GY_BUTTON_STATE)newState;
    _keysChangedThisFrame[key] = true;
}

static inline void _setKeyUp(GYKeyboard *const keybd, const GY_KEY key) {
    //keybd->keys[key] = (GY_BUTTON_STATE)(((keybd->keys[key] << 0x1)) & 0x11);
    unsigned newState = (keybd->keys[key] & (~0x1));
    keybd->keys[key] = (GY_BUTTON_STATE)newState;
    _keysChangedThisFrame[key] = true;
}

static inline void _setMouseButtonDown(GYMouse *const mouse, const GY_MOUSE_BUTTON button) {
    unsigned newState = mouse->buttons[button] | 0x1;
    mouse->buttons[button] = (GY_BUTTON_STATE)newState;
    _mouseButtonsChangedThisFrame[button] = true;
}

static inline void _setMouseButtonUp(GYMouse *const mouse, const GY_MOUSE_BUTTON button) {
    unsigned newState = (mouse->buttons[button] & (~0x1));
    mouse->buttons[button] = (GY_BUTTON_STATE)newState;
    _mouseButtonsChangedThisFrame[button] = true;
}

class EventFilter: public QObject, public GYKeyboard, public GYMouse {

public:

    EventFilter(void) {
        memset(static_cast<GYKeyboard*>(this), 0, sizeof(GYKeyboard));
        memset(static_cast<GYMouse*>(this), 0, sizeof(GYMouse));
    }

    virtual ~EventFilter(void) = default;

    virtual bool eventFilter(QObject* /*object*/, QEvent* event) override {
        QKeyEvent*      keyEvent = static_cast<QKeyEvent*>(event);
        QMouseEvent*    mouseEvent = static_cast<QMouseEvent*>(event);
        QWheelEvent*    wheelEvent = static_cast<QWheelEvent*>(event);


        switch(event->type()) {
        case QEvent::KeyPress:
            _setKeyDown(static_cast<GYKeyboard*>(this), _TranslateKeyFromQt(keyEvent->key()));
            break;

        case QEvent::KeyRelease:
            _setKeyUp(static_cast<GYKeyboard*>(this), _TranslateKeyFromQt(keyEvent->key()));
            break;

        case QEvent::MouseMove:
            screenPos.x = mouseEvent->pos().x();
            screenPos.y = mouseEvent->pos().y();
            _mousePosChangedThisFrame = true;
            break;

         case QEvent::MouseButtonPress:
            _setMouseButtonDown(static_cast<GYMouse*>(this), _TranslateMouseButtonFromQt(mouseEvent->button()));
            break;

        case QEvent::MouseButtonRelease:
            _setMouseButtonUp(static_cast<GYMouse*>(this), _TranslateMouseButtonFromQt(mouseEvent->button()));
            break;

        case QEvent::Wheel:
            scrollWheel.x = wheelEvent->angleDelta().x();
            scrollWheel.y = wheelEvent->angleDelta().y();
            _mouseScrollWheelChangedThisFrame = true;
            break;
        default:
            break;

        }

        return false;

    }

};

static EventFilter*  _inputFilter = nullptr;

extern "C" int gyInputInit(const GYInputInitParams*const params) {
    int success = 1;
    _inputFilter = new EventFilter;
    _initParams = *params;
    ((QObject*)_initParams.viewWidget)->installEventFilter(_inputFilter);
    success &= _sdlControllerInit(params);
    return 1;
}

extern "C" int gyInputUninit(void) {
    int success = 1;
     ((QObject*)_initParams.viewWidget)->removeEventFilter(_inputFilter);
    QApplication::instance()->removeEventFilter(_inputFilter);
    success &= _sdlControllerUninit();
    delete _inputFilter;
    return 1;
}

extern "C" int gyKeyboardPoll(const unsigned slot, GYKeyboard**const keybd) {
    if(slot > 0) {
        *keybd = NULL;
        return 0;
    }

    *keybd = static_cast<GYKeyboard*>(_inputFilter);
    (*keybd)->changed = 0;
    for(unsigned i = 0; i < GY_KEY_COUNT; ++i) {
        if(!_keysChangedThisFrame[i]) {
            unsigned wasDown = ((*keybd)->keys[i] & GY_BUT_FLAGS_IS_PRESSED);
            unsigned key = (*keybd)->keys[i];
            key <<= 1;
            key |= wasDown;
            key &= 0x03;
            (*keybd)->keys[i] = (GY_BUTTON_STATE)key;
          //  (*keybd)->changed = 1;
        } else {
            _keysChangedThisFrame[i] = false;
            (*keybd)->changed = 1;
        }
    }

    return 1;
}

extern "C" int gyMousePoll(const unsigned slot, GYMouse**const mouse) {
    if(slot > 0) {
        *mouse = NULL;
        return 0;
    }

    *mouse = (_inputFilter);
    (*mouse)->buttonsChanged = 0;
    for(unsigned i = 0; i < GY_MOUSE_BUT_COUNT; ++i) {
        if(!_mouseButtonsChangedThisFrame[i]) {
            unsigned wasDown = ((*mouse)->buttons[i] & GY_BUT_FLAGS_IS_PRESSED);
            unsigned button = (*mouse)->buttons[i];
            button <<= 1;
            button |= wasDown;
            button &= 0x03;
            (*mouse)->buttons[i] = (GY_BUTTON_STATE)button;
            (*mouse)->buttonsChanged = 1;
        } else {
            _mouseButtonsChangedThisFrame[i] = false;
            (*mouse)->buttonsChanged = 1;
        }
    }

    if(_mousePosChangedThisFrame) {
        (*mouse)->screenPosChanged = 1;
        _mousePosChangedThisFrame = false;
    } else (*mouse)->screenPosChanged = 0;

    if(_mouseScrollWheelChangedThisFrame) {
        (*mouse)->scrollWheelChanged = 1;
        _mouseScrollWheelChangedThisFrame = false;
    } else (*mouse)->scrollWheelChanged = 0;

    return 1;
}

extern "C" void gyInputDeviceDump(void) {
    _gyLog(GY_DEBUG_VERBOSE, "LibGyro Device Dump");

    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "Keyboard: ATTACHED [Qt]");
    _gyLog(GY_DEBUG_VERBOSE, "Mouse: ATTACHED [Qt]");

    _sdlControllerDump();
    _gyPop(1);


}
