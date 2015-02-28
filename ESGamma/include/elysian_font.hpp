#ifndef ELYSIAN_FONT_HPP
#define ELYSIAN_FONT_HPP

#include "elysian_sprite.hpp"

#define ELYSIAN_FONT_SPACING_DEFAULT    2


namespace elysian {

	class Font: public Sprite {
	public:
		enum ALIGNMENT	{ C_ALIGN, L_ALIGN, R_ALIGN };
		enum FORMAT		{ NO_FORMAT, FORMATTED };
		//enum CAMERA		{ NO_CAMERA, WITH_CAMERA};
		
		bool loadTextData(const char* const imgFile, int sheetWidth, int sheetHeight, int spriteWidth, int spriteHeight, const char* const widthFile);
		void drawString(const FORMAT format, const ALIGNMENT align, const char *str);

		unsigned int getStringPixelWidth(const char *string);
        unsigned int stringLengthWithinWidth(const char*string, unsigned int width);
		
		void drawStringf(const FORMAT format, const ALIGNMENT align, const char *str, ...);

		void startBatchRender(void);
		void endBatchRender(void);

		//virtual void render(void); //we can reimplement this with a stored string for RenderableObject polymorphic compatability
	private:

        unsigned short _spacing = ELYSIAN_FONT_SPACING_DEFAULT;
		bool _batching = false;
		bool _loadCharacterWidths(const char *const widthFile);
		unsigned short _charWidth[128];
		
		void _renderChar(const FORMAT format, float &curX, float &curY, const char *const str, int &i);
		
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
}

#endif
