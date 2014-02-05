#ifndef COLOR_H
#define COLOR_H

#include <QtGlobal>

// Get components of a packed color
inline quint8 GetA( quint32 argb ) { return (quint8) ( ( argb >> 24 ) & 0xFF ); }
inline quint8 GetR( quint32 argb ) { return (quint8) ( ( argb >> 16 ) & 0xFF ); }
inline quint8 GetG( quint32 argb ) { return (quint8) ( ( argb >> 8 ) & 0xFF );  }
inline quint8 GetB( quint32 argb ) { return (quint8) ( argb & 0xFF );           }
inline float GetfA( quint32 argb ) { return (float) GetA( argb ) * ( 1.0f / 255.0f ); }
inline float GetfR( quint32 argb ) { return (float) GetR( argb ) * ( 1.0f / 255.0f ); }
inline float GetfG( quint32 argb ) { return (float) GetG( argb ) * ( 1.0f / 255.0f ); }
inline float GetfB( quint32 argb ) { return (float) GetB( argb ) * ( 1.0f / 255.0f ); }

// Yeah, I got tired of writing switch statements in every damn function
// to accomodate for all color formats. Hopefully, this solution will
// fix all that.
class ColorConverter
{
public:

    virtual quint16 To16BPP( quint32 src ) const = 0;
    virtual quint32 To32BPP( quint16 src ) const = 0;
};

// For converting to and from RGB565
class RGB565Converter : public ColorConverter
{
public:

    quint16 To16BPP( quint32 src ) const
    {
        quint16 r = (quint16) ( ( GetR( src ) >> 3 ) & 0x1F );
        quint16 g = (quint16) ( ( GetG( src ) >> 2 ) & 0x3F );
        quint16 b = (quint16) ( ( GetB( src ) >> 3 ) & 0x1F );
        return ( r << 11 ) | ( g << 5 ) | b;
    }

    quint32 To32BPP( quint16 src ) const
    {
        quint32 a = 0xFF;
        quint32 r = ( ( src >> 11 ) & 0x1F ) << 3;
        quint32 g = ( ( src >> 5  ) & 0x3F ) << 2;
        quint32 b = ( ( src >> 0  ) & 0x1F ) << 3;
        return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;
    }
};

// For converting to and from ARGB1555
class ARGB1555Converter : public ColorConverter
{
public:

    quint16 To16BPP( quint32 src ) const
    {
        quint16 a = ( GetA( src ) < 0x7F ) ? 0 : 1;
        quint16 r = (quint16) ( ( GetR( src ) >> 3 ) & 0x1F );
        quint16 g = (quint16) ( ( GetG( src ) >> 3 ) & 0x1F );
        quint16 b = (quint16) ( ( GetB( src ) >> 3 ) & 0x1F );
        return ( a << 15 ) | ( r << 10 ) | ( g << 5 ) | b;
    }

    quint32 To32BPP( quint16 src ) const
    {
        quint32 a = ( ( src >> 15 ) == 1 ) ? 0xFF : 0;
        quint32 r = ( ( src >> 10 ) & 0x1F ) << 3;
        quint32 g = ( ( src >> 5  ) & 0x1F ) << 3;
        quint32 b = ( ( src >> 0  ) & 0x1F ) << 3;
        return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;
    }
};

// For converting to and from ARGB4444
class ARGB4444Converter : public ColorConverter
{
public:

    quint16 To16BPP( quint32 src ) const
    {
        quint16 a = (quint16) ( ( GetA( src ) >> 4 ) & 0xF );
        quint16 r = (quint16) ( ( GetR( src ) >> 4 ) & 0xF );
        quint16 g = (quint16) ( ( GetG( src ) >> 4 ) & 0xF );
        quint16 b = (quint16) ( ( GetB( src ) >> 4 ) & 0xF );
        return ( a << 12 ) | ( r << 8 ) | ( g << 4 ) | b;
    }

    quint32 To32BPP( quint16 src ) const
    {
        quint32 a = ( ( src >> 12 ) & 0xF ) << 4;
        quint32 r = ( ( src >> 8  ) & 0xF ) << 4;
        quint32 g = ( ( src >> 4  ) & 0xF ) << 4;
        quint32 b = ( ( src >> 0  ) & 0xF ) << 4;
        return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;
    }
};

class FloatColor
{
public:

    float a, r, g, b;

    FloatColor( void ) : a(0.0f), r(0.0f), g(0.0f), b(0.0f) {}
    FloatColor( float _a, float _r, float _g, float _b ) : a(_a), r(_r), g(_g), b(_b) {}
    FloatColor( const FloatColor& other ) : a(other.a), r(other.r), g(other.g), b(other.b) {}
    FloatColor( quint32 argb ) { (*this) = argb; }

    void Clear( void )
    {
        a = r = g = b = 0.0f;
    }

    void operator = ( quint32 argb )
    {
        a = GetfA( argb );
        r = GetfR( argb );
        g = GetfG( argb );
        b = GetfB( argb );
    }

    FloatColor operator - ( const FloatColor& other ) const
    {
        return FloatColor( a - other.a, r - other.r, g - other.g, b - other.b );
    }

    void operator += ( const FloatColor& other )
    {
        a += other.a;
        r += other.r;
        g += other.g;
        b += other.b;
    }

    void operator *= ( const float scalar )
    {
        a *= scalar;
        r *= scalar;
        g *= scalar;
        b *= scalar;
    }

    void operator /= ( const float scalar )
    {
        (*this) *= (1.0f / scalar);
    }

    inline quint32 GetARGB8888( void )
    {
        quint8 aa = (quint8) (a * 255.0f);
        quint8 rr = (quint8) (r * 255.0f);
        quint8 gg = (quint8) (g * 255.0f);
        quint8 bb = (quint8) (b * 255.0f);
        return ( aa << 24 ) | ( rr << 16 ) | ( gg << 8 ) | bb;
    }

    inline quint16 Get16BPP( const ColorConverter* converter )
    {
        return converter->To16BPP( GetARGB8888() );
    }
};

#endif // COLOR_H
