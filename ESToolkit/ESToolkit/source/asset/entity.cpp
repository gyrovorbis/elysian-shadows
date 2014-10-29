#include <QPainter>
#include <QToolBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include <QColorDialog>
#include <QFileDialog>
#include <QDirIterator>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>

#include "entity.h"
#include "elysian_entity.hpp"
#include "area.h"
#include "debug.h"
#include "sprite.h"
#include "component.h"
#include "assets.h"
#include "project.h"
#include "animation.h"
#include "collidable.h"
#include "character.h"
#include "warp.h"
#include "renderable.h"
#include <cmath>

TkEntity::TkEntity(elysian::Entity &entity): _elysianEntity(&entity),
	_renderable(NULL), _animation(NULL), _collidable(NULL),_warp(NULL),_item(NULL), _character(NULL), _friendly(NULL), _enemy(NULL),
    _gridSnap(false) {	//generate toolkit components from elysian components
#if 0
	if(_elysianEntity->getRenderable()) {
		_renderable = new Renderable(_elysianEntity->getRenderable());
		QObject::connect(_renderable, SIGNAL(PrimitiveUpdate()), this, SLOT(UpdateRect()));
	}
    if(_elysianEntity->getAnimation()) _animation = new Animation(_elysianEntity->getAnimation());
    if(_elysianEntity->getCollidable()) _collidable = new Collidable(_elysianEntity->getCollidable());

    if(_elysianEntity->getCharacter()) _character = new Character(_elysianEntity->getCharacter());
//    if(_elysianEntity->getFriendly()) _friendly = new Friendly(_elysianEntity->getFriendly());
//    if(_elysianEntity->getEnemy()) _enemy = new Enemy(_elysianEntity->getEnemy());
//    if(_elysianEntity->getItem()) _item = new Item(_elysianEntity->getItem());
    if(_elysianEntity->getWarp()) _warp = new Warp(_elysianEntity->getWarp());

	InitializeUI();

    //init TreeNode and QGraphicsRect data
    setName(entity.getName());
	_nameEdit->setText(entity.getName());
	QGraphicsRectItem::setPos(entity.transform._pos.x, entity.transform._pos.y);
	setZValue(entity.transform._pos.z);

	setPos(_elysianEntity->transform._pos.x, _elysianEntity->transform._pos.y);
	setPosZ(_elysianEntity->transform._pos.z);
	setSize(_elysianEntity->transform._size.x, _elysianEntity->transform._size.y);
	setScale(_elysianEntity->transform._scale.y, _elysianEntity->transform._scale.y);
	setOrientZ(_elysianEntity->transform.getOrientZ()*(180.0f/M_PI));

    //connect signals and slawts
    connect(_nameEdit, SIGNAL(editingFinished()), this, SLOT(OnNameChange()));
    connect(_posXEdit, SIGNAL(editingFinished()), this, SLOT(OnPosXChange()));
    connect(_posYEdit, SIGNAL(editingFinished()), this, SLOT(OnPosYChange()));
	connect(_posZEdit, SIGNAL(editingFinished()), this, SLOT(OnPosZChange()));
    connect(_sizeXEdit, SIGNAL(editingFinished()), this, SLOT(OnSizeXChange()));
    connect(_sizeYEdit, SIGNAL(editingFinished()), this, SLOT(OnSizeYChange()));
    connect(_scaleXEdit, SIGNAL(editingFinished()), this, SLOT(OnScaleXChange()));
    connect(_scaleYEdit, SIGNAL(editingFinished()), this, SLOT(OnScaleYChange()));
	connect(_orientZEdit, SIGNAL(editingFinished()), this, SLOT(OnOrientZChange()));
    connect(_snapsToGrid,SIGNAL(stateChanged(int)), this, SLOT(OnGridSnapChange(int)));

    //teh flags
    setFlag(ItemSendsScenePositionChanges,true);
    setFlag(ItemSendsGeometryChanges,true) ;
    setFlag(ItemIsFocusable,true);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemIsMovable, true);
    setFlag(ItemIgnoresParentOpacity,true);

    UpdateRect();
#endif
}


#if 0
QVariant Entity::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
         QPointF newPos = value.toPointF();


         if(_gridSnap) {
             qDebug() << "Pos snapping";
             int snap_x = ((newPos.x()+16) / 32);
                snap_x*=32;
                snap_x-=16; //engine offset
             int snap_y = ((newPos.y()+16) / 32);
                snap_y*=32;
                snap_y-=16; //engine offset

             newPos.setX(snap_x);
             newPos.setY(snap_y);
         }
        // value is the new position.

//        QRectF rect = scene()->sceneRect();
//        if (!rect.contains(newPos)) {
//            // Keep the item inside the scene rect.
//            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
//            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));

//            return newPos;
//        }
        return newPos;
    } else if (change == ItemPositionHasChanged && scene()) {
        QPointF newPos = value.toPointF();

        setPos(newPos.x(),newPos.y());
        //setRect(newPos.x(),newPos.y(),)

        //newPos += transformOriginPoint();
       //         qDebug() << "value in ItemPositionHasChange " << newPos;
       // QPointF newPos2 = mapToScene(newPos);
       //newPos.setX(newPos.x() - (width() * 0.5f));
       //newPos.setY(newPos.y() - (height() * 0.5f));

        //QString xStr = QString::number(newPos.x() , 'g', 4);
        //QString yStr = QString::number(newPos.y() , 'g', 4);

        //if(_posXEdit->text() != xStr) _posXEdit->setText(xStr);
       // if(_posYEdit->text() != yStr) _posYEdit->setText(yStr);

        //setTransformOriginPoint(newPos);
        //QGraphicsRectItem::setPos(0.0f, 0.0f);

       // UpdateRect();
    } else if(change == ItemTransformHasChanged && scene() ) {
       // UpdateRect();
    }

    return QGraphicsItem::itemChange(change, value);
}

void Entity::keyPressEvent(QKeyEvent *event){
    qDebug() << "Entity " << getName() << "key press event recieved.";
    if(event->key() == Qt::LeftArrow) {
        setRotation(rotation() - 5); //cc
        qDebug() <<"Entity KeyPressEvent :. LeftArrow";
    } else if(event->key() == Qt::RightArrow) {
        setRotation(rotation() +5); //c
        qDebug() <<"Entity KeyPressEvent :. LeftArrow";

    }
}
void Entity::focusInEvent(QFocusEvent* /*event*/) {
    qDebug() << "Entity " << getName() ;

}

void Entity::InitializeUI(void) {
	_mainLayout = new QVBoxLayout;

	_compToolBox = new QToolBox;

	_entityTab = new QWidget;
	_entityBaseLayout = new QVBoxLayout;
	_entityLayout = new QGridLayout;

	_nameLabel = new QLabel("Name");
	_transLabel = new QLabel("Xform");
	_posLabel = new QLabel("pos");
	_sizeLabel = new QLabel("size");
	_scaleLabel = new QLabel("scale");
	_orientLabel = new QLabel("orient");
	_xLabel = new QLabel("x");
	_yLabel = new QLabel("y");
	_zLabel = new QLabel("z");
	_nameEdit = new QLineEdit;
	_posXEdit = new QLineEdit;
	_posYEdit = new QLineEdit;
	_posZEdit = new QLineEdit;
	_sizeXEdit = new QLineEdit;
	_sizeYEdit = new QLineEdit;
	_scaleXEdit = new QLineEdit;
	_scaleYEdit = new QLineEdit;
	_orientZEdit = new QLineEdit;
    _snapsToGrid = new QCheckBox;
        _snapsToGrid->setText("Snaps To Grid");
        _snapsToGrid->setChecked(false);

	_entityLayout->addWidget(_nameLabel, 0, 0);
	_entityLayout->addWidget(_nameEdit, 0, 1, 1, 3);
	_entityLayout->addWidget(_transLabel, 1, 0);
	_entityLayout->addWidget(_xLabel, 1, 1);
	_entityLayout->addWidget(_yLabel, 1, 2);
	_entityLayout->addWidget(_zLabel, 1, 3);
	_entityLayout->addWidget(_posLabel, 2, 0);
	_entityLayout->addWidget(_posXEdit, 2, 1);
	_entityLayout->addWidget(_posYEdit, 2, 2);
	_entityLayout->addWidget(_posZEdit, 2, 3);
	_entityLayout->addWidget(_sizeLabel, 3, 0);
	_entityLayout->addWidget(_sizeXEdit, 3, 1);
	_entityLayout->addWidget(_sizeYEdit, 3, 2);
	_entityLayout->addWidget(_scaleLabel, 4, 0);
	_entityLayout->addWidget(_scaleXEdit, 4, 1);
	_entityLayout->addWidget(_scaleYEdit, 4, 2);
	_entityLayout->addWidget(_orientLabel, 5, 0);
	_entityLayout->addWidget(_orientZEdit, 5, 3);
    _entityLayout->addWidget(_snapsToGrid, 6,0);

	_entityBaseLayout->addLayout(_entityLayout);
	_entityBaseLayout->addStretch(-1);
	_entityTab->setLayout(_entityBaseLayout);
	_compToolBox->addItem(_entityTab, "Entity");

	//add components (if they exist) to ToolBox
	if(_renderable) _compToolBox->addItem(_renderable, "Renderable");
	if(_animation) _compToolBox->addItem(_animation, "Animation");
	if(_collidable) _compToolBox->addItem(_collidable, "Collidable");
	if(_character) _compToolBox->addItem(_character, "Character");
//	if(_friendly) _compToolBox->addItem(_friendly, "Friendly");
//	if(_enemy) _compToolBox->addItem(_enemy, "Enemy");
//	if(_item) _compToolBox->addItem(_item, "Item");
//	if(_warp) _compToolBox->addItem(_warp, "Warp");

	_mainLayout->addWidget(_compToolBox);
	//_mainLayout->addStretch(-1);
	setLayout(_mainLayout);
}

void Entity::PopulateWidgets() {

}

/*
  Should technically be decoupled and encapsulated within each renderable type
  like in the engine, but we still need to render nonrenderables in the Toolkit,
  so I suppose this is acceptable. --FGirgis
 */
void Entity::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/) {

//This all got completely raped with the Renderable rewrite!!!
#if 0
	if(_renderable && _renderable->getPrimType() == elysian::Primitive::SPRITE) {
		QPixmap *pixmap = _renderable->getCurrentFramePixmap();

		if(pixmap) {
			unsigned width, height;

			_renderable->getSpriteSize(width, height);
			painter->drawPixmap(QGraphicsRectItem::rect(), *pixmap, QRect(0, 0, width, height));
		}
		else {
			//RED ERROR RECT
			QBrush brush(QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f));
			painter->setBrush(brush);
			painter->drawRect(QGraphicsRectItem::rect()); }
	}
	else if(_renderable && _renderable->getPrimType() == elysian::Primitive::RECT) {
		GYColor4 color =_renderable->getColor();

		QBrush brush(QColor::fromRgbF(color.r, color.g, color.b, color.a));
		painter->setBrush(brush);
		painter->drawRect(QGraphicsRectItem::rect());
	}
    else {
#endif
		//_elysianEntity->getColorByIndex(0, a, r, g, b);
		QBrush brush(QColor::fromRgbF(0.5f, 0.5f, 0.5f, 0.5f));
        painter->setBrush(brush);
        painter->drawRect(QGraphicsRectItem::rect());
#if 0
    }
#endif
}

bool Entity::activate(void) { return true; }

bool Entity::deactivate(void) { return true; }



//====SLAWTS====
void Entity::OnNameChange() {
    setName(_nameEdit->text());
    _elysianEntity->setName(_nameEdit->text().toStdString().c_str());
}

void Entity::OnPosXChange() {
	setPos(_posXEdit->text().toFloat(), _elysianEntity->transform._pos.y);
}

void Entity::OnPosYChange() {
	setPos(_elysianEntity->transform._pos.x, _posYEdit->text().toFloat());
}

void Entity::OnPosZChange() {
	setPosZ(_posZEdit->text().toFloat());
}

void Entity::OnSizeXChange() {
	setSize(_sizeXEdit->text().toFloat(), _elysianEntity->transform._size.y);
}

void Entity::OnSizeYChange() {
	setSize(_elysianEntity->transform._size.x, _sizeYEdit->text().toFloat());
}

void Entity::OnScaleXChange() {
	setScale(_scaleXEdit->text().toFloat(), _elysianEntity->transform._scale.y);
}

void Entity::OnScaleYChange() {
	setScale(_elysianEntity->transform._scale.x, _scaleYEdit->text().toFloat());
}

void Entity::OnOrientZChange() {
	setOrientZ(_orientZEdit->text().toFloat());
}
void Entity::OnGridSnapChange(int state) {
    switch(state) {
    case Qt::Checked:
        _gridSnap=true;
        QGraphicsItem::setPos(QGraphicsRectItem::pos()); //apply gridsnap immediately
        UpdateRect();
        break;
    case Qt::Unchecked:
        _gridSnap=false;
        break;
    default:
        break;
    }
}

void Entity::setPos(float x, float y) {
    QString xStr = QString::number(x, 'g', 4);
    QString yStr = QString::number(y, 'g', 4);

	_elysianEntity->transform.setPos(x, y);
    if(_posXEdit->text() != xStr) _posXEdit->setText(xStr);
    if(_posYEdit->text() != yStr) _posYEdit->setText(yStr);
    UpdateRect();
}

void Entity::setPosZ(float z) {
    QString zStr = QString::number(z, 'g', 4);

	_elysianEntity->transform.setPosZ(z);
	setZValue(z);
	if(_posZEdit->text() != zStr) _posZEdit->setText(zStr);
    UpdateRect();
}

void Entity::setSize(float x, float y) {
    QString xStr = QString::number(x, 'g', 4);
    QString yStr = QString::number(y, 'g', 4);

    _elysianEntity->transform.setSize(x, y);
    if(_sizeXEdit->text() != xStr) _sizeXEdit->setText(xStr);
    if(_sizeYEdit->text() != yStr) _sizeYEdit->setText(yStr);
    UpdateRect();
}

void Entity::setScale(float x, float y) {
    QString xStr = QString::number(x, 'g', 4);
    QString yStr = QString::number(y, 'g', 4);

   _elysianEntity->transform.setScale(x, y);
   if(_scaleXEdit->text() != xStr) _scaleXEdit->setText(xStr);
   if(_scaleYEdit->text() != yStr) _scaleYEdit->setText(yStr);
   UpdateRect();
}

void Entity::setOrientZ(float angle) {
    QString angStr = QString::number(angle, 'g', 4);

	_elysianEntity->transform.setOrientZ(angle*(M_PI/180.0f));
	if(_orientZEdit->text() != angStr) _orientZEdit->setText(angStr);
    UpdateRect();
}

void Entity::UpdateRect() {

    GYVector2 pos;// = _elysianEntity->transform.getPosition();
    GYVector2 size;// = _elysianEntity->transform.getSize();
    GYVector2 scale;// = _elysianEntity->transform.getScale();
	_elysianEntity->transform.getPos(pos);
    _elysianEntity->transform.getSize(size);
    _elysianEntity->transform.getScale(scale);

    prepareGeometryChange();

    //setZValue(_elysianEntity->transform.getPosZ());

    //size *= scale;
    gyVec2CompProdAccum(&size, &scale);

    //pos -= size * 0.5f;
    pos.x -= size.x * 0.5f;
    pos.y -= size.y * 0.5f;

    setRect(0, 0, size.x, size.y);
   // setRect(_elysianEntity->transform.getPosition().x, _elysianEntity->transform.getPosition().y, size.x, size.y);
    setRotation(_elysianEntity->transform.getOrientZ()*(180.0f/M_PI));
   // setTransformOriginPoint(_elysianEntity->transform._pos.x, _elysianEntity->transform._pos.y);
   // setPos(_elysianEntity->transform.getPosition().x, _elysianEntity->transform.getPosition().y);
    //QGraphicsRectItem::setPos(0.0f, 0.0f);
    QGraphicsRectItem::update();
}

bool Entity::AttachRenderable() {
	elysian::Renderable *comp;

	if(_renderable) {
		QMessageBox::critical(this, tr("Error!"), tr("Renderable component already attached!"));
		return false;
	}
	else if((comp = _elysianEntity->addRenderable())) {
		_renderable = new Renderable(comp); //new Animation(_elysianEntity->getAnimation());
		connect(_renderable, SIGNAL(PrimitiveUpdate()), this, SLOT(UpdateRect()));
		_compToolBox->addItem(_renderable, "Renderable");
		emit ComponentAttached(this, _renderable);
		UpdateRect();
		return true;
	}
	else {
		 QMessageBox::critical(this, tr("Error!"), tr("Renderable could not be attached to the current entity!\n"));
		return false;
	}
}

bool Entity::AttachAnimation() {
	elysian::Animation *comp;

    if(_animation) {
        QMessageBox::critical(this, tr("Error!"), tr("Animation component already attached!"));
        return false;
    }
	else if((comp = _elysianEntity->addAnimation())) {
        _animation = new Animation(comp); //new Animation(_elysianEntity->getAnimation());
        _compToolBox->addItem(_animation, "Animation");
        emit ComponentAttached(this, _animation);
        return true;
    }
    else {
         QMessageBox::critical(this, tr("Error!"), tr("Animation could not be attached to the current entity!\n"));
        return false;
    }
}

bool Entity::AttachCollidable() {
	elysian::Collidable *comp;

    if(_collidable) {
        QMessageBox::critical(this, tr("Error!"), tr("Collidable component already attached!"));
        return false;
    }
	else if((comp = _elysianEntity->addCollidable())) {
        _collidable = new Collidable(comp); //new Animation(_elysianEntity->getAnimation());
        _compToolBox->addItem(_collidable, "Collidable");
        emit ComponentAttached(this, _collidable);
        return true;
    }
    else {
         QMessageBox::critical(this, tr("Error!"), tr("Collidable could not be attached to the current entity!"));
        return false;
    }
}

bool Entity::AttachCharacter() {
	elysian::Character *comp;

    if(_character) {
        QMessageBox::critical(this, tr("Error!"), tr("Character component already attached!"));
        return false;
    }
	else if((comp = _elysianEntity->addCharacter())) {
        _character = new Character(comp); //new Animation(_elysianEntity->getAnimation());
        _compToolBox->addItem(_character, "Character");
        emit ComponentAttached(this, _character);
        return true;
    }
    else {
         QMessageBox::critical(this, tr("Error!"), tr("Character could not be attached to the current entity! Requires both Animation and Collidable components first!\n"));
        return false;
    }
}

bool Entity::AttachWarp() {
	elysian::Warp *comp;

    if(_warp) {
        QMessageBox::critical(this, tr("Error!"), tr("Warp component already attached!"));
        return false;
    }
	else if((comp = _elysianEntity->addWarp())) {
        _warp = new Warp(comp); //new Animation(_elysianEntity->getAnimation());
        _compToolBox->addItem(_warp, "Warp");
        emit ComponentAttached(this, _warp);
        return true;
    }
    else {
         QMessageBox::critical(this, tr("Error!"), tr("Warp could not be attached to the current entity! Requires Collidable first!"));
        return false;
    }
}

#if 0
bool Entity::AttachItem() {
	elysian::Item *comp;

    if(_item) {
        QMessageBox::critical(this, tr("Error!"), tr("Item component already attached!"));
        return false;
    }
	else if((comp = _elysianEntity->addItem())) {
        _item = new Item(comp); //new Animation(_elysianEntity->getAnimation());
        _compToolBox->addItem(_item, "Item");
        emit ComponentAttached(this, _item);
        return true;
    }
    else {
         QMessageBox::critical(this, tr("Error!"), tr("Item could not be attached! Requires Animation and Collidable first!"));
        return false;
    }
}

bool Entity::AttachFriendly() {
	elysian::Friendly *comp;

    if(_friendly) {
        QMessageBox::critical(this, tr("Error!"), tr("Friendly component already attached!"));
        return false;
    }
	else if((comp = _elysianEntity->addFriendly())) {
        _friendly = new Friendly(comp); //new Animation(_elysianEntity->getAnimation());
        _compToolBox->addItem(_friendly, "Friendly");
        emit ComponentAttached(this, _friendly);
        return true;
    }
    else {
         QMessageBox::critical(this, tr("Error!"), tr("Friendly could not be attached to the current entity! Requires Character first!"));
        return false;
    }
}

bool Entity::AttachEnemy() {
	elysian::Enemy *comp;

    if(_enemy) {
        QMessageBox::critical(this, tr("Error!"), tr("Enemy component already attached!"));
        return false;
    }
	else if((comp = _elysianEntity->addEnemy())) {
        _enemy = new Enemy(comp); //new Animation(_elysianEntity->getAnimation());
        _compToolBox->addItem(_enemy, "Enemy");
        emit ComponentAttached(this, _enemy);
        return true;
    }
    else {
         QMessageBox::critical(this, tr("Error!"), tr("Enemy could not be attached to the current entity! Requires Character first!"));
        return false;
    }
}
#endif

bool Entity::RemoveRenderable() {
	if(!_renderable) {
		QMessageBox::critical(this, tr("Error!"), tr("Renderable component isn't attached!"));
		return false;
	}
	_compToolBox->removeItem(_compToolBox->indexOf(_renderable));

	Component *temp = _renderable;
	_renderable = NULL;
	emit ComponentRemoved(this, temp);
	_elysianEntity->removeRenderable();
	delete temp;
	UpdateRect();
	return true;
}


bool Entity::RemoveAnimation() {
    if(!_animation) {
        QMessageBox::critical(this, tr("Error!"), tr("Animation component isn't attached!"));
        return false;
    }
    _compToolBox->removeItem(_compToolBox->indexOf(_animation));

    Component *temp = _animation;
    _animation = NULL;
    emit ComponentRemoved(this, temp);
	_elysianEntity->removeAnimation();
    delete temp;
    return true;
}

bool Entity::RemoveCollidable() {
    if(!_collidable) {
        QMessageBox::critical(this, tr("Error!"), tr("Collidable component isn't attached!"));
        return false;
    }
    _compToolBox->removeItem(_compToolBox->indexOf(_collidable));
    Component *temp = _collidable;
    _collidable = NULL;
    emit ComponentRemoved(this, temp);
	_elysianEntity->removeCollidable();
    delete temp;
    return true;
}

bool Entity::RemoveWarp() {
    if(!_warp) {
        QMessageBox::critical(this, tr("Error!"), tr("Warp component isn't attached!"));
        return false;
    }
    _compToolBox->removeItem(_compToolBox->indexOf(_warp));
    Component *temp = _warp;
    _warp = NULL;
    emit ComponentRemoved(this, temp);
	_elysianEntity->removeWarp();
    delete temp;
    return true;
}

bool Entity::RemoveCharacter() {
    if(!_character) {
        QMessageBox::critical(this, tr("Error!"), tr("Character component isn't attached!"));
        return false;
    }
    _compToolBox->removeItem(_compToolBox->indexOf(_character));
    Component *temp = _character;
    _character = NULL;
    emit ComponentRemoved(this, temp);
	_elysianEntity->removeCharacter();
    delete temp;
    _character = NULL;
    return true;
}
#if 0
bool Entity::RemoveFriendly() {
    if(!_friendly) {
        QMessageBox::critical(this, tr("Error!"), tr("Friendly component isn't attached!"));
        return false;
    }
    _compToolBox->removeItem(_compToolBox->indexOf(_friendly));
    Component *temp = _friendly;
    _friendly = NULL;
    emit ComponentRemoved(this, temp);
	_elysianEntity->removeFriendly();
    delete temp;
    return true;
}

bool Entity::RemoveEnemy() {
    if(!_enemy) {
        QMessageBox::critical(this, tr("Error!"), tr("Enemy component isn't attached!"));
        return false;
    }
    _compToolBox->removeItem(_compToolBox->indexOf(_enemy));
    Component *temp = _enemy;
    _enemy = NULL;
    emit ComponentRemoved(this, temp);
	_elysianEntity->removeEnemy();
    delete temp;
    return true;
}

bool Entity::RemoveItem() {
    if(!_item) {
        QMessageBox::critical(this, tr("Error!"), tr("Item component isn't attached!"));
        return false;
    }
    _compToolBox->removeItem(_compToolBox->indexOf(_item));
    Component *temp = _item;
    _item = NULL;
    emit ComponentRemoved(this, temp);
    _elysianEntity->removeItem();
    delete temp;
    return true;
}

#endif

Renderable *Entity::getRenderable() const { return _renderable; }
Animation *Entity::getAnimation() const { return _animation; }
Collidable *Entity::getCollidable() const { return _collidable; }
Item *Entity::getItem() const { return _item; }
Warp *Entity::getWarp() const { return _warp; }
Character *Entity::getCharacter() const { return _character; }
Friendly *Entity::getFriendly() const { return _friendly; }
Enemy *Entity::getEnemy() const { return _enemy; }


#endif

