#ifndef ELYSIAN_FONT_HPP
#define ELYSIAN_FONT_HPP

#include "elysian_sprite.hpp"

#define ELYSIAN_FONT_BUFFER_SIZE                200
#define ELYSIAN_FONT_SPACING_DEFAULT            2

#define ELYSIAN_FONT_MARKUP_SIGNAL_CHAR         '~'

#define ELYSIAN_FONT_MARKUP_COLOR_CHAR          'c'
#define ELYSIAN_FONT_MARKUP_COLOR_PALETTE_SIZE  50

namespace elysian {

	class Font: public Sprite {
	public:

		enum ALIGNMENT	{ C_ALIGN, L_ALIGN, R_ALIGN };
		        bool            loadTextData(const char* const imgFile, const int sheetWidth, const int sheetHeight, const int spriteWidth, const int spriteHeight, const char* const widthFile);
        void            drawString(const ALIGNMENT align, const char *str);

        unsigned        getStringPixelWidth(const char *string);
        unsigned        stringLengthWithinWidth(const char*string, const unsigned int width);
		
        void            drawStringf(const ALIGNMENT align, const char *str, ...);

        void            startBatchRender(void);
        void            endBatchRender(void);

        static GYColor4 getColorPaletteEntry(const unsigned index);
        static void     setColorPaletteEntry(const unsigned index, const GYColor4 color);

	private:

        static GYColor4 _colorPalette[ELYSIAN_FONT_MARKUP_COLOR_PALETTE_SIZE];
        unsigned short  _charWidth[128];
        unsigned short  _spacing = ELYSIAN_FONT_SPACING_DEFAULT;
        bool            _batching = false;

        bool            _loadCharacterWidths(const char *const widthFile);
        void            _renderChar(float &curX, float &curY, const char *const str, int &i);

        void            _processColorMarkup(const char*const string, int &index);
		
	};







	// ====== INLINEZ =====


	inline void Font::startBatchRender(void) {
		_batching = true;
		gyVidTexBind(_tex);
		if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_PUNCHTHRU);
		else gyVidAlphaLevel(GY_ALPHA_FULL);
		//gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
		gyVidQuadListBegin(&_color);
	}
	inline void Font::endBatchRender(void) {
		_batching = false;
		gyVidQuadListEnd();
	}

    inline unsigned int Font::stringLengthWithinWidth(const char *const string, const unsigned int width) {
        unsigned curWidth = 0;
        unsigned curIndex = 0;
        unsigned length = strlen(string);

        while(curWidth < width && curIndex < length) {
            ++curIndex;
            curWidth += _charWidth[(int)string[curIndex]] + _spacing;
        }

        if(curWidth > width) --curIndex;

        return curIndex;
    }

    inline GYColor4 Font::getColorPaletteEntry(const unsigned index) { return _colorPalette[index]; }
    inline void Font::setColorPaletteEntry(const unsigned index, const GYColor4 color) { _colorPalette[index] = color; }

    inline void Font::_processColorMarkup(const char*const string, int &index) {
        unsigned paletteEntry   = (unsigned)(string[++index] - 48) * 10;
        paletteEntry            += (unsigned)(string[++index] - 48);

        GYColor4 color = _colorPalette[paletteEntry];
        color.a = _color.a;
        setColor(color);

        //We have to end the current quad list and start another.
        gyVidQuadListEnd();
        if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_PUNCHTHRU);
        else gyVidAlphaLevel(GY_ALPHA_FULL);
        gyVidQuadListBegin(&_color);
    }
}

#endif
