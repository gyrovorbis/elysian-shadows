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

    float LabelWidget::getPosition(AXIS axis) const {
        return _position[axis];
    }

    void LabelWidget::setSize(AXIS axis, float size) {
        //Uhm, fuck you.
    }

    float LabelWidget::getSize(AXIS axis) const {
        if(axis == Y) {
            return _font->getCharacterHeight();
        } else {
            return _width;
        }
    }

    void LabelWidget::render(void) {
        if(!_width) return;

        gyMatPush();
        gyMatLoadIdentity();

        _font->_transform.setPos(_position[X], _position[Y]);
        _font->_transform.setScale(1.0f, 1.0f);
        _font->setColor(_color);
        _font->drawString(Font::C_ALIGN, _text);

        gyMatPop();

    }


}
