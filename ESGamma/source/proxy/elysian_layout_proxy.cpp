#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <AssetIO/proxy/elysian_layout_proxy.hpp>
#include <AssetIO/proxy/elysian_widget_proxy.hpp>

/*!
    \qmltype TableLayout
    \since 1.6.0
    \inmodule Engine
    \brief Abstract class for organizing Widgets as child GUI elements within a parent Widget.
*/

OOLUA_EXPORT_FUNCTIONS(TableLayout,
                       removeWidget,
                       setHorizontalSpacing,
                       setVerticalSpacing,
                       setSpacing,
                       setHorizontalAlignment,
                       setVerticalAlignment,
                       setAlignment)

OOLUA_EXPORT_FUNCTIONS_CONST(TableLayout,
                             getWidgetCount,
                             getParent,
                             getHorizontalSpacing,
                             getVerticalSpacing,
                             getHorizontalAlignment,
                             getVerticalAlignment)
/*!
    \qmltype SingleAxisLayout
    \inherits TableLayout
    \since 1.6.0
    \inmodule Engine
    \brief Abstract class for TableLayouts whose size is limited to only a single dimension
*/
OOLUA_EXPORT_FUNCTIONS(SingleAxisLayout,
                       setWidgets,
                       addWidget)
OOLUA_EXPORT_FUNCTIONS_CONST(SingleAxisLayout,
                             getWidget,
                             getWidgets)


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
