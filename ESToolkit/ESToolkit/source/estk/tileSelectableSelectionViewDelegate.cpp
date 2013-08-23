#include <QComboBox>
#include <QCheckBox>
#include "tileSelectableSelectionViewDelegate.h"
#include "assets.h"
#include "level.h"
#include "area.h"
#include "propertyStandardItem.h"

//===== STATIC MEMBERS =====

QIcon **ElysianTileWrapper::_icons = NULL;

void ElysianTileWrapper::allocIcons(void) {
	if(_icons) return;

	//ALLOCATE THE BASTARDS
	_icons = new QIcon*[elysian::Tile::COLLIDER_MAX];
	for(unsigned i = 0; i < elysian::Tile::COLLIDER_MAX; ++i)
		_icons[i] = new QIcon(_colliderIconNameFromEnum[i]);
}

//Don't forget this table has to be manually updated every time elysian::Tile::COLLIDER_TYPE changes, fuckmite!!!
const char ElysianTileWrapper::_colliderStringFromEnum[elysian::Tile::COLLIDER_MAX][100] =
{
	"COLLIDER_NONE", "COLLIDER_FULL",
	"COLLIDER_HALF_E", "COLLIDER_HALF_N", "COLLIDER_HALF_W", "COLLIDER_HALF_S",
	"COLLIDER_QUAD_SE", "COLLIDER_QUAD_NE", "COLLIDER_QUAD_NW", "COLLIDER_QUAD_SW",
	"COLLIDER_L_SE", "COLLIDER_L_NE", "COLLIDER_L_NW", "COLLIDER_L_SW",
	"COLLIDER_QUADS_SE_NW", "COLLIDER_QUADS_SW_NE",
	"COLLIDER_45DEG_SE", "COLLIDER_45DEG_NE", "COLLIDER_45DEG_NW", "COLLIDER_45DEG_SW"
};

const char ElysianTileWrapper::_colliderIconNameFromEnum[elysian::Tile::COLLIDER_MAX][100] =
{
	":/colliders/images/colliderNone.png", ":/colliders/images/colliderFull.png",
	":/colliders/images/colliderHalfEast.png", ":/colliders/images/colliderHalfNorth.png", ":/colliders/images/colliderHalfWest.png", ":/colliders/images/colliderHalfSouth.png",
	":/colliders/images/colliderQuadSE.png", ":/colliders/images/colliderQuadNE.png", ":/colliders/images/colliderQuadNW.png", ":/colliders/images/colliderQuadSW.png",
	":/colliders/images/colliderLSouthEast.png", ":/colliders/images/colliderLNorthEast.png", ":/colliders/images/colliderLNorthWest.png", ":/colliders/images/colliderLSouthWest.png",
	":/colliders/images/colliderDiagSENW.png", ":/colliders/images/colliderDiagSWNE.png",
	":/colliders/images/collider45SE.png", ":/colliders/images/collider45NE.png", ":/colliders/images/collider45NW.png", ":/colliders/images/collider45SW.png"
};


//===== INSTANCE MEMBERS =====

void ElysianTileWrapper::setColliderString(const QString string) {
	colliderType = getColliderEnumFromString(string);

	//Update scenes in case they are rendering solidity, and that solid changed!
	if(TileEntry::renderColliders) {
		Level::updateScenes();
		Area::updateScene();
	}
}

QWidget *TileSelectableSelectionViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

	auto model = static_cast<const PropertyStackModel*>(index.model());
	auto item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));
	QComboBox *colliderEditor;
	QCheckBox *animatedEditor;

	switch(item->getUndoItemType()) {
	case UndoItem::TILE_COLLIDER:
		colliderEditor = new QComboBox(parent);
		colliderEditor->setFocusPolicy(Qt::WheelFocus);

		//Populate ComboBox
		for(signed i = 0; i < elysian::Tile::COLLIDER_MAX; ++i) {
			colliderEditor->addItem(ElysianTileWrapper::getColliderIconFromEnum((ElysianTileWrapper::COLLIDER_TYPE)i),
							ElysianTileWrapper::getColliderStringFromEnum((ElysianTileWrapper::COLLIDER_TYPE)i));
		}

		return colliderEditor;
	/*case UndoItem::TILE_ANIMATED:
		animatedEditor = new QCheckBox(parent);
		return animatedEditor;*/

	default:
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void TileSelectableSelectionViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	auto model = static_cast<const PropertyStackModel*>(index.model());
	auto item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));
	QComboBox *colliderEditor;
	QCheckBox *animatedEditor;
	QString string;
	bool checked;

	switch(item->getUndoItemType()) {
	case UndoItem::TILE_COLLIDER:
		colliderEditor = static_cast<QComboBox*>(editor);

		//Set ComboBox's selection to current data
		string = index.model()->data(index, Qt::EditRole).toString();
		colliderEditor->setCurrentIndex((int)ElysianTileWrapper::getColliderEnumFromString(string));
		break;

	/*case UndoItem::TILE_ANIMATED:
		animatedEditor = static_cast<QCheckBox*>(editor);
		checked = index.model()->data(index, Qt::EditRole).toBool();
		animatedEditor->setChecked(checked);

		break;*/
	default:
		QStyledItemDelegate::setEditorData(editor, index);
	}

}

void TileSelectableSelectionViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	auto item = static_cast<ESTkStandardItem*>(static_cast<PropertyStackModel*>(model)->itemFromIndex(index));
	QComboBox *colliderEditor;
	QCheckBox *animatedEditor;

	switch(item->getUndoItemType()) {
	case UndoItem::TILE_COLLIDER:
		colliderEditor = static_cast<QComboBox*>(editor);
		//Update model to reflect comboBox choice
		model->setData(index, ElysianTileWrapper::getColliderStringFromEnum((ElysianTileWrapper::COLLIDER_TYPE)colliderEditor->currentIndex()), Qt::EditRole);
		break;

	/*case UndoItem::TILE_ANIMATED:
		animatedEditor = static_cast<QCheckBox*>(editor);
		model->setData(index, animatedEditor->isChecked());

		break;
*/
	default:
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

