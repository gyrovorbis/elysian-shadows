#ifndef TWIDDLER_H
#define TWIDDLER_H

class Twiddler
{
public:

    Twiddler( int w, int h );

    ~Twiddler( void );

    int	GetIndex( int x, int y ) { return m_index[ y * m_width + x ]; }
    int GetIndex( int index )	 { return m_index[ index ]; }

private:

    int Twiddle(int* output, int stride, int x, int y, int blocksize, int seq);

    int		m_width;
    int		m_height;
    int*	m_index;
};

#endif // TWIDDLER_H
