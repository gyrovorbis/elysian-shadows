#ifndef ELYSIAN_ITEM_DELEGATE_HPP
#define ELYSIAN_ITEM_DELEGATE_HPP

#include <crtp/elysian_lua_variant.hpp>
#include <gui/elysian_item_model.hpp>
#include <map>

namespace elysian {

    class Widget;

    class ItemDelegate {
    protected:
        ItemModel*          _model = nullptr;
    public:
                            ItemDelegate(ItemModel* model);
        virtual             ~ItemDelegate(void) = default;

        virtual void        deactivate(void) = 0;
        virtual Widget*     getDisplayWidget(const ItemModelIndex& index) const = 0;
        virtual unsigned    sizeHint(const ItemModelIndex& index, int axis) const = 0;

    };

    class DefaultItemDelegate: public ItemDelegate {
    private:
        mutable std::map<ItemModelIndex, Widget*>
                            _widgetCache;

        Widget*             _createWidget(const ItemModelIndex& index) const;
        Widget*             _findOrCreateWidget(const ItemModelIndex& index) const;
        void                _clearWidgetCache(void);

    public:
                            DefaultItemDelegate(ItemModel* model);
        virtual             ~DefaultItemDelegate(void) override;

        bool                isValidIndex(const ItemModelIndex& index) const;

        virtual Widget*     getDisplayWidget(const ItemModelIndex &index) const override;
        virtual unsigned    sizeHint(const ItemModelIndex&, int axis) const override;
        virtual void        deactivate(void) override;

    };



    //===== INLINEZ ========
    inline ItemDelegate::ItemDelegate(ItemModel* model):
        _model(model) {}

    inline DefaultItemDelegate::DefaultItemDelegate(ItemModel *model):
        ItemDelegate(model) {}


    inline Widget* DefaultItemDelegate::_findOrCreateWidget(const ItemModelIndex &index) const {
        Widget* widget = nullptr;
        auto it = _widgetCache.find(index);

        if(it != _widgetCache.end()) {
            widget = it->second;
        } else {
            widget = _createWidget(index);
            _widgetCache[index] = widget;
        }

        return widget;
    }


}


#endif // ELYSIAN_ITEM_DELEGATE_HPP
