/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LABEL_WIDGET_HPP
#define ELYSIAN_LABEL_WIDGET_HPP

#include <ESGamma/gui/elysian_widget.hpp>
#include <AssetIO/crtp/elysian_color.hpp>

#define ELYSIAN_LABEL_WIDGET_TEXT_SIZE  100

namespace elysian {

    class Font;

    class LabelWidget: public Widget {
    public:
        enum ALIGNMENT {
            LEFT,
            CENTER,
            RIGHT
        };

    private:
        char            _text[ELYSIAN_LABEL_WIDGET_TEXT_SIZE]   = { 0 };
        Vector3         _position;
        Color4          _color                                  = Color4::white;
        ALIGNMENT       _alignment                              = CENTER;
        int             _width                                  = 0;
        int             _strlen                                 = 0;
        Font*           _font                                   = _defaultFont;
        //bool          _markupEnabled                          = true;
        static Font*    _defaultFont;


    public:
                        LabelWidget(const char* str=nullptr, Color4 color=Color4::white, ALIGNMENT align=LEFT);

        const char*     getText(void) const;
        void            setText(const char* str);

        Color4          getColor(void) const;
        void            setColor(Color4 color);
        unsigned char   getAlpha(void) const;
        void            setAlpha(unsigned char alpha);


        ALIGNMENT       getAlignment(void) const;
        void            setAlignment(ALIGNMENT align);

        Font*           getFont(void) const;
        void            setFont(Font* font);

        int             getStrlen(void) const;

        //Overridden from Widget
        virtual float   getPosition(AXIS axis) const override;
        virtual void    setPosition(AXIS axis, float pos) override;
        virtual float   getPositionZ(void) const override;
        virtual void    setPositionZ(float pos);
        virtual float   getSize(AXIS axis) const override;
        virtual void    setSize(AXIS axis, float size) override;
        virtual void    render(void) const override;

        //static
        static void     setDefaultFont(Font* font);
        static Font*    getDefaultFont(void);

    };







    //=========== INLINEZ =============
    inline LabelWidget::LabelWidget(const char* str, Color4 color, ALIGNMENT align):
        _color(color),
        _alignment(align)
    {
        setSizePolicy(FIXED, FIXED);
        setText(str);
    }

    inline const char* LabelWidget::getText(void) const { return _text; }
    inline int LabelWidget::getStrlen(void) const { return _strlen; }
    inline Color4 LabelWidget::getColor(void) const { return _color; }
    inline void LabelWidget::setColor(Color4 color) { _color = color; }
    inline auto LabelWidget::getAlignment(void) const -> ALIGNMENT { return _alignment; }
    inline void LabelWidget::setAlignment(ALIGNMENT align) { _alignment = align; }
    inline Font* LabelWidget::getFont(void) const { return _font; }
    inline void LabelWidget::setDefaultFont(Font* font) { _defaultFont = font; }
    inline Font* LabelWidget::getDefaultFont(void) { return _defaultFont; }
    inline unsigned char LabelWidget::getAlpha(void) const { return _color.a; }
    inline void LabelWidget::setAlpha(unsigned char alpha) { _color.a = alpha; }



}

#endif // ELYSIAN_LABEL_WIDGET_HPP
