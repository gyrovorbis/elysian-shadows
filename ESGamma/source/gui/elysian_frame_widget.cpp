#include <ESGamma/gui/elysian_frame_widget.hpp>
#include <ESGamma/gui/elysian_layout.hpp>

namespace elysian {

FrameWidget::FrameWidget(SingleAxisLayout* layout):
    GroupWidget(layout)
{}

FrameWidget::FrameWidget(Widget* centralWidget) {
    setLayout(new ELYSIAN_FRAME_WIDGET_LAYOUT_TYPE_DEFAULT());
    setCentralWidget(centralWidget);
}

FrameWidget::FrameWidget(std::nullptr_t):
    FrameWidget((Widget*)nullptr)
{}

SingleAxisLayout* FrameWidget::getSingleAxisLayout(void) const {
    return static_cast<SingleAxisLayout*>(getLayout());
}

void FrameWidget::setCentralWidget(Widget* centralWidget) {
    GY_ASSERT(getLayout());
    GY_ASSERT(getSingleAxisLayout()->getWidgetCount() <= 1);

    //get rid of any existing shit
    if(getLayout()->getWidgetCount() && getSingleAxisLayout()->getWidget(0) && getSingleAxisLayout()->getWidget(0) != centralWidget) {
        getLayout()->removeWidget(getSingleAxisLayout()->getWidget(0));
    }

    //add new shit (if there is new shit)
    if(centralWidget) getSingleAxisLayout()->addWidget(centralWidget);
}

Widget* FrameWidget::getCentralWidget(void) const {
    GY_ASSERT(getLayout());
    return getSingleAxisLayout()->getWidget(0);
}

}
