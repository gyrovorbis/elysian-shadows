#ifndef TK_SLIDER_PROPERTY_ITEM_DELEGATE_H
#define TK_SLIDER_PROPERTY_ITEM_DELEGATE_H

#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>

#define TK_SLIDER_PROPERTY_ITEM_DELEGATE_MIN_DEFAULT    0.0f
#define TK_SLIDER_PROPERTY_ITEM_DELEGATE_MAX_DEFAULT    1.0f

namespace QESTkUtils {

    class SliderPropertyItemDelegate: public DynamicContentPropertyItemDelegate {
        using PopulatorType = std::function<QStringList(ESTkStandardItem*)>;

    private:
        float _max      = TK_SLIDER_PROPERTY_ITEM_DELEGATE_MAX_DEFAULT;
        float _min      = TK_SLIDER_PROPERTY_ITEM_DELEGATE_MIN_DEFAULT;


    public:
                        SliderPropertyItemDelegate(float min, float max);
                        SliderPropertyItemDelegate(void) = default;

        int             toSliderValue(float value) const;
        float           fromSliderValue(int value) const;


        virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
        virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    };





    //======= INLINEZ =========
    inline SliderPropertyItemDelegate::SliderPropertyItemDelegate(float min, float max):
        _max(max),
        _min(min)
    {}

    inline int SliderPropertyItemDelegate::toSliderValue(float value) const {
        return (value/(_max-_min))*100;
    }
    inline float SliderPropertyItemDelegate::fromSliderValue(int value) const {
        return ((float)value/(float)100.0f*(_max-_min))+_min;
    }

}


#endif // TK_SLIDER_PROPERTY_ITEM_DELEGATE_H
