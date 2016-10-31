#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_gui_system_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS_CONST(MenuWidget)
OOLUA_EXPORT_FUNCTIONS(MenuWidget)

OOLUA_EXPORT_FUNCTIONS(MenuText,
                       setRelPos,
                       setFontType,
                       setColor,
                       setAlignment,
                       setText)
OOLUA_EXPORT_FUNCTIONS_CONST(MenuText)

OOLUA_EXPORT_FUNCTIONS(MenuEntity,
                       addChildWidget,
                       setSizeType,
                       removeChildWidget)
OOLUA_EXPORT_FUNCTIONS_CONST(MenuEntity)

/*!
    \qmltype AdventureLog
    \since 1.5.8
    \inmodule ESToolkit
    \brief GUI Widget for Combat Log and in-game text. Can be used to display custom content.
*/

/*!
 * \qmlmethod void AdventureLog::setLabel(string label)
 *
 * Renames the title of the AdventureLog to "label."
 */

/*!
 * \qmlmethod void AdventureLog::setStaticText(bool toggle)
 *
 * Static text disables the auto-scroll feature of the AdventureLog, so you can use it to display custom information. Set it back to false to return to normal behavior.
 */

/*!
 * \qmlmethod void AdventureLog::addLine(string text)
 *
 * Pushes the string "text" onto the bottom of the AdventureLog. Text may contain markup characters.
 */

/*!
 * \qmlmethod void AdventureLog::clear()
 *
 * Clears all visible text from the AdventureLog.
 */

OOLUA_EXPORT_FUNCTIONS(AdventureLog,
                       setLabel,
                       setStaticText,
                       addLine,
                       clear)
OOLUA_EXPORT_FUNCTIONS_CONST(AdventureLog);


OOLUA_EXPORT_FUNCTIONS(GuiSystem,
                       addCombatText,
                       setFgMenuWidget,
                       clearFgMenuWidget,
                       setBgMenuWidget,
                       clearBgMenuWidget,
                       setClearColor,
                       setAdvLogEnabled,
                       addVictoryText,
                       setFontColorPaletteEntry,
                       getAdvLog)
OOLUA_EXPORT_FUNCTIONS_CONST(GuiSystem,
                             getResolution,
                             getFontColorPaletteEntry)
