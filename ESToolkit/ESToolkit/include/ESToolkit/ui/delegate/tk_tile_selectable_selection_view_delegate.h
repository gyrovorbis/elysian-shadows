/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TILE_SELECTABLE_SELECTION_VIEW_DELEGATE_H
#define TILE_SELECTABLE_SELECTION_VIEW_DELEGATE_H

#include <QStyledItemDelegate>
#include <ESToolkit/AssetIO/tk_elysian_tile_wrapper.h>

//Used as a delegate for SelectionView when a TileSelectable is the active model
class TileSelectableSelectionViewDelegate: public QStyledItemDelegate {
public:
	TileSelectableSelectionViewDelegate(void);
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};


//===== INLINE METHODS =====


inline TileSelectableSelectionViewDelegate::TileSelectableSelectionViewDelegate(void) {
	ElysianTileWrapper::allocIcons();
}




#endif // TILESELECTABLESELECTIONVIEWDELEGATE_H
