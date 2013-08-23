#ifndef SPRITE_H
#define SPRITE_H

#include <QPixmap>
#include "tileSheetUtilities.h"
//#include "elysian_textured.hpp"

/*
	THIS ENTIRE CLASS HAS BEEN CASTRATED SINCE THE ELYSIAN RENDERABLE
	PARADIGM HAS COMPLETELY CHANGED!!!
*/


//DELETE ME
namespace elysian {
	typedef void Textured;
}

//Can either encapsulate an EXISTING elysian::Sprite or be standalone
class SpriteSheet {
    friend class TileManager;
	friend class Renderable;
private:
	//elysian::Textured* _attrib; //This is still used to store attributes
	QPixmap *_frames;
	bool _fromElysian;

public:
	SpriteSheet(const QString /*file*/, const unsigned short int /*spriteWidth*/, const unsigned short int /*spriteHeight*/): /*_attrib(new elysian::Textured),*/ _frames(NULL), _fromElysian(false) {
#if 0
		_attrib->LoadTexture(file.toStdString().c_str(), 0, 0, spriteWidth, spriteHeight);
		if(!TileManager::DivideSpriteSheet(*this)) {
			delete _frames;
			_frames = NULL;
		}
#endif
	}

	//Create SpriteSheet from existing elysian::Textured attributes
	SpriteSheet(elysian::Textured* /*source*/, bool /*load=true*/): /*_attrib(source),*/ _frames(NULL), _fromElysian(true) {
#if 0
		if(load) {
			if(!TileManager::DivideSpriteSheet(*this)) {
				delete _frames;
				_frames = NULL;
			}
		}
#endif
	}

	~SpriteSheet() {
//		if(!_fromElysian) delete _attrib;
		delete[] _frames;
	}

	bool Reload(const QString /*file*/, const unsigned short int /*spriteWidth*/, const unsigned short int /*spriteHeight*/) {
#if 0
		unsigned int sheetWidth, sheetHeight;

		if(!TileManager::GetImageSize(file, sheetWidth, sheetHeight)) {
			delete[] _frames;
			_frames = NULL;
			return false;
		}
		_attrib->LoadTexture(file.toStdString().c_str(), sheetWidth, sheetHeight, spriteWidth, spriteHeight);
		delete[] _frames;
		_frames = NULL;
		if(!TileManager::DivideSpriteSheet(*this)) {
			delete _frames;
			_frames = NULL;
			return false;
		}
		return true;
#endif
		return false;
	}

	QPixmap *getPixmap(unsigned short int /*frame*/) const { /*return (_frames)? &_frames[frame] : NULL;*/ return NULL; }
	QPixmap *getCurrentPixmap() const { /*return (_frames)? &_frames[_attrib->getFrame()] : NULL;*/ return NULL; }

	void setFrame(unsigned /*fr*/) const { /*_attrib->setFrame(fr);*/ }

	void getSpriteSize(unsigned &/*width*/, unsigned &/*height*/) const {/* _attrib->getSpriteSize(width, height);*/ }
};


#endif // SPRITE_H
