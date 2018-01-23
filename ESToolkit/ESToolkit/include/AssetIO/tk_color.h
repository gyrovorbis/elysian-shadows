#ifndef TK_COLOR_H
#define TK_COLOR_H

#include <primitive/elysian_color_base.hpp>
#include <QColor>

class TkColor4: public elysian::Color4Base {

public:

                    TkColor4(void);
                    TkColor4(GYColor4* color);
                    TkColor4(const GYColor4& other);
                    TkColor4(const Color4& other);
                    TkColor4(GYColor4&& other);
                    TkColor4(const QColor& color);
                    TkColor4(unsigned char c);
                    TkColor4(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    operator        const QColor(void) const;
    TkColor4&       operator=(const QColor& rhs);

    static void    initializeDelegates(void);

};


//===== INLINEZ ======

inline TkColor4::TkColor4(void): elysian::Color4Base() {}
inline TkColor4::TkColor4(unsigned char c): elysian::Color4Base(c) {}
inline TkColor4::TkColor4(GYColor4* color): elysian::Color4Base(color) {}
inline TkColor4::TkColor4(const GYColor4& other): elysian::Color4Base(other) {}
inline TkColor4::TkColor4(GYColor4 &&other): elysian::Color4Base(std::move(other)) {}
inline TkColor4::TkColor4(const TkColor4& color) : elysian::Color4Base(color) {
    *this = color;
}
inline TkColor4::TkColor4(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a):
    elysian::Color4Base(r, g, b, a)
{}
inline TkColor4::TkColor4(const QColor &color):
    elysian::Color4Base(color.red(), color.blue(), color.green(), color.alpha())
{}

inline TkColor4& TkColor4::operator=(const QColor& rhs) {
    setComponents(rhs.red(), rhs.green(), rhs.blue(), rhs.alpha());
    return *this;
}

inline TkColor4::operator const QColor(void) const {
    return QColor(getR(), getG(), getB(), getA());
}



#endif // TK_COLOR_H
