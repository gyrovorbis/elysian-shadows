/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QESTkUtils/ui/delegate/tk_spinbox_property_item_delegate.h>
#include <QESTkUtils/model/tk_property_stack_model.h>
#include <QESTkUtils/model/tk_property_standard_item.h>
#include <QSpinBox>


QWidget* SpinboxPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
    auto* editor = new QSpinBox(parent);
    editor->setMaximum(_max);
    editor->setMinimum(_min);
    return editor;
}


void SpinboxPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QSpinBox* spinbox = qobject_cast<QSpinBox*>(editor);
    auto* item = modelIndexToESTkStandardItem(index);

    if(spinbox) spinbox->setValue(item->data(Qt::EditRole).toInt());
    else QStyledItemDelegate::setEditorData(editor, index);
}


void SpinboxPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QSpinBox* spinbox = qobject_cast<QSpinBox*>(editor);

    if(spinbox) {
        auto* item = modelIndexToESTkStandardItem(index);

        item->setData(spinbox->value(), Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}
