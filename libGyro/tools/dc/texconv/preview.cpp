#include "preview.h"

// Generate a preview image for uncompressed images.
// In case there are mipmaps, the image will be 1.5 times as wide as the original
// and the mipmaps will be place to the right of the original image, top to bottom.
void Write16BPPPreview( const std::string& filename, const QVector<QImage>& mipmaps, const ColorConverter* converter )
{
    const int w = mipmaps[0].width();
    const int h = mipmaps[0].height();
    int xoffset = 0;
    int yoffset = 0;

    QImage output( ( mipmaps.size() > 1 ) ? ( w + w / 2) : w, h, QImage::Format_ARGB32 );

    for ( QVector<QImage>::const_iterator it = mipmaps.begin(); it != mipmaps.end(); ++it )
    {
        for ( int y=0; y<it->height(); y++ )
        {
            for ( int x=0; x<it->width(); x++ )
            {
                quint32 pixel = it->pixel( x, y );

                if ( converter != NULL )
                    pixel = converter->To32BPP( converter->To16BPP( pixel ) );

                output.setPixel( xoffset + x, yoffset + y, pixel );
            }
        }

        if ( xoffset == 0 )
            xoffset = w;
        else
            yoffset += it->height();
    }

    output.save( QString::fromStdString( filename ) );
}

// Generate a preview image for compressed images.
// In case there are mipmaps, the image will be 1.5 times as wide as the original
// and the mipmaps will be place to the right of the original image, top to bottom.
void WriteVqPreview( const std::string& filename, const QVector<QuadImage*>& mipmaps, const VQCodebook& codebook, const ColorConverter* converter )
{
    const int w = mipmaps[0]->GetDimension();
    const int h = mipmaps[0]->GetDimension();
    int xoffset = 0;
    int yoffset = 0;

    QImage output( ( mipmaps.size() > 1 ) ? ( w + w / 2) : w, h, QImage::Format_ARGB32 );

    for ( QVector<QuadImage*>::const_iterator it = mipmaps.begin(); it != mipmaps.end(); ++it )
    {
        for ( int i=0; i<(*it)->GetNumberOfQuads(); i++ )
        {
            int quadsAcross = ( (*it)->GetDimension() / 2 );
            int x = ( i % quadsAcross ) * 2;
            int y = ( i / quadsAcross ) * 2;
            const VQQuad& quad = codebook.GetCodeValue( codebook.Find( (*it)->GetQuad(i) ) );

            quint32 pixel[4];
            pixel[0] = converter->To32BPP( converter->To16BPP( quad.GetColor(0).GetARGB8888() ) );
            pixel[1] = converter->To32BPP( converter->To16BPP( quad.GetColor(1).GetARGB8888() ) );
            pixel[2] = converter->To32BPP( converter->To16BPP( quad.GetColor(2).GetARGB8888() ) );
            pixel[3] = converter->To32BPP( converter->To16BPP( quad.GetColor(3).GetARGB8888() ) );

            output.setPixel( xoffset + x + 0, yoffset + y + 0, pixel[0] );
            output.setPixel( xoffset + x + 1, yoffset + y + 0, pixel[1] );
            output.setPixel( xoffset + x + 0, yoffset + y + 1, pixel[2] );
            output.setPixel( xoffset + x + 1, yoffset + y + 1, pixel[3] );
        }

        if ( xoffset == 0 )
            xoffset = w;
        else
            yoffset += (*it)->GetDimension();

    }

    output.save( QString::fromStdString( filename ) );
}

// Generate a preview image for lossless compressed images.
// In case there are mipmaps, the image will be 1.5 times as wide as the original
// and the mipmaps will be place to the right of the original image, top to bottom.
void WriteLosslessVqPreview( const std::string& filename, const QVector<QuadImage16*>& mipmaps, const ColorConverter* converter )
{
    const int w = mipmaps[0]->GetDimension();
    const int h = mipmaps[0]->GetDimension();
    int xoffset = 0;
    int yoffset = 0;

    QImage output( ( mipmaps.size() > 1 ) ? ( w + w / 2) : w, h, QImage::Format_ARGB32 );

    for ( QVector<QuadImage16*>::const_iterator it = mipmaps.begin(); it != mipmaps.end(); ++it )
    {
        for ( int i=0; i<(*it)->GetNumberOfQuads(); i++ )
        {
            int quadsAcross = ( (*it)->GetDimension() / 2 );
            int x = ( i % quadsAcross ) * 2;
            int y = ( i / quadsAcross ) * 2;
            const VQQuad16& quad = (*it)->GetQuad(i);

            quint32 pixel[4];
            pixel[0] = converter->To32BPP( quad.col[0] );
            pixel[1] = converter->To32BPP( quad.col[1] );
            pixel[2] = converter->To32BPP( quad.col[2] );
            pixel[3] = converter->To32BPP( quad.col[3] );

            output.setPixel( xoffset + x + 0, yoffset + y + 0, pixel[0] );
            output.setPixel( xoffset + x + 1, yoffset + y + 0, pixel[1] );
            output.setPixel( xoffset + x + 0, yoffset + y + 1, pixel[2] );
            output.setPixel( xoffset + x + 1, yoffset + y + 1, pixel[3] );
        }

        if ( xoffset == 0 )
            xoffset = w;
        else
            yoffset += (*it)->GetDimension();

    }

    output.save( QString::fromStdString( filename ) );
}
