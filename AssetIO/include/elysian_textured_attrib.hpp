#ifndef ELYSIAN_TEXTURED_ATTRIB_HPP
#define ELYSIAN_TEXTURED_ATTRIB_HPP

#include "gyro.h"
#include "elysian_asset.hpp"
#include <fstream>
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
	class TexturedAttrib: public Asset {
	protected:
		GYTexture* _tex;
		unsigned spriteWidth, spriteHeight;//, frame;

#ifndef ELYSIAN_ENGINE //Define basic texture functionality for non-libGyro platforms
		static GYTexture* _TkTexCreate(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight) {
                        if(spriteWidth == 0 || spriteHeight == 0) return NULL;
                        GYTexture *tex = new GYTexture;
                        unsigned int x_tiles = (unsigned int)(sheetWidth / spriteWidth);
                        unsigned int y_tiles = (unsigned int)(sheetHeight / spriteHeight);
			strcpy(tex->filename, filename);
			tex->_coord = new GYTexCoordSheet;
			tex->_coord->sheetWidth = sheetWidth;
			tex->_coord->sheetHeight = sheetHeight;
			tex->_coord->spriteWidth = spriteWidth;
			tex->_coord->spriteHeight = spriteHeight;
			tex->_coord->frames = x_tiles * y_tiles;
			return tex;
		}

		static bool _TkTexDestroy(GYTexture* const tex) {
			delete tex->_coord;
			delete tex;
			return true;
		}
#endif
	public:
		TexturedAttrib(void): _tex(NULL) { }

		~TexturedAttrib(void) {
			if(_tex) {
#ifdef ELYSIAN_ENGINE
				gyVidTexDestroy(_tex);
#else
				_TkTexDestroy(_tex);
#endif
			}
		}

		//Gonna shit their pants if there is no texture allocated yet...
		void getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const { sheetWidth = _tex->_coord->sheetWidth; sheetHeight = _tex->_coord->sheetHeight; }
		void getSpriteSize(unsigned& spriteWidth, unsigned& spriteHeight) const { spriteWidth = _tex->_coord->spriteWidth; spriteHeight = _tex->_coord->spriteHeight; }
		const char* getTexPath(void) const { return _tex->filename; }

		void loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight) {
			if(_tex) {
#ifdef ELYSIAN_ENGINE
				gyVidTexDestroy(_tex);
#else
				_TkTexDestroy(_tex);
#endif
			}

#ifdef ELYSIAN_ENGINE
			_tex = gyVidTexCreateSheetUniform(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);
#else
			_tex = _TkTexCreate(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);
#endif
		}


		void loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight) {
				if(_tex) {
#ifdef ELYSIAN_ENGINE
				gyVidTexDestroy(_tex);
#else
				_TkTexDestroy(_tex);
#endif
			}

#ifdef ELYSIAN_ENGINE
			_tex = gyVidTexCreateSheet(filename, sheetWidth, sheetHeight);
#else
			_tex = _TkTexCreate(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);
#endif

		}

		bool load(std::ifstream& stream) {
			static char filename[GYRO_PATH_MAX_SIZE];
			unsigned sheetWidth, sheetHeight;
			unsigned spriteWidth, spriteHeight;

			dbgprintf(LOG_VERBOSE, "Textured::Load(ifstream&)");
			dbgPush();
			stream >> filename;
			stream >> sheetWidth;
			stream >> sheetHeight;
			stream >> spriteWidth;
			stream >> spriteHeight;
			dbgprintf(LOG_VERBOSE, "_texPath - %s", filename);
			dbgprintf(LOG_VERBOSE, "_sheetSize - <%d, %d>, _spriteSize - <%d, %d>",  sheetWidth, sheetHeight, spriteWidth, spriteHeight);
			//dbgprintf(LOG_VERBOSE, "_frame - %d", _curFrame);
#ifdef ELYSIAN_ENGINE
			_tex = gyVidTexCreateSheetUniform(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);
#else
			_tex = _TkTexCreate(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);
#endif
			dbgPop();
			return true;
		}

		bool save(std::ofstream& stream) {
			assert(_tex);
			assert(_tex->_coord);
			dbgprintf(LOG_VERBOSE, "Textured::Save(ofstream&)");
			dbgPush();
			stream << _tex->filename << std::endl;
			stream << _tex->_coord->sheetWidth << std::endl;
			stream << _tex->_coord->sheetHeight << std::endl;
			stream << _tex->_coord->spriteWidth << std::endl;
			stream << _tex->_coord->spriteHeight << std::endl;
			dbgPop();
			return true;
		}

		//unsigned getFrame(void) const { return frame; }
		//void setFrame(const unsigned fr) { frame = fr; }

	};

}

#endif
