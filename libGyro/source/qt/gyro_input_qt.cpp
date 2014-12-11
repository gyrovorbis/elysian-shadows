#include <gyro_input_api.h>
#include <QApplication>
#include <QKeyEvent>

static unsigned _keyDownFrames[GY_KEY_LAST] = { 0 };

static inline void _updateKeyTaps(GYInput& input) {

    for(unsigned k = 0; k < GY_KEY_LAST; ++k) {

        if(input.keyDown[k]) {
            if(_keyDownFrames[k] >= 1) {
                input.keyTapped[k] = 0;
            } else {
                ++_keyDownFrames[k];
                input.keyTapped[k] = 1;
            }
        } else {
            _keyDownFrames[k] = 0;
            input.keyTapped[k] = 0;
        }
    }

}

static inline GY_X86_KEY _TranslateFromQt(const int keyCode) {
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

static inline void _setKeyDown(GYInput *const input, const GY_X86_KEY key) {
    input->keyDown[key] = 1;
}

static inline void _setKeyUp(GYInput *const input, const GY_X86_KEY key) {
    input->keyDown[key] = 0;
}


class EventFilter: public QObject, public GYInput {

public:

    EventFilter(void) {
        memset((GYInput*)this, 0, sizeof(GYInput));
    }
    virtual ~EventFilter(void) = default;

    virtual bool eventFilter(QObject* object, QEvent* event) override {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);


        switch(event->type()) {
        case QEvent::KeyPress:
            _setKeyDown(this, _TranslateFromQt(keyEvent->key()));

            return false;

        case QEvent::KeyRelease:
            _setKeyUp(this, _TranslateFromQt(keyEvent->key()));

            return false;


        default:
            return false;

        }

    }

};

static EventFilter* _inputFilter;

int gyInputInit(void) {
    _inputFilter = new EventFilter;
    QApplication::instance()->installEventFilter(_inputFilter);
    return 1;
}

int gyInputUninit(void) {
    QApplication::instance()->removeEventFilter(_inputFilter);
    delete _inputFilter;
    return 1;
}

void gyInputPoll(GYInput*const input) {
    _updateKeyTaps(*_inputFilter);
    *input = *(GYInput*)_inputFilter;

}
