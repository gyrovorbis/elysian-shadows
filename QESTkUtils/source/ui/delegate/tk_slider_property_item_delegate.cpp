#include <QESTkUtils/ui/delegate/tk_slider_property_item_delegate.h>
#include <QSlider>
#include <QCursor>
#include <QToolTip>

namespace QESTkUtils {

QWidget* SliderPropertyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &) const {
    QSlider* editor = new QSlider(Qt::Horizontal, parent);
    editor->setAutoFillBackground(true);
    editor->setMaximum(100);
    editor->setMinimum(0);
    editor->setGeometry(option.rect);

    QObject::connect(editor, &QSlider::valueChanged, [=](int value) {
        QToolTip::showText(QCursor::pos(), QString("%1").arg(fromSliderValue(value)), nullptr, QRect(), _toolTipTime);
    });

    return editor;
}

void SliderPropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QSlider* slider = static_cast<QSlider*>(editor);
    auto* item = modelIndexToESTkStandardItem(index);

    if(slider) {
        slider->setValue(toSliderValue(item->data(Qt::EditRole).toFloat()));
    } else QStyledItemDelegate::setEditorData(editor, index);
}


void SliderPropertyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QSlider* slider = static_cast<QSlider*>(editor);

    if(slider) {
        auto* item = modelIndexToESTkStandardItem(index);

        item->setData(fromSliderValue(slider->value()), Qt::EditRole);
    } else QStyledItemDelegate::setModelData(editor, model, index);
}

}
