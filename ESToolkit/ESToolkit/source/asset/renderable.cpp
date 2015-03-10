#include "renderable.h"
#include "sprite.h"
#include "project.h"
#include "elysian_area.hpp"
#include "elysian_renderable.hpp"
//#include "elysian_rect_prim.hpp"
//#include "elysian_sprite_prim.hpp"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

Renderable::~Renderable(void) { delete _sheet; }

void Renderable::ReloadSpriteSheet(void) {
    if(_texEdit->text().isEmpty() || _sprWEdit->text().isEmpty() || _sprHEdit->text().isEmpty() || _frameEdit->text().isEmpty()) return;
    _sheet->Reload(_texEdit->text(), _sprWEdit->text().toUInt(), _sprHEdit->text().toUInt());
    //Q_EMIT PrimitiveUpdate();
}
/*
void Renderable::OnSetColorR(void) { setColor(_rEdit->text().toFloat(), _gEdit->text().toFloat(), _bEdit->text().toFloat(), _aEdit->text().toFloat());}
void Renderable::OnSetColorG(void) { setColor(_rEdit->text().toFloat(), _gEdit->text().toFloat(), _bEdit->text().toFloat(), _aEdit->text().toFloat());}
void Renderable::OnSetColorB(void) { setColor(_rEdit->text().toFloat(), _gEdit->text().toFloat(), _bEdit->text().toFloat(), _aEdit->text().toFloat());}
void Renderable::OnSetColorA(void) { setColor(_rEdit->text().toFloat(), _gEdit->text().toFloat(), _bEdit->text().toFloat(), _aEdit->text().toFloat());}
*/
//elysian::Primitive::TYPE Renderable::getPrimType(void) const { /*return _elysianRenderable->getPrimType();*/ }

GYColor4 Renderable::getColor(void) const {
	GYColor4 color = { 255, 255, 255, 255 };
	//_elysianPrim.elysianRect->getColor(color);
    return color;
}

QPixmap* Renderable::getCurrentFramePixmap(void) const {
	//return (getPrimType() == elysian::Primitive::SPRITE)? _sheet->getCurrentPixmap() : NULL;
	return NULL;
}

void Renderable::getSpriteSize(unsigned &width, unsigned &height) const { _sheet->getSpriteSize(width, height); }
#if 0
void Renderable::OnSetTextured(bool /*val*/) {

	val = _texCheckBox->isChecked(); //don't trust that fucking parameter

	_texEdit->setEnabled(val);
	_browseBut->setEnabled(val);
	_sprWEdit->setEnabled(val);
	_sprHEdit->setEnabled(val);
	_frameEdit->setEnabled(val);
	_reloadBut->setEnabled(val);

	if(val) {
		GYColor4 color;
		_elysianPrim.elysianRect->getColor(color);
		_elysianPrim.elysianSprite = new elysian::SpritePrim;
//		_elysianRenderable->setPrimitive(_elysianPrim.elysianSprite);
		setColor(color.r, color.g, color.b, color.a);
		_sheet = new SpriteSheet(_elysianPrim.elysianSprite, false);
		ReloadSpriteSheet();
	}
	else {
		GYColor4 color;
		_elysianPrim.elysianSprite->getColor(color);
		_elysianPrim.elysianRect = new elysian::RectPrim;
//		_elysianRenderable->setPrimitive(_elysianPrim.elysianRect);
		setColor(color.r, color.g, color.b, color.a);
		delete _sheet;
		_sheet = NULL;
	}

}

void Renderable::OnSetTexturedPath() {

}

void Renderable::OnSetSpriteWidth() {

}
void Renderable::OnSetSpriteHeight() {

}

void Renderable::OnSetFrame() {
	_sheet->setFrame(_frameEdit->text().toUInt());
	Q_EMIT PrimitiveUpdate();
}
void Renderable::OnSwatchClick() {
	QColor color(255, 255, 255, 255);
//	color = QColorDialog::getColor(color, this, "Rect Color", QColorDialog::ShowAlphaChannel);

	if(color.isValid()) {
		float a = color.alphaF();
		float r = color.redF();
		float g = color.greenF();
		float b = color.blueF();
		setColor(r, g, b, a);
	}
}

void Renderable::OnBrowseClick() {
	QString filename;

//	filename = QFileDialog::getOpenFileName(this, tr("Select PNG Image"), getAssets()->getCurrentProject()->getLocation(), tr("Image Files (*.png)"));

	if(filename.isEmpty()) return;

	QDir projectDir(getAssets()->getCurrentProject()->getLocation());
	QString relativePath = projectDir.relativeFilePath(filename);

	if(relativePath.contains("..")) {
    //	QMessageBox::critical(this, tr("Warning!"), tr("Image file is not located within the project directory! It will now be copied into the current Area directory."));

		QFile imgFile(filename);
		QString newLocation = getAssets()->getCurrentArea()->getLocation();
		newLocation.append("/");
		QStringList splitList = filename.split("/");
		newLocation.append(splitList.last());

		if(!imgFile.copy(newLocation)) {
    //		QMessageBox::critical(this, tr("Error!"), tr("Image file could not be copied to area directory! A file with that name already exists!"));
		}

		relativePath = projectDir.relativeFilePath(newLocation);
	}

	unsigned width, height;
	TileSheetUtilities::GetImageSize(relativePath, width, height);

	_texEdit->setText(relativePath);
	_shWEdit->setText(QString::number(width));
	_shHEdit->setText(QString::number(height));
}
#endif


void Renderable::InitializeUI(void) {
	_mainLayout = new QVBoxLayout;
	_colorLayout = new QGridLayout;
	_texLayout = new QGridLayout;

	_colorLabel = new QLabel("Color");
	_rLabel = new QLabel("r");
	_gLabel = new QLabel("g");
	_bLabel = new QLabel("b");
	_aLabel = new QLabel("a");
	_rEdit = new QLineEdit;
	_gEdit = new QLineEdit;
	_bEdit = new QLineEdit;
	_aEdit = new QLineEdit;
	_swatchBut = new QPushButton("swatch");

	_colorLayout->addWidget(_colorLabel, 0, 0);
	_colorLayout->addWidget(_rLabel, 0, 1);
	_colorLayout->addWidget(_gLabel, 0, 2);
	_colorLayout->addWidget(_bLabel, 0, 3);
	_colorLayout->addWidget(_aLabel, 0, 4);
	_colorLayout->addWidget(_swatchBut, 1, 0);
	_colorLayout->addWidget(_rEdit, 1, 1);
	_colorLayout->addWidget(_gEdit, 1, 2);
	_colorLayout->addWidget(_bEdit, 1, 3);
	_colorLayout->addWidget(_aEdit, 1, 4);

	_texLabel = new QLabel("Texture");
	_sizeLabel = new QLabel("size");
	_widthLabel = new QLabel("width");
	_heightLabel = new QLabel("height");
	_spriteLabel = new QLabel("sprite");
	_sheetLabel = new QLabel("sheet");
	_frameLabel = new QLabel("frame");

	_texEdit = new QLineEdit;
	_shWEdit = new QLineEdit;
	_shHEdit = new QLineEdit;
	_shWEdit->setEnabled(false);
	_shHEdit->setEnabled(false);
	_sprWEdit = new QLineEdit;
	_sprHEdit = new QLineEdit;
	_frameEdit = new QLineEdit;
	_browseBut = new QPushButton("browse");
	_texCheckBox = new QCheckBox;
	_reloadBut = new QPushButton("reload texture");

	_texLayout->addWidget(_texLabel, 0, 0, 1, 2);
	_texLayout->addWidget(_texCheckBox, 0, 2);
	_texLayout->addWidget(_texEdit, 1, 0, 1, 2);
	_texLayout->addWidget(_browseBut, 1, 2);
	_texLayout->addWidget(_sizeLabel, 2, 0);
	_texLayout->addWidget(_widthLabel, 2, 1);
	_texLayout->addWidget(_heightLabel, 2, 2);
	_texLayout->addWidget(_sheetLabel, 3, 0);
	_texLayout->addWidget(_shWEdit, 3, 1);
	_texLayout->addWidget(_shHEdit, 3, 2);
	_texLayout->addWidget(_spriteLabel, 4, 0);
	_texLayout->addWidget(_sprWEdit, 4, 1);
	_texLayout->addWidget(_sprHEdit, 4, 2);
	_texLayout->addWidget(_frameLabel, 5, 0);
	_texLayout->addWidget(_frameEdit, 5, 1);
	_texLayout->addWidget(_reloadBut, 6, 0, 1, 3);

	_mainLayout->addLayout(_colorLayout);
	_mainLayout->addLayout(_texLayout);
	_mainLayout->insertStretch(-1);
    //setLayout(_mainLayout);
}

void Renderable::PopulateUI(void) {
#if 0
	GYColor4 color;
	_elysianPrim.elysianRect->getColor(color);
	//else _elysianPrim.elysianSprite->getColor(color);
	setColor(color.r, color.g, color.b, color.a);

        if(getPrimType() == elysian::Primitive::SPRITE) {
                unsigned width, height;
                _texEdit->setText(_elysianPrim.elysianSprite->getTexPath());
		_elysianPrim.elysianSprite->getSheetSize(width, height);
                _shWEdit->setText(QString::number(width));
                _shHEdit->setText(QString::number(height));
		_elysianPrim.elysianSprite->getSpriteSize(width, height);
                _sprWEdit->setText(QString::number(width));
                _sprHEdit->setText(QString::number(height));
                _frameEdit->setText(QString::number(_elysianPrim.elysianSprite->getFrame()));
                _texCheckBox->setChecked(true);
                OnSetTextured(true);
	}
        else OnSetTextured(false);
#endif
}

Renderable::Renderable(elysian::Renderable *const /*comp*/):  _sheet(NULL) {
#if 0
	InitializeUI();

	switch(getPrimType()) {
        //case elysian::Primitive::SPRITE:
	case elysian::Primitive::NONE: //Make it a Rect
//		comp->setPrimitive(new elysian::RectPrim);
		//PURPOSE FALLTHROUGH
	case elysian::Primitive::RECT:
//		_elysianPrim.elysianRect = (elysian::RectPrim*)comp->getPrimitive();
		break;
        case elysian::Primitive::SPRITE:
//		_elysianPrim.elysianSprite = (elysian::SpritePrim*)comp->getPrimitive();
                break;
	default:
		//UNSUPPORTED PRIMITIVE TYPE
        Q_ASSERT(0);
	}
#endif
	//Connect sigs and slawts
/*	connect(_rEdit, SIGNAL(editingFinished()), this, SLOT(OnSetColorR()));
	connect(_gEdit, SIGNAL(editingFinished()), this, SLOT(OnSetColorG()));
	connect(_bEdit, SIGNAL(editingFinished()), this, SLOT(OnSetColorB()));
	connect(_aEdit, SIGNAL(editingFinished()), this, SLOT(OnSetColorA()));
	connect(_texCheckBox, SIGNAL(toggled(bool)), this, SLOT(OnSetTextured(bool)));
	connect(_texEdit, SIGNAL(editingFinished()), this, SLOT(OnSetTexturedPath()));
	connect(_sprWEdit, SIGNAL(editingFinished()), this, SLOT(OnSetSpriteWidth()));
	connect(_sprHEdit, SIGNAL(editingFinished()), this, SLOT(OnSetSpriteHeight()));
	connect(_frameEdit, SIGNAL(editingFinished()), this, SLOT(OnSetFrame()));
	connect(_swatchBut, SIGNAL(clicked()), this, SLOT(OnSwatchClick()));
	connect(_browseBut, SIGNAL(clicked()), this, SLOT(OnBrowseClick()));
    connect(_reloadBut, SIGNAL(clicked()), this, SLOT(OnReloadClick())); */

	PopulateUI();
}
/*
void Renderable::OnReloadClick() {
	ReloadSpriteSheet();
}*/

void Renderable::setColor(float r, float g, float b, float a) {
	QString aStr = QString::number(a, 'g', 4);
	QString rStr = QString::number(r, 'g', 4);
	QString gStr = QString::number(g, 'g', 4);
	QString bStr = QString::number(b, 'g', 4);

//	_elysianPrim.elysianRect->setColor(r, g, b, a);
	if(_aEdit->text() != aStr) _aEdit->setText(aStr);
	if(_rEdit->text() != rStr) _rEdit->setText(rStr);
	if(_gEdit->text() != gStr) _gEdit->setText(gStr);
	if(_bEdit->text() != bStr) _bEdit->setText(bStr);
    //Q_EMIT PrimitiveUpdate(); //Signal the entity to update its geometry
}
