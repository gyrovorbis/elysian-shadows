#include <QComboBox>
#include <QCheckBox>
#include <ESToolkit/ui/delegate/tk_tile_selectable_selection_view_delegate.h>
#include <ESToolkit/core/tk_assets.h>
#include <ESToolkit/AssetIO/tk_level.h>
#include <ESToolkit/AssetIO/tk_area.h>
#include <QESTkUtils/model/tk_property_standard_item.h>

//===== STATIC MEMBERS =====


QWidget *TileSelectableSelectionViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

	auto model = static_cast<const PropertyStackModel*>(index.model());
	auto item = static_cast<ESTkStandardItem*>(model->itemFromIndex(index));
	QComboBox *colliderEditor;
//	QCheckBox *animatedEditor;

	switch(item->getUndoItemType()) {
	case UndoItem::TILE_COLLIDER:
    case UndoItem::TILE_GROUP_COLLIDER:
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
//	QCheckBox *animatedEditor;
	QString string;

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
//	QCheckBox *animatedEditor;

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




