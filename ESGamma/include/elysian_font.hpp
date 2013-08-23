#ifndef ELYSIAN_FONT_HPP
#define ELYSIAN_FONT_HPP

#include "elysian_sprite.hpp"

namespace elysian {

//tolua_begin
	class Font: public Sprite {
	public:
		enum ALIGNMENT	{ C_ALIGN, L_ALIGN, R_ALIGN };
		enum FORMAT		{ NO_FORMAT, FORMATTED };
		//enum CAMERA		{ NO_CAMERA, WITH_CAMERA};
		
		bool load(const char* const imgFile, int sheetWidth, int sheetHeight, int spriteWidth, int spriteHeight, const char* const widthFile);
		void drawString(const FORMAT format, const ALIGNMENT align, const char *str);

		unsigned int getStringPixelWidth(const char *string);
//tolua_end
		
		void drawStringf(const FORMAT format, const ALIGNMENT align, const char *str, ...);
		Font(void);

		//virtual void render(void); //we can reimplement this with a stored string for RenderableObject polymorphic compatability
	private:
		bool _loadCharacterWidths(const char *const widthFile);
		unsigned short _charWidth[128];
		
		void _renderChar(const FORMAT format, float &curX, float &curY, const char *const str, int &i);
		
	}; //tolua_export
}

#endif