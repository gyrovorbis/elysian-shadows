#ifndef ELYSIAN_ITEM_MODEL_HPP
#define ELYSIAN_ITEM_MODEL_HPP

#include <AssetIO/crtp/elysian_lua_variant.hpp>
#include <ESGamma/gui/elysian_widget.hpp>

#define ELYSIAN_ITEM_MODEL_INDEX_INVALID    -1

namespace elysian {

    struct ItemModelIndex {
        const static int    InvalidIndex                = ELYSIAN_ITEM_MODEL_INDEX_INVALID;
        int                 indices[Widget::AXIS_COUNT] = { InvalidIndex };

                            ItemModelIndex(void) = default;
                            ItemModelIndex(int row, int col);
        bool                operator<(const ItemModelIndex& rhs) const;

        bool                isValid(void) const;
        int                 getAxisIndex(Widget::AXIS axis) const;
        int                 getRow(void) const;
        int                 getCol(void) const;
    };

    class ItemModel {
    public:
        virtual             ~ItemModel(void) = default;

        virtual unsigned    getAxisCount(Widget::AXIS axis) const = 0;
        virtual LuaVariant  getItemValue(const ItemModelIndex& index) const = 0;
        virtual bool        setItemValue(const ItemModelIndex& index, LuaVariant value) = 0;

        //built-in utility functions
        unsigned            getRowCount(void) const;
        unsigned            getColCount(void) const;
        bool                isValidIndex(const ItemModelIndex& index);


        //events/signals and shit for values and dimensions changing

    };


    //Super thin-as-fuck inlined proxy around a LuaVariant table
    class LuaArrayModel: public ItemModel {
    private:
        LuaVariant          _table;
        bool                _isValid = false;

    public:
                            LuaArrayModel(LuaVariant table);

        virtual unsigned    getAxisCount(Widget::AXIS axis) const override;
        virtual LuaVariant  getItemValue(const ItemModelIndex& index) const override;
        virtual bool        setItemValue(const ItemModelIndex& index, LuaVariant value) override;

    };



    //==== INLINEZ =====

    inline ItemModelIndex::ItemModelIndex(int row, int col) {
        indices[Widget::Y] = row;
        indices[Widget::X] = col;
    }

    inline bool ItemModelIndex::isValid(void) const {
        return indices[Widget::X] != InvalidIndex && indices[Widget::Y] != InvalidIndex;
    }

    inline bool ItemModelIndex::operator <(const ItemModelIndex& rhs) const {
        return (getRow() < rhs.getRow()) || (getRow() == rhs.getRow() && getCol() < rhs.getCol());
    }

    inline int ItemModelIndex::getAxisIndex(Widget::AXIS axis) const {
        return indices[axis];
    }

    inline int ItemModelIndex::getRow(void) const { return indices[Widget::Y]; }
    inline int ItemModelIndex::getCol(void) const { return indices[Widget::X]; }


    inline unsigned ItemModel::getRowCount(void) const { return getAxisCount(Widget::Y); }
    inline unsigned ItemModel::getColCount(void) const { return getAxisCount(Widget::X); }
    inline bool ItemModel::isValidIndex(const ItemModelIndex& index) {
        return index.isValid() &&
                index.getRow() < (int)getRowCount() &&
                index.getCol() < (int)getColCount();
    }

    inline LuaArrayModel::LuaArrayModel(LuaVariant table):
        _table(std::move(table))
    {
        _isValid = (_table.getType() == LUA_TTABLE);
    }

    inline unsigned LuaArrayModel::getAxisCount(Widget::AXIS axis) const {
        return (axis == Widget::Y)? _table.getLength() : 1;
    }

    inline LuaVariant LuaArrayModel::getItemValue(const ItemModelIndex &index) const {
        return _table[index.getRow()+1].getVariant();
    }

    inline bool LuaArrayModel::setItemValue(const ItemModelIndex &index, LuaVariant value) {
        return _table[index.getRow()+1].setVariant(std::move(value)); //Lua arrays start at index 1, dickweasel!!!
    }

}

/*
 * std::less specialization to allow ItemModelIndex to be the key of std::map without requiring a comparitor
 * and instead using the standard comparison operator defined within the class
 */
namespace std {
    template<> struct less<elysian::ItemModelIndex> {
        bool operator()(const elysian::ItemModelIndex& lhs, const elysian::ItemModelIndex& rhs) const {
            return lhs < rhs;
        }
    };
}



#endif // ELYSIAN_UI_MODEL_HPP
