#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_layout_proxy.hpp>
#include <proxy/elysian_widget_proxy.hpp>

/*!
    \qmltype Layout
    \since 1.6.0
    \inmodule Engine
    \brief Abstract class for organizing Widgets as child GUI elements within a parent Widget.
*/

OOLUA_EXPORT_FUNCTIONS(Layout,
                       addWidget,
                       removeWidget,
                       setWidgets,
                       setHorizontalSpacing,
                       setVerticalSpacing,
                       setSpacing,
                       setHorizontalAlignment,
                       setVerticalAlignment,
                       setAlignment)

OOLUA_EXPORT_FUNCTIONS_CONST(Layout,
                             getWidgets,
                             getWidgetCount,
                             getWidget,
                             getParent,
                             getHorizontalSpacing,
                             getVerticalSpacing,
                             getHorizontalAlignment,
                             getVerticalAlignment)


/*!
    \qmltype HorizontalLayout
    \since 1.6.0
    \inmodule Engine
    \brief GUI Layout for arranging Widgets horizontally into columns within a parent Widget.
*/

OOLUA_EXPORT_FUNCTIONS(HorizontalLayout)
OOLUA_EXPORT_FUNCTIONS_CONST(HorizontalLayout)


/*!
    \qmltype VerticalLayout
    \since 1.6.0
    \inmodule Engine
    \brief GUI Layout for arranging Widgets vertically into rows within a parent Widget.
*/

OOLUA_EXPORT_FUNCTIONS(VerticalLayout)
OOLUA_EXPORT_FUNCTIONS_CONST(VerticalLayout)
