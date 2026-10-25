/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_COLOR_SWATCH_PROPERTY_ITEM_DELEGATE_H
#define TK_COLOR_SWATCH_PROPERTY_ITEM_DELEGATE_H

#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>
#include <AssetIO/utility/elysian_functional.hpp>
#include <QPainter>
#include <QColorDialog>
#include <QDebug>
#include <AssetIO/proxy/elysian_primitive_proxy.hpp>

#define TK_COLOR_SWATCH_ICON_PATH ":/misc/images/color_swatch_icon.png"
#if 0
WriteProxy  operator[](I&& index);
V           operator[](I&& index) const;
#endif

namespace QESTkUtils {

    template<typename T,
             typename IndexType=int,
             typename ComponentType=decltype((*std::declval<const T*>())[std::declval<IndexType>()])>
    struct QtColorConverter {
        static T        fromQColor(const QColor& qcolor);
        static QColor   toQColor(const T& color);
    };

    template<typename T, typename IndexType, typename ComponentType>
    inline T QtColorConverter<T, IndexType, ComponentType>::fromQColor(const QColor& qcolor) {
        return T(
            (ComponentType)qcolor.red(),
            (ComponentType)qcolor.green(),
            (ComponentType)qcolor.blue(),
            (ComponentType)qcolor.alpha()
        );
    }

    template<typename T, typename IndexType, typename ComponentType>
    inline QColor QtColorConverter<T, IndexType, ComponentType>::toQColor(const T& color) {
        return QColor((int)color[0], (int)color[1], (int)color[2], (int)color[3]);
    }

    template<typename T,
             typename IndexType>
    struct QtColorConverter<T, IndexType, float> {
        static T        fromQColor(const QColor& qcolor);
        static QColor   toQColor(const T& color);
    };


    template<typename T, typename IndexType>
    inline T QtColorConverter<T, IndexType, float>::fromQColor(const QColor& qcolor) {
        return T (
            qcolor.redF(),
            qcolor.greenF(),
            qcolor.blueF(),
            qcolor.alphaF()
        );
    }

    template<typename T, typename IndexType>
    inline QColor QtColorConverter<T, IndexType, float>::toQColor(const T& color) {
        return QColor::fromRgbF(color[0], color[1], color[2], color[3]);
    }


    template<typename ColorType,
             typename IndexType=int,
             typename ComponentType=decltype(std::declval<ColorType>()[std::declval<IndexType>()]),
             typename = typename std::enable_if_t<!std::is_void<ComponentType>::value>>
    class ColorSwatchPropertyItemDelegate: public DynamicContentPropertyItemDelegate {

    public:
        ColorSwatchPropertyItemDelegate(void) {
                addButton(QIcon(TK_COLOR_SWATCH_ICON_PATH), &ColorSwatchPropertyItemDelegate::onColorSwatchButtonClick, "Select Color from Swatch");
                setReadOnly();
        }
    virtual void        paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
            DynamicContentPropertyItemDelegate::paint(painter, option, index);

            auto* luaItem = modelIndexToLuaTableFieldItem(index);
            if(luaItem) {
                auto* color = luaItem->getVariant().template getValue<ColorType*>();
                if(color) {
                    QStyleOptionViewItem itemOption(option);
                    initStyleOption(&itemOption, index);
                    int y = option.rect.y();
                    int h = option.rect.height() - 2;
                    int x = option.rect.x() + 5;
                    int w = 64;
                    //painter->setPen(*color);


                    painter->setBrush(QBrush(QtColorConverter<ColorType>::toQColor(*color)));
                    painter->drawRect(x, y, w, h);
                }
            }
        }

    private:

            static void onColorSwatchButtonClick(ESTkStandardItem* item) {
                LuaTableFieldItem* luaFieldItem = LuaTableFieldItem::fromStandardItem(item);
                if(luaFieldItem) {
                    ColorType* color = luaFieldItem->getVariant().getValue<ColorType*>();

                    if(color) {
                        QColor qcolor = QColorDialog::getColor(QtColorConverter<ColorType>::toQColor(*color));
                        if(qcolor.isValid()) {

                            *color = QtColorConverter<ColorType>::fromQColor(qcolor);
                            luaFieldItem->setVariant(color);

                            luaFieldItem->setDirty();
                            luaFieldItem->emitDataChanged();
            #if 0
                            LuaTableFieldItem* parentItem = LuaTableFieldItem::fromStandardItem(luaFieldItem->getTableData()->getParentItem());
                            color->emitFieldChanged("r");
                            color->emitFieldChanged("g");
                            color->emitFieldChanged("b");
                            color->emitFieldChanged("a");
            #endif
                        }
                       }
                }
            }
    };

}




//=========== INLINEZ ===========




#endif // TK_COLOR_SWATCH_PROPERTY_ITEM_DELEGATE_H
