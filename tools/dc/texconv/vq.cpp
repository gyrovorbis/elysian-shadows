#include "vq.h"

QuadImage16::QuadImage16( const QImage& image, const ColorConverter* converter )
{
    m_dimension = image.width();
    int quadsAcross = m_dimension / 2;
    m_nQuads = quadsAcross * quadsAcross;
    m_pQuads = new VQQuad16[ m_nQuads ];

    for ( int i=0; i<m_nQuads; i++ )
    {
        int x = ( i % quadsAcross ) * 2;
        int y = ( i / quadsAcross ) * 2;

        m_pQuads[i].col[0] = converter->To16BPP( image.pixel( x + 0, y + 0 ) );
        m_pQuads[i].col[1] = converter->To16BPP( image.pixel( x + 1, y + 0 ) );
        m_pQuads[i].col[2] = converter->To16BPP( image.pixel( x + 0, y + 1 ) );
        m_pQuads[i].col[3] = converter->To16BPP( image.pixel( x + 1, y + 1 ) );
    }
}

QuadImage::QuadImage( const QuadImage16* qi, const ColorConverter* converter )
{
    m_dimension = qi->GetDimension();
    m_nQuads = qi->GetNumberOfQuads();
    m_pQuads = new VQQuad[ m_nQuads ];

    for ( int i=0; i<m_nQuads; i++ )
    {
        const VQQuad16& q = qi->GetQuad( i );

        m_pQuads[i] = VQQuad( converter->To32BPP( q.col[0] ),
                              converter->To32BPP( q.col[1] ),
                              converter->To32BPP( q.col[2] ),
                              converter->To32BPP( q.col[3] ) );
    }
}

int VQCodebook::Find( const VQQuad& quad ) const
{
    if ( m_vCodes.empty() )
        return 0;

    int closestEntry = 0;
    double closestDistance = VQQuad::Delta_E( m_vCodes[0].value, quad );

    for ( int i=1; i<(int)m_vCodes.size(); i++ )
    {
        double distance = VQQuad::Delta_E( m_vCodes[i].value, quad );

        if ( distance < closestDistance )
        {
            // Closest so far
            closestEntry = i;
            closestDistance = distance;

            if ( closestDistance < 0.0001 )
            {
                // Close enough
                return closestEntry;
            }
        }

    }

    return closestEntry;
}

void VQCodebook::PlaceQuads( const QuadImage* pQuadImage )
{
    for ( int i=0; i<pQuadImage->GetNumberOfQuads(); i++ )
    {
        const VQQuad& quad = pQuadImage->GetQuad(i);

        // Find closest code
        int idx = Find( quad );

        // Update the average
        m_vCodes[idx].pos_sum += quad;
        m_vCodes[idx].pos_count++;

        // Update the max distance if needed
        double distance = VQQuad::Delta_E( m_vCodes[idx].value, quad );
        if ( distance > m_vCodes[idx].max_dist )
        {
            m_vCodes[idx].max_dist = distance;
            m_vCodes[idx].max_dist_vec = quad;
        }
    }
}

void VQCodebook::Clean( void )
{
    for ( QVector<VQCode>::iterator it = m_vCodes.begin(); it != m_vCodes.end(); ++it )
    {
        if ( it->pos_count > 0 )
        {
            // Normalize the sum and update the value
            it->pos_sum /= (float) it->pos_count;
            it->value = it->pos_sum;
        }
    }
}

void VQCodebook::Split( void )
{
    // The size will change and we don't wanna iterate
    // over the new codes.
    const int count = m_vCodes.size();

    for ( int i=0; i<count; i++ )
    {
        VQCode& code = m_vCodes[i];

        if ( code.pos_count > 1 )
        {
            // I'm not sure sure about the details here. I didn't come up with
            // this algorithm. Basically split this quad into two based on
            // the max_dist vector.
            VQQuad diff = code.max_dist_vec - code.value;
            double diffLen = diff.Length() * 256.0;
            diff /= diffLen;
            VQQuad newQuad = code.value;
            newQuad += diff;
            code.value = code.value - diff;

            // Add the new code
            VQCode code;
            code.Reset();
            code.value = newQuad;
            m_vCodes.push_back( code );
        }
    }
}
