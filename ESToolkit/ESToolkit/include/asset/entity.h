#ifndef ENTITY_H
#define ENTITY_H

#include <QGraphicsRectItem>
#include "treenode.h"
#include "assetManager.h"

class QPainter;
class QToolBox;
class QLabel;
class QLineEdit;
class QCheckBox;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QGridLayout;
class QColorDialog;
class QFileDialog;
class QDirIterator;
class QFile;
class QMessageBox;

class Sprite;
class Component;
class Renderable;
class Collidable;
class Animation;
class Warp;
class Item;
class Character;
class Friendly;
class Enemy;

namespace elysian {
	class Entity;
}

class Entity: public QWidget, public TreeNode, public QGraphicsRectItem, public AssetManager {
    friend class ESToolkit;
    Q_OBJECT
protected:
    //encapsulated (although memory is still managed by elysian::Area)
	elysian::Entity *_elysianEntity;
private:

    /* COMPONENTS */
	Renderable *_renderable;
	Animation *_animation;
    Collidable *_collidable;
    Warp *_warp;
    Item *_item;
    Character *_character;
    Friendly *_friendly;
    Enemy *_enemy;

	//QList<Behavior *> _behavior;

    /*
	  WIDGETS for BASE ENTITY attributes:
	  name, transform
	  PROBABLY want to redo this in designer?
    */
	QToolBox *_compToolBox;
	QWidget *_entityTab;
	QVBoxLayout *_mainLayout, *_entityBaseLayout;
	QGridLayout *_entityLayout;
	QLabel *_nameLabel, *_transLabel, *_posLabel, *_sizeLabel, *_scaleLabel, *_orientLabel;
    QLabel *_xLabel, *_yLabel, *_zLabel;
	QLineEdit *_nameEdit, *_posXEdit, *_posYEdit, *_posZEdit, *_sizeXEdit, *_sizeYEdit,
        *_scaleXEdit, *_scaleYEdit, *_orientZEdit;
    QCheckBox *_snapsToGrid;

    bool _gridSnap;
	void InitializeUI(void);
	void PopulateWidgets(void);

public slots:
	void OnNameChange(void);
	void OnPosXChange(void);
	void OnPosYChange(void);
	void OnPosZChange(void);
	void OnSizeXChange(void);
	void OnSizeYChange(void);
	void OnScaleXChange(void);
	void OnScaleYChange(void);
	void OnOrientZChange(void);
    void OnGridSnapChange(int state);
	virtual void UpdateRect(void);


	/*
    void OnTextureLocChange();
    void OnSheetWidthChange();
    void OnSheetHeightChange();
    void OnSpriteWidthChange();
    void OnSpriteHeightChange();
    void OnStartFrameChange();
    void OnClickSwatch1();
    void OnClickSwatch2();
    void OnClickSwatch3();
    void OnClickSwatch4();
    void OnColor1Change();
    void OnColor2Change();
    void OnColor3Change();
    void OnColor4Change();
    void OnClickBrowseSheet();
	void OnReloadSheet();*/


public:
	Entity(elysian::Entity &parent);
	~Entity(void);

	//Selectable
	virtual Selectable::TYPE getType(void) const { return Selectable::ENTITY; }
	virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_ENTITY; }

	//Should "technically" be decoupled. See .cpp for comment.
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual bool activate(void);
	virtual bool deactivate(void);

	void setPos(float x, float y);
	void setPos(float x, float y, float z);
	void setPosZ(float z);
    void setSize(float x, float y);
    void setScale(float x, float y);
	void setOrientZ(float angle);

	bool AttachRenderable(void);
	bool AttachAnimation(void);
	bool AttachCollidable(void);
	bool AttachWarp(void);
	bool AttachItem(void);
	bool AttachCharacter(void);
	bool AttachFriendly(void);
	bool AttachEnemy(void);
	bool AttachConversation(void);

	bool RemoveRenderable(void);
	bool RemoveAnimation(void);
	bool RemoveCollidable(void);
	bool RemoveWarp(void);
	bool RemoveItem(void);
	bool RemoveCharacter(void);
	bool RemoveFriendly(void);
	bool RemoveEnemy(void);
	bool RemoveConversation(void);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);

	elysian::Entity *getElysianEntity(void) const;
	Renderable *getRenderable(void) const;
	Animation *getAnimation(void) const;
	Collidable *getCollidable(void) const;
	Warp *getWarp(void) const;
	Item *getItem(void) const;
	Character *getCharacter(void) const;
	Friendly *getFriendly(void) const;
	Enemy *getEnemy(void) const;

signals:
    void ComponentAttached(Entity *const entity, Component *const comp);
    void ComponentRemoved(Entity *const entity, Component *const comp);


};

#endif // ENTITY_H
