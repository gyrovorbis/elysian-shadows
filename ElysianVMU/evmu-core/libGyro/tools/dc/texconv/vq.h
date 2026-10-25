#ifndef VQ_H
#define VQ_H

#include <QtGui/QImage>
#include <cmath>

#include "color.h"


// This class represents a single 2x2 quad of 16BPP colors.
class VQQuad16
{
public:

    union
    {
        quint64 cmp;
        quint16 col[4];
    };

    bool operator < ( const VQQuad16& other ) const
    {
        return cmp < other.cmp;
    }
};


// An image consisting of 16BPP quads.
// NOTE: These images are always square.
class QuadImage16
{
public:

    // Construct a 16BPP quad image from a QImage
    QuadImage16( const QImage& image, const ColorConverter* converter );

    ~QuadImage16( void ) { delete m_pQuads; }

    int GetNumberOfQuads( void ) const { return m_nQuads; }

    const VQQuad16& GetQuad( int index ) const { return m_pQuads[ index ]; }

    // Returns the dimension of this image in pixels. This is the
    // same for width and height since the image is square.
    int GetDimension( void ) const { return m_dimension; }

private:

    int         m_dimension;
    int		m_nQuads;
    VQQuad16*	m_pQuads;
};


// 2x2 pixel quad where the colors are stored as floats.
class VQQuad
{
public:

    VQQuad( void )
    {
        Clear();
    }

    VQQuad( quint32 c0, quint32 c1, quint32 c2, quint32 c3 )
    {
        col[0] = c0;
        col[1] = c1;
        col[2] = c2;
        col[3] = c3;
    }

    void Clear( void )
    {
        for ( int i=0; i<4; i++ )
            col[i].Clear();
    }

    void operator = ( const VQQuad& other )
    {
        for ( int i=0; i<4; i++ )
            col[i] = other.col[i];
    }

    void operator += ( const VQQuad& other )
    {
        for ( int i=0; i<4; i++ )
            col[i] += other.col[i];
    }

    VQQuad operator - ( const VQQuad& other ) const
    {
        VQQuad q;
        for ( int i=0; i<4; i++ )
            q.col[i] = col[i] - other.col[i];
        return q;
    }

    void operator /= ( const double scalar )
    {
        if ( scalar < 1.0f )
            Clear();
        else
            for ( int i=0; i<4; i++ )
                col[i] /= scalar;
    }

    double Length( void )
    {
        double total = 0.0;

        for ( int i=0; i<4; i++ )
        {
            total += ( col[i].a * col[i].a );
            total += ( col[i].r * col[i].r );
            total += ( col[i].g * col[i].g );
            total += ( col[i].b * col[i].b );
        }

        return sqrt( total );
    }

    // Delta_E gives a single number that acts as a measurement of how closely
    // two colors relate to each other. The smaller, the better.
    static double Delta_E( const VQQuad& a, const VQQuad& b )
    {
        return (a - b).Length();
    }

    FloatColor GetColor( int index ) const	{ return col[ index ];      }
    FloatColor GetColor( int x, int y ) const	{ return col[ y * 2 + x ];  }

private:

    FloatColor col[4];
};


// An image consisting of float quads.
// NOTE: These images are always square.
class QuadImage
{
public:

    // Construct a float quad image from a 16BPP quad image. This improves the compression
    // quality as there are fewer input colors to begin with.
    QuadImage( const QuadImage16* qi, const ColorConverter* converter );

    ~QuadImage( void ) { delete[] m_pQuads; }

    int GetNumberOfQuads( void ) const { return m_nQuads; }

    const VQQuad& GetQuad( int index ) const { return m_pQuads[ index ]; }

    // Returns the dimension of this image in pixels. This is the
    // same for width and height since the image is square.
    int GetDimension( void ) const { return m_dimension; }

private:

    int     m_dimension;
    int     m_nQuads;
    VQQuad* m_pQuads;
};


// This is where all the magic happens.
class VQCodebook
{
public:

    // Make sure there's always at least one code in the codebook.
    // Start of with a single black quad.
    VQCodebook( void ) { m_vCodes.push_back( VQCode() ); }

    void ResetCodes( void )
    {
        for ( QVector<VQCode>::iterator it = m_vCodes.begin(); it != m_vCodes.end(); ++it )
            it->Reset();
    }

    // Returns the index of the code that resembles the given quad the most.
    int Find( const VQQuad& quad ) const;

    // Place all quads of the given image in the codebook.
    void PlaceQuads( const QuadImage* pQuadImage );

    // Clean the codebook by normalizing all codes.
    void Clean( void );

    // Split the codebook once. Each split makes the codebook double
    // in size. Split 8 times to get 256 codes as used by dreamcast.
    void Split( void );

    // Self-explanatory
    int GetNumberOfCodes( void ) const { return (int) m_vCodes.size(); }
    const VQQuad& GetCodeValue( int index ) const { return m_vCodes[index].value; }

private:

    class VQCode
    {
    public:

        VQCode( void )
        {
            Reset();
            value.Clear();
        }

        // Resets the variables used for compression
        void Reset( void )
        {
            pos_sum.Clear();
            pos_count = 0;
            max_dist_vec.Clear();
            max_dist = 0.0;
        }

        // Sum of quads using this code
        VQQuad pos_sum;
        int pos_count;

        // Quad that is furthest away from this code
        double max_dist;
        VQQuad max_dist_vec;

        // Actual quad value of this codebook entry
        VQQuad value;
    };

    QVector<VQCode> m_vCodes;
};

#endif // VQ_H
