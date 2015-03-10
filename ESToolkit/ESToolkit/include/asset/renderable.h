#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "assetManager.h"
#include "component.h"
//#include "elysian_primitive.hpp"
#include <gyro.h>

class QPushButton;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushBUtton;
class QCheckBox;
class QPixmap;

class SpriteSheet;

namespace elysian {
	class Renderable;
    class SpritePrim;
    class RectPrim;
}

//ONLY ENCAPSULATING/HANDLING RECT AND SPRITE PRIMITIVES!!!!
class Renderable: public Component, public AssetManager {
private:
//	elysian::Renderable *_elysianRenderable;
	SpriteSheet *_sheet;
/*
	union ElysianPrimitive {
		elysian::SpritePrim *elysianSprite;
		elysian::RectPrim *elysianRect;
	}_elysianPrim;
*/
	QVBoxLayout *_mainLayout;
	QGridLayout *_colorLayout, *_texLayout;
	QLabel *_colorLabel, *_rLabel, *_gLabel, *_bLabel, *_aLabel, *_texLabel, *_sizeLabel,
		*_widthLabel, *_heightLabel, *_sheetLabel, *_spriteLabel, *_frameLabel;
	QLineEdit *_rEdit, *_gEdit, *_bEdit, *_aEdit, *_texEdit, *_shWEdit, *_shHEdit, *_sprWEdit, *_sprHEdit,
		*_frameEdit;
	QPushButton *_swatchBut, *_browseBut, *_reloadBut;
	QCheckBox *_texCheckBox;

    void ReloadSpriteSheet(void);
/*
public Q_SLOTS:

    void OnSetColorR(void);
    void OnSetColorG(void);
    void OnSetColorB(void);
    void OnSetColorA(void);

	void OnSetTextured(bool val);
	void OnSetTexturedPath();
	//void OnSetSheetWidth();
	//void OnSetSheetHeight();
	void OnSetSpriteWidth();
	void OnSetSpriteHeight();
	void OnSetFrame();
	void OnSwatchClick();
	void OnBrowseClick();
	void OnReloadClick();

Q_SIGNALS:
    void PrimitiveUpdate(); */

public:

	void InitializeUI(void);
	void PopulateUI(void);

	Renderable(elysian::Renderable *const comp);

    ~Renderable(void);
	elysian::Component::TYPE getType(void) const { return elysian::Component::RENDERABLE; }
	//elysian::Primitive::TYPE getPrimType(void) const;

    GYColor4 getColor(void) const;

    QPixmap* getCurrentFramePixmap(void) const;
    void getSpriteSize(unsigned &width, unsigned &height) const;
	void setColor(float r, float g, float b, float a);
};

#endif // RENDERABLE_H
