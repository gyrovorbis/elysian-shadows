///////////////////////////////////////////////////////////
//                 .__                                   //
//    ___________  |  |   _________.__. ____   ____      //
//    \____ \__  \ |  |  /  ___<   |  |/    \_/ ___\     //
//    |  |_> > __ \|  |__\___ \ \___  |   |  \  \___     //
//    |   __(____  /____/____  >/ ____|___|  /\___  >    //
//    |__|       \/          \/ \/         \/     \/     //
//                                                       //
///////////////////////////////////////////////////////////
// Syncs paletted textures output by texconv to use the  //
// same palette. It's only possible to mix 8BPP textures //
// with other 8BPP textures and vice versa for 4BPP.     //
//                                                       //
// By Anton Norgren, 2011.                               //
///////////////////////////////////////////////////////////

#include <QTypeInfo>
#include <QVector>
#include <QHash>
#include <QSet>

#include <iostream>
#include <cstdio>
#include <cstring>

// The supported texture formats.
#define FORMAT_INVALID          -1
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

class Texture
{
    public:

        Texture( const char* szName )
        {
            name = szName;
            memset( &header, 0, sizeof(TextureHeader) );
            data = NULL;
        }

        ~Texture()
        {
            delete[] data;
        }

        std::string     name;
        TextureHeader   header;
        quint8*         data;
};

// Palette header, 8 bytes
struct PaletteHeader
{
    char    magic[4];
    qint32  num_colors;
};

class Palette
{
    public:

        Palette( int nc )
        {
            memcpy( header.magic, "PALT", 4 );
            header.num_colors = nc;
            colors = new quint32[ nc ];
        }

        ~Palette()
        {
            delete[] colors;
        }

        PaletteHeader   header;
        quint32*        colors;
};

int GetBPP( int format )
{
    switch ( format )
    {
    case FORMAT_PAL4BPP:    return 4;
    case FORMAT_PAL8BPP:    return 8;
    default:                return 16;
    }
}

int GetNumColorsInPalette( int format )
{
    switch ( format )
    {
    case FORMAT_PAL4BPP:    return 16;
    case FORMAT_PAL8BPP:    return 256;
    default:                return 0;
    }
}

// Sync a 4BPP pixel buffer with the new palette.
// For each pixel in buffer:
//   Look up color in originalPalette
//   Find the new index of that color in colorMap
//   Replace the old index with the new one
void Sync4BPPTexture( const QHash<quint32, quint8>& colorMap, Texture* tex, const Palette* originalPalette )
{
    const bool mipmapped = ( ( tex->header.flags & FLAG_MIPMAPPED ) != 0 );
    const int first = mipmapped ? 2 : 0;

    // A 4BPP mipmapped texture actually starts at the 4th nibble.
    // So we need this extra stuff.
    if ( mipmapped )
    {
        quint8 oldIndex = tex->data[1] & 0xF;
        tex->data[1] = ( colorMap.find( originalPalette->colors[ oldIndex ] ).value() ) & 0xF;
    }

    for ( int i=first; i<tex->header.size; i++ )
    {
        quint8 lowerIndex = tex->data[i] & 0x0F;
        quint8 upperIndex = ( tex->data[i] & 0xF0 ) >> 4;
        quint8 newLowerIndex = ( colorMap.find( originalPalette->colors[ lowerIndex ] ).value() ) & 0xF;
        quint8 newUpperIndex = ( colorMap.find( originalPalette->colors[ upperIndex ] ).value() ) & 0xF;
        tex->data[i] = ( newUpperIndex << 4 ) | newLowerIndex;
    }
}

// Sync an 8BPP pixel buffer with the new palette.
// For each pixel in buffer:
//   Look up color in originalPalette
//   Find the new index of that color in colorMap
//   Replace the old index with the new one
void Sync8BPPTexture( const QHash<quint32, quint8>& colorMap, Texture* tex, const Palette* originalPalette )
{
    const bool mipmapped = ( ( tex->header.flags & FLAG_MIPMAPPED ) != 0 );
    const int first = mipmapped ? PAL8BPP_MM_OFFSET : 0;

    for ( int i=first; i<tex->header.size; i++ )
    {
        quint8 oldIndex = tex->data[i];
        tex->data[i] = colorMap.find( originalPalette->colors[ oldIndex ] ).value();
    }
}

Texture* LoadTexture( const char* filename, int& workingFormat )
{
    FILE* fp = NULL;
    TextureHeader hdr;
    Texture* tex = NULL;

    // Open up the texture file
    if ( ( fp = fopen( filename, "rb" ) ) != NULL )
    {
        // Read the texture header
        fread( &hdr, sizeof(TextureHeader), 1, fp );

        // Make sure it's a texture file
        if ( memcmp( hdr.magic, "TXTR", 4 ) == 0 )
        {
            // Make sure the texture is paletted
            if ( hdr.format == FORMAT_PAL4BPP || hdr.format == FORMAT_PAL8BPP )
            {
                // If we haven't specified a working format, set it now.
                if ( workingFormat == FORMAT_INVALID )
                {
                    workingFormat = hdr.format;
                    std::cout << "Working with " << GetBPP(workingFormat) << "BPP palettes" << std::endl;
                }

                // Make sure the working format equals that of the texture.
                if ( hdr.format == workingFormat )
                {
                    // It's FINALLY ok to load the texture data...
                    tex = new Texture( filename );
                    memcpy( &tex->header, &hdr, sizeof(TextureHeader) );
                    tex->data = new quint8[ hdr.size ];
                    fread( tex->data, 1, hdr.size, fp );
                }

                // The texture format is different from the working format, failure!
                else
                    std::cout << filename << " is a " << GetBPP(hdr.format) << "BPP texture. Can't sync when in " << GetBPP(workingFormat) << "BPP mode." << std::endl;
            }
            else
                std::cerr << filename << " is not a paletted texture" << std::endl;
        }
        else
            std::cerr << filename << " is not a texture file!" << std::endl;

        fclose( fp );
    }
    else
        std::cerr << "Failed to open " << filename << std::endl;

    return tex;
}

Palette* LoadPalette( const char* filename, int workingFormat )
{
    if ( workingFormat == FORMAT_INVALID )
        return NULL;

    FILE* fp = NULL;
    PaletteHeader hdr;
    Palette* pal = NULL;

    // Open up the palette file
    if ( ( fp = fopen( filename, "rb" ) ) != NULL )
    {
        // Read the palette header
        fread( &hdr, sizeof(PaletteHeader), 1, fp );

        // Make sure it's a palette file
        if ( memcmp( hdr.magic, "PALT", 4 ) == 0 )
        {
            // Make sure there aren't too many colors for the format we're working with
            if ( ( ( workingFormat == FORMAT_PAL4BPP ) && ( hdr.num_colors < 16 ) ) ||
                    ( ( workingFormat == FORMAT_PAL8BPP ) && ( hdr.num_colors < 256 ) ) )
            {
                // Read the palette data
                pal = new Palette( hdr.num_colors );
                fread( pal->colors, sizeof(quint32), hdr.num_colors, fp );
            }
            else
                std::cerr << "Too many colors in palette for an " << GetBPP(workingFormat) << "BPP texture" << std::endl;
        }
        else
            std::cerr << filename << " is not a palette file!" << std::endl;

        fclose( fp );
    }
    else
        std::cerr << "Failed to open " << filename << std::endl;

    return pal;
}

void SaveTexture( const Texture* tex )
{
    FILE* out = fopen( tex->name.c_str(), "wb" );

    if ( out == NULL )
        std::cerr << "Failed to open " << tex->name.c_str() << std::endl;
    else
    {
        fwrite( &tex->header, sizeof(TextureHeader), 1, out );
        fwrite( tex->data, 1, tex->header.size, out );
        fclose( out );
    }
}

void SavePalette( const char* filename, const QHash<quint32, quint8>& colorMap )
{
    FILE* out = fopen( filename, "wb" );

    if ( out == NULL )
        std::cerr << "Failed to open " << filename << std::endl;
    else
    {
        // Grab a linear presentation of the palette
        quint32 colors[256];
        memset( colors, 0, sizeof(quint32) * 256 );
        for ( QHash<quint32, quint8>::const_iterator it = colorMap.begin(); it != colorMap.end(); ++it )
            colors[ it.value() ] = it.key();

        PaletteHeader header;
        memcpy( &header.magic, "PALT", 4 );
        header.num_colors = colorMap.size();

        fwrite( &header, sizeof(PaletteHeader), 1, out );
        fwrite( colors, sizeof(quint32), colorMap.size(), out );
        fclose( out );
    }
}

void PrintUsage()
{
    std::cerr << "Usage: palsync <options...> <output palette> tex0.tex pal0.pal tex1.tex pal1.pal [tex2.tex pal2.pal...]" << std::endl;
    std::cerr << "  Options:" << std::endl;
    std::cerr << "    -v   Verbose mode" << std::endl;
}

int main( int argc, char** argv )
{
    // Make sure we have at least two input textures.
    if ( argc < 6 )
    {
        PrintUsage();
        return -1;
    }

    bool verbose = false;
    int optEnd = argc;

    // Parse options
    for ( int i=1; i<argc; i++ )
    {
        if ( argv[i][0] == '-' )
        {
            if ( strcmp( argv[i], "-v" ) == 0 )
                verbose = true;
            else
            {
                std::cerr << "Unknown option '" << argv[i] << "'" << std::endl;
                return -1;
            }
        }
        else
        {
            optEnd = i;
            break;
        }
    }

    // Still need 5 more arguments to run this stuff...
    int remainingArgs = argc - optEnd;
    if ( remainingArgs < 5 || ( remainingArgs % 2 ) == 0  )
    {
        PrintUsage();
        return -1;
    }

    int workingFormat = FORMAT_INVALID;
    QVector<Texture*> textures;
    QVector<Palette*> palettes;
    const char* palname = argv[optEnd];
    bool success = true;

    // Load all the textures and palettes
    for ( int i=(optEnd+1); i<argc; i+=2 )
    {
        if ( verbose )
            std::cout << "Loading '" << argv[i] << "'" << std::endl;

        Texture* tex = LoadTexture( argv[i], workingFormat );

        if ( tex == NULL )
        {
            success = false;
            break;
        }
        else
            textures.push_back( tex );

        if ( verbose )
            std::cout << "Loading '" << argv[i+1] << "'" << std::endl;

        Palette* pal = LoadPalette( argv[i+1], workingFormat );

        if ( pal == NULL )
        {
            success = false;
            break;
        }
        else
            palettes.push_back( pal );
    }

    if ( success )
    {
        QSet<quint32> combinedPalette;

        if ( verbose )
            std::cout << "Creating combined palette" << std::endl;

        // Create a unique set of the colors of all textures
        for ( int i=0; i<palettes.size(); i++ )
            for ( int j=0; j<palettes[i]->header.num_colors; j++ )
                combinedPalette.insert( palettes[i]->colors[j] );

        if ( verbose )
            std::cout << combinedPalette.size() << " colors in combined palette" << std::endl;

        if ( combinedPalette.size() > GetNumColorsInPalette( workingFormat ) )
            std::cerr << "Too many colors for " << GetBPP( workingFormat ) << "BPP.\nExiting..." << std::endl;
        else
        {
            // Create a color => index hash from that unique set.
            QHash<quint32, quint8> colorMap;
            for ( QSet<quint32>::iterator it = combinedPalette.begin(); it != combinedPalette.end(); ++it )
                colorMap.insert( (*it), (quint8)colorMap.size() );

            if ( verbose )
                std::cout << "Synching texture data" << std::endl;

            // Sync the index data of every texture with the new palette
            if ( workingFormat == FORMAT_PAL4BPP )
                for ( int i=0; i<textures.size(); i++ )
                    Sync4BPPTexture( colorMap, textures[i], palettes[i] );
            else if ( workingFormat == FORMAT_PAL8BPP )
                for ( int i=0; i<textures.size(); i++ )
                    Sync8BPPTexture( colorMap, textures[i], palettes[i] );

            // Save all the textures
            for ( int i=0; i<textures.size(); i++ )
            {
                if ( verbose )
                    std::cout << "Saving '" << textures[i]->name << "'" << std::endl;

                SaveTexture( textures[i] );
            }

            if ( verbose )
                std::cout << "Saving '" << palname << "'" << std::endl;

            // Save the new palette
            SavePalette( palname, colorMap );
        }
    }

    if ( verbose )
        std::cout << "Cleaning up" << std::endl;

    // Clean up
    for ( int i=0; i<textures.size(); i++ ) delete textures[i];
    for ( int i=0; i<palettes.size(); i++ ) delete palettes[i];

    return 0;
}
