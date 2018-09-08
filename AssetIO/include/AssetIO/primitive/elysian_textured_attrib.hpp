#ifndef ELYSIAN_TEXTURED_ATTRIB_HPP
#define ELYSIAN_TEXTURED_ATTRIB_HPP

#include <libGyro/gyro_texture_api.h>
#include <string.h>
#include <cassert>

#define ELYSIAN_TEXTURE_PATH_SIZE	100

struct GYTexture;

namespace elysian {

	//Base class for textured renderables containing ATTRIBUTES
	//GYTexture C++ abstraction

	/*
		Preprocessor directives are technically an OPTIMIZATION.
		The engine doesn't neeed to duplicate texture data, because every instance of the same
		texture shares one set of attributes. The Toolkit doesn't have the libGyro runtime,
		so every texture needs to have its own separate attributes.
	 */
    class TexturedAttrib {
	protected:
		GYTexture* _tex;
		unsigned spriteWidth, spriteHeight;//, frame;
	public:
        TexturedAttrib(void): _tex(nullptr) { }

        ~TexturedAttrib(void);

        //Gonna shit their pants if there is no texture allocated yet...
        void getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const;
        void getSpriteSize(unsigned& spriteWidth, unsigned& spriteHeight) const;
        const char* getTexPath(void) const;

        void loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight);
        void loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight);


        //unsigned getFrame(void) const { return frame; }
        //void setFrame(const unsigned fr) { frame = fr; }
    };

}

#endif
