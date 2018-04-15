#ifndef ELYSIAN_FRAME_WIDGET_HPP
#define ELYSIAN_FRAME_WIDGET_HPP

#include <gui/elysian_widget.hpp>

namespace elysian {

    class SingleAxisLayout;

    class FrameWidget: public GroupWidget {
    public:
                            FrameWidget(void) = default;
                            FrameWidget(SingleAxisLayout* layout);
                            FrameWidget(Widget* centralWidget);
                            FrameWidget(std::nullptr_t);
        void                setCentralWidget(Widget* centralWidget);
        Widget*             getCentralWidget(void) const;

        SingleAxisLayout*   getSingleAxisLayout(void) const;
    };


}

#endif // ELYSIAN_FRAME_WIDGET_HPP
