#include <cstring>
#include "gui/elysian_label_widget.hpp"
#include "gui/elysian_font.hpp"

namespace elysian {

    Font* LabelWidget::_defaultFont = nullptr;

    void LabelWidget::setText(const char* str) {
        if(!str) {
            _text[0] = '\0';
            _strlen = 0;
            _width  = 0;
        } else {
            strncpy(_text, str, ELYSIAN_LABEL_WIDGET_TEXT_SIZE);
            _strlen = strlen(str);
            if(_font) _width = _font->getStringPixelWidth(str);
        }
    }

    void LabelWidget::setFont(Font* font) {
        _font = font;
        _width = font? font->getStringPixelWidth(_text) : 0;
    }

    void LabelWidget::setPosition(AXIS axis, float pos) {
        _position[axis] = pos;
    }

    void LabelWidget::setPositionZ(float pos) {
        _position.z = pos;
    }

    float LabelWidget::getPositionZ(void) const {
        return _position.z;
    }

    float LabelWidget::getPosition(AXIS axis) const {
        return _position[axis];
    }

    void LabelWidget::setSize(AXIS axis, float size) {
        //Uhm, fuck you.
    }

    float LabelWidget::getSize(AXIS axis) const {
        if(axis == Y) {
            //return _font->getCharacterHeight();
            return 28;
        } else {
            return _width;
        }
    }

    void LabelWidget::render(void) const {
        if(_width) {
            gyMatPush();
            gyMatLoadIdentity();

            _font->_transform.setPos(_position);
            _font->_transform.setScale(1.0f, 1.0f);
            _font->_transform.setSizeY(28.0f);
            _font->setColor(_color);

            switch(_alignment) {
            case CENTER:
            default:
                _font->drawString(Font::C_ALIGN, _text);
                break;
#if 0
            case LEFT:
                _font->getTransform().translate(-0.5f*_width, 0.0f, 0.0f);
                _font->drawString(Font::L_ALIGN, _text);
                break;
            case RIGHT:
                _font->getTransform().translate(0.5f*_width, 0.0f, 0.0f);
                _font->drawString(Font::R_ALIGN, _text);
                break;
#endif

            }

            gyMatPop();

        }
        Widget::render();
    }


}
