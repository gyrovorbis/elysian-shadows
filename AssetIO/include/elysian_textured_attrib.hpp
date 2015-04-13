#ifndef ELYSIAN_TEXTURED_ATTRIB_HPP
#define ELYSIAN_TEXTURED_ATTRIB_HPP

#include <elysian_asset.hpp>
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
    class TexturedAttrib: public Asset {
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

        bool load(FILE* /*file*/) {
#if 0
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
            _tex = gyVidTexCreateSheetUniform(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);

            dbgPop();
            return true;
#endif
            return true;
        }

        bool save(FILE* /*file*/) {
#if 0
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
#endif
            return true;
        }

        //unsigned getFrame(void) const { return frame; }
        //void setFrame(const unsigned fr) { frame = fr; }
    };

}

#endif
