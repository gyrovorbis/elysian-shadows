#ifndef ELYSIAN_TEXTURED_ATTRIB_HPP
#define ELYSIAN_TEXTURED_ATTRIB_HPP

#include <gyro_texture_api.h>
#include <string.h>
#include <cassert>

#define ELYSIAN_TEXTURE_PATH_SIZE	100

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

		~TexturedAttrib(void) {
			if(_tex) {
				gyVidTexDestroy(_tex);
			}
		}

		//Gonna shit their pants if there is no texture allocated yet...
		void getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const { sheetWidth = _tex->_coord->sheetWidth; sheetHeight = _tex->_coord->sheetHeight; }
		void getSpriteSize(unsigned& spriteWidth, unsigned& spriteHeight) const { spriteWidth = _tex->_coord->spriteWidth; spriteHeight = _tex->_coord->spriteHeight; }
		const char* getTexPath(void) const { return _tex->filename; }

		void loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight) {
			if(_tex) {

				gyVidTexDestroy(_tex);

			}

			_tex = gyVidTexCreateSheetUniform(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);

		}


		void loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight) {
				if(_tex) {
				gyVidTexDestroy(_tex);

			}

			_tex = gyVidTexCreateSheet(filename, sheetWidth, sheetHeight);


		}


		//unsigned getFrame(void) const { return frame; }
		//void setFrame(const unsigned fr) { frame = fr; }
	};

}

#endif
