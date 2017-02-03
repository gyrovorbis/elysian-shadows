#include "gui/elysian_layout.hpp"
#include "gui/elysian_widget.hpp"

namespace elysian {

    Layout::~Layout(void) {
        for(auto&& w: _widgets)
            delete w;
    }

    void Layout::update(void) {

    }



}
