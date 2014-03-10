#include "Twiddler.h"

Twiddler::Twiddler( int w, int h )
{
    m_width = w;
    m_height = h;
    m_index = new int[m_width * m_height];

    int index = 0;

    if (m_width < m_height)
        for (int y = 0; y < (int)m_height; y += m_width)
            index += Twiddle(m_index, m_width, 0, y, m_width, index);
    else
        for (int x = 0; x < (int)m_width; x += m_height)
            index += Twiddle(m_index, m_width, x, 0, m_height, index);
}

Twiddler::~Twiddler( void )
{
    delete[] m_index;
}

int Twiddler::Twiddle(int* output, int stride, int x, int y, int blocksize, int seq)
{
    int before;

    before = seq;
    switch (blocksize)
    {
    case 1:
        // cant divide anymore
        output[seq++] = y * stride + x;
        break;

    default:
        blocksize = blocksize >> 1;
        seq += Twiddle(output, stride, x, y, blocksize, seq);
        seq += Twiddle(output, stride, x, y + blocksize, blocksize, seq);
        seq += Twiddle(output, stride, x + blocksize, y, blocksize, seq);
        seq += Twiddle(output, stride, x + blocksize, y + blocksize, blocksize, seq);
        break;
    }

    return (seq - before);
}
