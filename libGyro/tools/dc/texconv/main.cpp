///////////////////////////////////////////////////////////
//       __                                              //
//     _/  |_  ____ ___  ___ ____  ____   _______  __    //
//     \   __\/ __ \\  \/  // ___\/  _ \ /    \  \/ /    //
//      |  | \  ___/ >    <\  \__(  <_> )   |  \   /     //
//      |__|  \___  >__/\_ \\___  >____/|___|  /\_/      //
//                \/      \/    \/           \/          //
//                                                       //
///////////////////////////////////////////////////////////
// Converts images to the dreamcast texture format.      //
// Supports compressed and uncompressed 16BPP textures   //
// as well as paletted ones.                             //
//                                                       //
// NOTE: ALL textures output by this tool are twiddled.  //
//                                                       //
// By Anton Norgren, 2011.                               //
///////////////////////////////////////////////////////////

#include <QtGui/QImage>
#include <QTime>
#include <QVector>
#include <QHash>
#include <QSet>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>

#include "color.h"
#include "twiddler.h"
#include "preview.h"
#include "vq.h"

// The supported texture formats.
#define FORMAT_RGB565		0
#define FORMAT_ARGB1555		1
#define FORMAT_ARGB4444		2
#define FORMAT_PAL4BPP		3
#define FORMAT_PAL8BPP		4

// Texture flags. These are "or"ed together to form the
// flags variable in the header.
#define FLAG_TWIDDLED		1
#define FLAG_COMPRESSED		2
#define FLAG_MIPMAPPED		4

// Mipmapped uncompressed textures all have a small offset
// before the actual texture data starts.
#define PAL4BPP_MM_OFFSET	1
#define PAL8BPP_MM_OFFSET	3
#define COL16BPP_MM_OFFSET	6

// Texture header, 24 bytes
struct TextureHeader
{
    char    magic[4];
    qint32  width;
    qint32  height;
    qint32  format;
    qint32  flags;
    qint32  size;
};

// Palette header, 8 bytes
struct PaletteHeader
{
    char    magic[4];
    qint32  num_colors;
};

// Given the dimensions of an image, this function returns how many pixels there
// are in total, including mipmaps. Assumes width == height if mipmapped == true.
int GetPixelCount( int width, int height, bool mipmapped )
{
    if ( mipmapped )
    {
        if ( width == 1 )
            return 1;
        else
            return width * height + GetPixelCount( width / 2, height / 2, mipmapped );
    }

    return width * height;
}

// Calculates the size, in bytes, of an image. This does not include texture headers
// or palettes.
static int CalculateSize( int format, int w, int h, bool bMipmapped, bool bCompressed )
{
    int size = 0;
    int pixels = GetPixelCount( w, h, bMipmapped );

    if ( bMipmapped )
    {
        if ( bCompressed )
        {
            size += 2048; // Codebook
            size += 1;	// The 1x1 pixel
            size += ( pixels - 1 ) / 4;	// Rest of the pixels
        }
        else
        {
            switch ( format )
            {
            case FORMAT_RGB565:
            case FORMAT_ARGB1555:
            case FORMAT_ARGB4444:
                size += COL16BPP_MM_OFFSET;	// Offset for mipmaps
                size += pixels * 2;		// All pixels
                break;

            case FORMAT_PAL4BPP:
                size += PAL4BPP_MM_OFFSET;	// Offset for mipmaps
                size += 1;			// The 1x1 half-pixel
                size += ( pixels - 1 ) / 2;	// Rest of the pixels
                break;

            case FORMAT_PAL8BPP:
                size += PAL8BPP_MM_OFFSET;	// Offset for mipmaps
                size += pixels;			// All pixels
                break;
            }
        }
    }
    else
    {
        if ( bCompressed )
        {
            size += 2048; // Codebook
            size += ( pixels / 4 ); // All pixels
        }
        else
        {
            switch ( format )
            {
            case FORMAT_RGB565:
            case FORMAT_ARGB1555:
            case FORMAT_ARGB4444:
                size += pixels * 2;
                break;

            case FORMAT_PAL4BPP:
                size += pixels / 2;
                break;

            case FORMAT_PAL8BPP:
                size += pixels;
                break;
            }
        }
    }

    return size;
}

// Returns true if the texture size is valid on dreamcast.
bool IsValidSize( int width, int height )
{
    return ( width  == 8 || width  == 16 || width  == 32 || width  == 64 || width  == 128 || width  == 256 || width  == 512 || width  == 1024 ) &&
           ( height == 8 || height == 16 || height == 32 || height == 64 || height == 128 || height == 256 || height == 512 || height == 1024 );
}

// Writes n bytes of junk to file.
void WriteJunk( FILE* fp, int n )
{
    const quint8 junk = 0;
    for ( int i=0; i<n; i++ )
        fwrite( &junk, sizeof(quint8), 1, fp );
}

bool WritePalette( const std::string& filename, const QHash<quint32, quint8>& colorMap )
{
    quint32 palette[256];
    memset( palette, 0, sizeof(quint32) * 256 );

    for ( QHash<quint32, quint8>::const_iterator it = colorMap.begin(); it != colorMap.end(); ++it )
        palette[ it.value() ] = it.key();

    // Write the palette
    PaletteHeader header;
    memcpy( &header.magic, "PALT", 4 );
    header.num_colors = colorMap.size();

    FILE* fp = fopen( filename.c_str(), "wb" );

    if ( fp == NULL )
    {
        std::cerr << "Failed to open " << filename << std::endl;
        return false;
    }

    fwrite( &header, sizeof(PaletteHeader), 1, fp );

    if ( header.num_colors > 0 )
        fwrite( palette, sizeof(quint32), header.num_colors, fp );

    fclose( fp );
    return true;
}

// Converts a single QImage to 16BPP and writes it to file.
void Write16BPPImage( FILE* fp, const ColorConverter* converter, const QImage& image )
{
    Twiddler twiddler( image.width(), image.height() );
    const int pixels = image.width() * image.height();

    for ( int i=0; i<pixels; i++ )
    {
        const int index = twiddler.GetIndex( i );
        const int x = index % image.width();
        const int y = index / image.width();

        quint32 col = image.pixel( x, y );
        quint16 pxl = converter->To16BPP( col );
        fwrite( &pxl, sizeof(quint16), 1, fp );
    }
}

// Writes index data of a single paletted pixelmap to file.
void WritePalettedImage( FILE* fp, int format, const QImage& image, const QHash<quint32, quint8>& colorMap )
{
    if ( image.width() == 1 )
    {
        // Special case, this is the same for both 4BPP and 8BPP.
        quint32 col = image.pixel( 0, 0 );
        quint8 colIndex = colorMap.find( col ).value();
        fwrite( &colIndex, sizeof(quint8), 1, fp );
    }
    else
    {
        Twiddler twiddler( image.width(), image.height() );
        const int pixels = image.width() * image.height();

        if ( format == FORMAT_PAL4BPP )
        {
            // 4BPP, write two pixels at a time.
            for ( int i=0; i<pixels; i+=2 )
            {
                quint8 colIndex[2];

                for ( int j=0; j<2; j++ )
                {
                    const int index = twiddler.GetIndex( i + j );
                    const int x = index % image.width();
                    const int y = index / image.width();
                    const quint32 col = image.pixel( x, y );
                    colIndex[j] = colorMap.find( col ).value();
                }

                quint8 index = ( ( colIndex[1] & 0xF ) << 4 ) | ( colIndex[0] & 0xF );
                fwrite( &index, sizeof(quint8), 1, fp );
            }
        }
        else if ( format == FORMAT_PAL8BPP )
        {
            for ( int i=0; i<pixels; i++ )
            {
                const int index = twiddler.GetIndex( i );
                const int x = index % image.width();
                const int y = index / image.width();

                quint32 col = image.pixel( x, y );
                quint8 colIndex = colorMap.find( col ).value();
                fwrite( &colIndex, sizeof(quint8), 1, fp );
            }
        }
    }
}

void WriteVqTexture( FILE* fp, const ColorConverter* converter, const VQCodebook& codebook, const QVector<QuadImage*>& mipmaps )
{
    //------------
    // 1. Codebook
    //------------

    quint16 codes[ 256 * 4 ];
    memset( codes, 0, 256 * 4 * sizeof(quint16) );

    // Fill in the codebook
    for ( int i=0; i<codebook.GetNumberOfCodes(); i++ )
    {
        const VQQuad& quad = codebook.GetCodeValue( i );

        // Codebook needs to be twiddled
        codes[ i * 4 + 0 ] = quad.GetColor(0).Get16BPP( converter );
        codes[ i * 4 + 1 ] = quad.GetColor(2).Get16BPP( converter );
        codes[ i * 4 + 2 ] = quad.GetColor(1).Get16BPP( converter );
        codes[ i * 4 + 3 ] = quad.GetColor(3).Get16BPP( converter );
    }

    // Write the codebook
    fwrite( codes, 2, 256 * 4, fp );

    //------------
    // 2. Indices
    //------------

    // The 1x1 pixel is a special case. It references the same quad as
    // the 2x2 mipmap does.
    if ( mipmaps.size() > 1 )
    {
        const QuadImage* pImage = mipmaps.back();
        quint8 index = (quint8) codebook.Find( pImage->GetQuad( 0 ) );
        fwrite( &index, 1, 1, fp );
    }

    // Write the rest (or only) mipmap(s).
    QVectorIterator<QuadImage*> it( mipmaps );
    it.toBack();
    while ( it.hasPrevious() )
    {
        const QuadImage* pImage = it.previous();
        int quadsAcross = pImage->GetDimension() / 2;
        Twiddler twiddler( quadsAcross, quadsAcross );
        const int indices = quadsAcross * quadsAcross;

        for ( int i=0; i<indices; i++ )
        {
            quint8 index = (quint8) codebook.Find( pImage->GetQuad( twiddler.GetIndex( i ) ) );
            fwrite( &index, 1, 1, fp );
        }
    }
}

void WriteLosslessVqTexture( FILE* fp, const std::map<VQQuad16, quint8>& quadMap, const QVector<QuadImage16*>& mipmaps )
{
    //------------
    // 1. Codebook
    //------------

    quint16 codes[ 256 * 4 ];
    memset( codes, 0, 256 * 4 * sizeof(quint16) );

    // Fill in the codebook
    for ( std::map<VQQuad16, quint8>::const_iterator it = quadMap.begin(); it != quadMap.end(); ++it )
    {
        int index = (int) it->second;

        // Codebook needs to be twiddled
        codes[ index * 4 + 0 ] = it->first.col[0];
        codes[ index * 4 + 1 ] = it->first.col[2];
        codes[ index * 4 + 2 ] = it->first.col[1];
        codes[ index * 4 + 3 ] = it->first.col[3];
    }

    // Write the codebook
    fwrite( codes, 2, 256 * 4, fp );

    //------------
    // 2. Indices
    //------------

    // The 1x1 pixel is a special case. It references the same quad as
    // the 2x2 mipmap does.
    if ( mipmaps.size() > 1 )
    {
        const QuadImage16* pImage = mipmaps.back();
        quint8 index = 0;

        std::map<VQQuad16, quint8>::const_iterator it = quadMap.find( pImage->GetQuad( 0 ) );

        if ( it != quadMap.end() )
            index = it->second;
        else
            std::cerr << "Couldn't find lossless quad in map" << std::endl;

        fwrite( &index, 1, 1, fp );
    }

    // Write the rest (or only) mipmap(s).
    QVectorIterator<QuadImage16*> it( mipmaps );
    it.toBack();
    while ( it.hasPrevious() )
    {
        const QuadImage16* pImage = it.previous();
        int quadsAcross = pImage->GetDimension() / 2;
        Twiddler twiddler( quadsAcross, quadsAcross );
        const int indices = quadsAcross * quadsAcross;

        for ( int i=0; i<indices; i++ )
        {
            quint8 index = 0;
            std::map<VQQuad16, quint8>::const_iterator it2 = quadMap.find( pImage->GetQuad( twiddler.GetIndex( i ) ) );

            if ( it2 != quadMap.end() )
                index = it2->second;
            else
                std::cerr << "Couldn't find lossless quad in map" << std::endl;

            fwrite( &index, 1, 1, fp );
        }
    }
}

int main(int argc, char *argv[])
{
    // Check if there are enough arguments. If not, print usage.
    if ( argc < 4 )
    {
        std::cerr << "Usage: textool <options...> <format> <infilename> <outfilename>" << std::endl;
        std::cerr << "  Options:" << std::endl;
        std::cerr << "    -c   Compress texture" << std::endl;
        std::cerr << "    -m   Create mipmaps" << std::endl;
        std::cerr << "    -p   Generate preview image (<outfilename>_preview.png)" << std::endl;
        std::cerr << "    -v   Verbose mode" << std::endl;
        std::cerr << "  Formats:" << std::endl;
        std::cerr << "    RGB565" << std::endl;
        std::cerr << "    ARGB1555" << std::endl;
        std::cerr << "    ARGB4444" << std::endl;
        std::cerr << "    PAL4BPP" << std::endl;
        std::cerr << "    PAL8BPP" << std::endl;
        return -1;
    }

    // Some simplifications for parsing the commandline arguments
    int optStart = 1;
    int optEnd = ( argc - 3 );
    const std::string formatArg = argv[argc - 3];
    const std::string infileArg = argv[argc - 2];
    const std::string outfileArg = argv[argc - 1];
    std::string previewFilename;

    // Options
    bool bCompress = false;
    bool bMipmap = false;
    bool bPaletted = false;
    bool bVerbose = false;
    int format = -1;

    // Parse the commandline options
    for ( int i=optStart; i<optEnd; i++ )
    {
        if ( strcmp( argv[i], "-c" ) == 0 )
            bCompress = true;
        else if ( strcmp( argv[i], "-m" ) == 0 )
            bMipmap = true;
        else if ( strcmp( argv[i], "-v" ) == 0 )
            bVerbose = true;
        else if ( strcmp( argv[i], "-p" ) == 0 )
            previewFilename = outfileArg + "_preview.png";
        else
        {
            std::cerr << "Unknown option '" << argv[i] << "'" << std::endl;
            return -1;
        }
    }

    ColorConverter* colconv = NULL;

    // Parse the wanted fomat
    if ( formatArg == "RGB565" )
    {
        format = FORMAT_RGB565;
        colconv = new RGB565Converter();
    }
    else if ( formatArg == "ARGB1555" )
    {
        format = FORMAT_ARGB1555;
        colconv = new ARGB1555Converter();
    }
    else if ( formatArg == "ARGB4444" )
    {
        format = FORMAT_ARGB4444;
        colconv = new ARGB4444Converter();
    }
    else if ( formatArg == "PAL4BPP" )
    {
        format = FORMAT_PAL4BPP;
        bPaletted = true;
    }
    else if ( formatArg == "PAL8BPP" )
    {
        format = FORMAT_PAL8BPP;
        bPaletted = true;
    }
    else
    {
        std::cerr << "Unknown format '" << formatArg << "'" << std::endl;
        return -1;
    }

    // Make sure we're not trying to compress a paletted image
    if ( bPaletted && bCompress )
    {
        std::cerr << "Paletted images can't be compressed" << std::endl;
        return -1;
    }

    // Time to load the image
    QVector<QImage> mipmaps;
    mipmaps.push_back( QImage( infileArg.c_str() ) );

    if ( mipmaps[0].isNull() )
    {
        std::cerr << "Failed to load image '" << infileArg << "'" << std::endl;
        return -1;
    }

    if ( bVerbose )
        std::cout << "Loaded input image '" << infileArg << "'" << std::endl;

    // Check if the texture is a valid size
    if ( !IsValidSize( mipmaps[0].width(), mipmaps[0].height() ) )
    {
        std::cerr << "Invalid texture size (" << mipmaps[0].width() << ", " << mipmaps[0].height() << ")" << std::endl;
        return -1;
    }

    // Compressed and mipmapped images must be square
    if ( ( bCompress || bMipmap ) && ( mipmaps[0].width() != mipmaps[0].height() ) )
    {
        std::cerr << "Compressed and/or mipmapped textures must be square" << std::endl;
        return -1;
    }

    // Grab the palette and make sure there aren't too many colors
    QHash<quint32, quint8> colorMap;
    if ( bPaletted )
    {
        const int maxColors = (format == FORMAT_PAL4BPP) ? 16 : 256;

        for ( int y=0; y<mipmaps[0].height(); y++ )
        {
            for ( int x=0; x<mipmaps[0].width(); x++ )
            {
                quint32 color = mipmaps[0].pixel( x, y );

                if ( !colorMap.contains( color ) )
                {
                    // We need to insert another color. Check if it's ok.
                    if ( (int)colorMap.size() >= maxColors )
                    {
                        std::cerr << "Too many colors (" << colorMap.size() << ") in palette! Exiting..." << std::endl;
                        return -1;
                    }

                    colorMap.insert( color, (quint8)colorMap.size() );
                }
            }
        }

        if ( bVerbose )
            std::cout << "Colors in palette: " << colorMap.size() << std::endl;
    }

    // Create mipmaps if needed
    if ( bMipmap )
    {
        while ( mipmaps.back().width() > 1 )
        {
            const int newSize = mipmaps.back().width() / 2;
            const Qt::TransformationMode mode = bPaletted ? Qt::FastTransformation : Qt::SmoothTransformation;
            mipmaps.push_back( mipmaps.back().scaled( newSize, newSize, Qt::IgnoreAspectRatio, mode ) );
        }

        if ( bVerbose )
            std::cout << "Generated mipmaps" << std::endl;
    }

    FILE* out = fopen( outfileArg.c_str(), "wb" );

    if ( out == NULL )
    {
        std::cerr << "Failed to open output file '" << outfileArg << "'" << std::endl;
        return -1;
    }

    // Create and write the texture header
    TextureHeader header;
    memcpy( &header.magic, "TXTR", 4 );
    header.width = mipmaps[0].width();
    header.height = mipmaps[0].height();
    header.format = format;
    header.flags = FLAG_TWIDDLED | ( bCompress ? FLAG_COMPRESSED : 0 ) | ( bMipmap ? FLAG_MIPMAPPED : 0 );
    header.size = CalculateSize( format, mipmaps[0].width(), mipmaps[0].height(), bMipmap, bCompress );
    fwrite( &header, sizeof(TextureHeader), 1, out );

    //===================
    // Compressed images
    //===================
    if ( bCompress )
    {
        // The whole compression stuff is basically just taken from vqenc. I converted
        // it back to C++ (the code vqenc was based on was C++) to hopefully make it
        // easier to follow. I also added a check for lossless compression. The way
        // this works is that if there are only as many unique quads in the image as
        // the codebook can hold, we don't have to run the compression algorithm. Just
        // grab those quads and write the texture. Sure, this will only work for VERY
        // few textures, but the check doesn't take long and I still convert the source
        // images to 16BPP in either case (see below for details).

        QVector<QuadImage16*> reducedMipmaps;
        std::set<VQQuad16> uniqueQuads;

        // I noticed better end results by doing this. Reduce the original image to whatever output
        // format we need before the compression. Fewer color variations in the input image = better
        // compression.
        for ( QVector<QImage>::iterator it = mipmaps.begin(); it != mipmaps.end(); ++it )
        {
            if ( it->width() > 1 )
            {
                // Create a Quad16 image of the original mipmap
                reducedMipmaps.push_back( new QuadImage16( (*it), colconv ) );

                // Keep looking for unique quads
                const QuadImage16* img = reducedMipmaps.back();
                for ( int i=0; i<img->GetNumberOfQuads(); i++ )
                    uniqueQuads.insert( img->GetQuad(i) );
            }
        }

        if ( bVerbose )
            std::cout << uniqueQuads.size() << " unique quads in image" << std::endl;

        // Check if we can do lossless compression
        if ( uniqueQuads.size() <= 256 )
        {
            if ( bVerbose )
                std::cout << "Going for 'lossless' vq compression" << std::endl;

            std::map<VQQuad16, quint8> quadMap;
            quint8 index = 0;
            for ( std::set<VQQuad16>::iterator it = uniqueQuads.begin(); it != uniqueQuads.end(); ++it )
            {
                quadMap.insert( std::make_pair( (*it), index ) );
                index++;
            }

            // Write texture
            WriteLosslessVqTexture( out, quadMap, reducedMipmaps );

            if ( bVerbose )
                std::cout << "Wrote texture '" << outfileArg << "'" << std::endl;

            // Generate and write preview if neccessary
            if ( !previewFilename.empty() )
            {
                WriteLosslessVqPreview( previewFilename, reducedMipmaps, colconv );

                if ( bVerbose )
                    std::cout << "Wrote preview image '" << previewFilename << "'" << std::endl;
            }
        }

        // Go for lossy compression
        else
        {
            if ( bVerbose )
                std::cout << "Going for lossy vq compression" << std::endl;

            QVector<QuadImage*> quadMipmaps;
            VQCodebook codebook;

            // Generate quads of each source mipmap, but skip the 1x1 map.
            for ( QVector<QuadImage16*>::iterator it = reducedMipmaps.begin(); it != reducedMipmaps.end(); ++it )
                quadMipmaps.push_back( new QuadImage( (*it), colconv ) );

            // The actual encoding part. This is taken from vqenc and has been modified for C++.
            // It seems the official SEGA encoder does a WAY faster job at this as well as producing
            // nicer results. This however, is better than nothing.
            for ( int i=0; i<8; i++ )
            {
                for ( int j=0; j<3; j++ )
                {
                    codebook.ResetCodes();
                    for ( QVector<QuadImage*>::iterator it=quadMipmaps.begin(); it!=quadMipmaps.end(); ++it )
                        codebook.PlaceQuads( *it );
                    codebook.Clean();
                }

                // Each split doubles the amount of codes in the codebook.
                // 8 splits = 2^8 = 256 codes.
                codebook.Split();

                if ( bVerbose )
                    std::cout << "(" << (i+1) << "/8) splits done" << std::endl;
            }

            // Write texture
            WriteVqTexture( out, colconv, codebook, quadMipmaps );

            if ( bVerbose )
                std::cout << "Wrote texture '" << outfileArg << "'" << std::endl;

            // Generate and write preview if neccessary
            if ( !previewFilename.empty() )
            {
                WriteVqPreview( previewFilename, quadMipmaps, codebook, colconv );

                if ( bVerbose )
                    std::cout << "Wrote preview image '" << previewFilename << "'" << std::endl;
            }

            for ( QVector<QuadImage*>::iterator it = quadMipmaps.begin(); it != quadMipmaps.end(); ++it )
                delete (*it);
        }

        for ( QVector<QuadImage16*>::iterator it = reducedMipmaps.begin(); it != reducedMipmaps.end(); ++it )
            delete (*it);
    }

    //=================
    // Paletted images
    //=================
    else if ( bPaletted )
    {
        WritePalette( outfileArg + ".pal", colorMap );

        if ( bMipmap )
        {
            if ( format == FORMAT_PAL4BPP )
                WriteJunk( out, PAL4BPP_MM_OFFSET );
            else if ( format == FORMAT_PAL8BPP )
                WriteJunk( out, PAL8BPP_MM_OFFSET );
        }

        // Write the index data
        QVectorIterator<QImage> it(mipmaps);
        it.toBack();
        while ( it.hasPrevious() )
            WritePalettedImage( out, format, it.previous(), colorMap );

        if ( bVerbose )
            std::cout << "Wrote texture '" << outfileArg << "'" << std::endl;

        // Generate and write preview if neccessary
        if ( !previewFilename.empty() )
        {
            // No point in generating previews for paletted images really. They're stored in
            // ARGB8888 so there's no loss in quality from the input image.
            // Still doing this to preview the generated mipmaps though.
            Write16BPPPreview( previewFilename, mipmaps, NULL );

            if ( bVerbose )
                std::cout << "Wrote preview image '" << previewFilename << "'" << std::endl;
        }
    }

    //===============
    // 16BPP images
    //===============
    else
    {
        if ( bMipmap )
            WriteJunk( out, COL16BPP_MM_OFFSET );

        QVectorIterator<QImage> it(mipmaps);
        it.toBack();
        while ( it.hasPrevious() )
            Write16BPPImage( out, colconv, it.previous() );

        if ( bVerbose )
            std::cout << "Wrote texture '" << outfileArg << "'" << std::endl;

        // Generate and write preview if neccessary
        if ( !previewFilename.empty() )
        {
            Write16BPPPreview( previewFilename, mipmaps, colconv );

            if ( bVerbose )
                std::cout << "Wrote preview image '" << previewFilename << "'" << std::endl;
        }
    }

    fclose( out );

    delete colconv;
    return 0;
}
