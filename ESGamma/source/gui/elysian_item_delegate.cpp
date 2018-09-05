#include <ESGamma/gui/elysian_item_delegate.hpp>
#include <ESGamma/gui/elysian_item_model.hpp>
#include <ESGamma/gui/elysian_label_widget.hpp>
#include <map>


namespace elysian {

    DefaultItemDelegate::~DefaultItemDelegate(void) {
        _clearWidgetCache();
    }

    void DefaultItemDelegate::_clearWidgetCache(void) {
        for(auto& widget : _widgetCache) {
            delete widget.second;
        }
    }

    void DefaultItemDelegate::deactivate(void) {
        _clearWidgetCache();
    }

    Widget* DefaultItemDelegate::_createWidget(const ItemModelIndex &index) const {
        return new LabelWidget(_model->getItemValue(index).asString().getValue<const char*>());
    }

    Widget* DefaultItemDelegate::getDisplayWidget(const ItemModelIndex &index) const {
        return _model->isValidIndex(index)? _findOrCreateWidget(index) : nullptr;
    }

    unsigned DefaultItemDelegate::sizeHint(const ItemModelIndex& index,int axis) const {
        return _model->isValidIndex(index)? _findOrCreateWidget(index)->getSize((Widget::AXIS)axis) : 0;
    }

    void DefaultItemDelegate::onCurrentChanged(Widget* prev, Widget* next) const {
        if(prev) static_cast<LabelWidget*>(prev)->setColor(Color4::white);
        if(next) static_cast<LabelWidget*>(next)->setColor(Color4::aqua);
    }


}
