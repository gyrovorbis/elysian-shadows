#ifndef ELYSIAN_LAYOUT_HPP
#define ELYSIAN_LAYOUT_HPP

#include <vector>
#include <initializer_list>

namespace elysian {

    class Widget;

    class Layout {
    public:
        enum MARGIN {

        };

    private:
        std::vector<Widget*>    _widgets;
        unsigned                _spacing = 0;

    public:

                    Layout(void) = default;
                    Layout(std::vector<Widget*>&& widgets);
                    Layout(std::vector<Widget*>& widgets);

                    ~Layout(void);

        void        addWidget(Widget* widget);
        void        removeWidget(Widget* widget);
        unsigned    getWidgetCount(void) const;
        Widget*     getWidget(unsigned index);

        void        update(void);

    };







    //======== INLINEZ =============
    Layout::Layout(std::vector<Widget*>&& widgets):
        _widgets(widgets) {}

    Layout::Layout(std::vector<Widget*>& widgets):
        _widgets(widgets) {}

    void Layout::addWidget(Widget* widget) {
        _widgets.push_back(widget);
    }

    Widget* Layout::getWidget(unsigned index) {
        return _widgets[index];
    }

    void Layout::removeWidget(Widget* widget) {
        for(auto it = _widgets.begin(); it != _widgets.end(); ++it) {
            if(*it == widget) {
                _widgets.erase(it);
                break;
            }
        }
    }

    unsigned Layout::getWidgetCount(void) const {
        return (unsigned)_widgets.size();
    }

}

#endif // ELYSIAN_LAYOUT_HPP
