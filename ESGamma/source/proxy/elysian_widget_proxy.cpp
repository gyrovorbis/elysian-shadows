#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_layout_proxy.hpp>
#include <proxy/elysian_widget_proxy.hpp>

/*!
    \qmltype Widget
    \since 1.6.0
    \inmodule ESToolkit
    \brief Abstract base class for all objects that can be added to the GUI layer.
*/

OOLUA_EXPORT_FUNCTIONS(Widget,
                       setMinWidth,
                       setMinHeight,
                       setMinSize,
                       setMaxWidth,
                       setMaxHeight,
                       setMaxSize,
                       setHorizontalSizePolicy,
                       setVerticalSizePolicy,
                       setSizePolicy,
                       setMarginWidth,
                       setMarginHeight,
                       setMarginSize,
                       setLayout,
                       setActive)
OOLUA_EXPORT_FUNCTIONS_CONST(Widget,
                             getMinWidth,
                             getMinHeight,
                             getMaxWidth,
                             getMaxHeight,
                             getHorizontalSizePolicy,
                             getVerticalSizePolicy,
                             getMarginWidth,
                             getMarginHeight,
                             getLayout,
                             isActive)

/*!
    \qmltype EntityWidget
    \since 1.6.0
    \inmodule ESToolkit
    \brief Widget container for displaying an Entity in a GUI.
*/

OOLUA_EXPORT_FUNCTIONS(EntityWidget,
                       setEntity)

OOLUA_EXPORT_FUNCTIONS_CONST(EntityWidget,
                             getEntity)

/*!
    \qmltype LabelWidget
    \since 1.6.0
    \inmodule ESToolkit
    \brief Widget used for displaying text in a GUI.
*/

OOLUA_EXPORT_FUNCTIONS(LabelWidget,
                       setText,
                       setColor)

OOLUA_EXPORT_FUNCTIONS_CONST(LabelWidget,
                             getText,
                             getColor)

