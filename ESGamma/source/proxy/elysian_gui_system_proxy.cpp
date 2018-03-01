#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_widget_proxy.hpp>
#include <proxy/elysian_layout_proxy.hpp>
#include <proxy/elysian_gui_system_proxy.hpp>

/*!
    \qmltype AdventureLog
    \since 1.5.8
    \inmodule Engine
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
OOLUA_EXPORT_FUNCTIONS_CONST(AdventureLog)


/*!
    \qmltype GuiSystem
    \since 1.6.0
    \inmodule Engine
    \brief Engine Subsystem for handling all menus, text, and UI overlays.

    The GuiSystem's main responsibity is to manage UI widgets (overlays AND in-scene),
    including any type of font and text rendering.

    \section2 Retrieving the GuiSystem
    The GuiSystem object can be retrieved through the static Engine class:
    \code
    guiSys = Engine.getGuiSys()
    \endcode

    \section2 AdventureLog Widget
    The GuiSystem contains the \l{AdventureLog} \l{Widget} used to display major in-game events such as
    combat events, gaining/losing party members, quest acceptance/completion and more. It also
    can be useful for displaying debug information from scirpts.
    \sa getAdvLog

    \section2 Foreground and Background Widgets
    The GuiSystem can optionally accept custom \l{Widget} objects for rendering arbitrary entities, menus,
    or graphics above or below the scene. This is useful for special effects going on in the background, beneath
    a see-through map, or for implementing custom UI overlay widgets.
    \sa setFgWidget(), setBgWidget(), clearFgWidget(), clearBgWidget()

    \section2 Changing Render Pass Clear Colors
    The GuiSystem lets you modify the default blank color of the framebuffer when a new frame has started.
    It defaults to black, but may be set to any arbitrary color.
    \sa setClearColor()

    \section2 In-Game Text
    In addition to handling background and foreground layer text, the GuiSystem is also responsible for rendering
    general text that exists in the game world such as the damage numbers that fly when a player gives or recevies
    damage, or the EXP and Level up notifications that get displayed during combat. You can use this functionality
    with your own custom text.
    \sa addCombatText(), addVictoryText

    \section2 Font Color Palette
    The GuiSystem maintains an internal color palette that may be referenced from special markup within arbitrary
    strings to change the color of text. An example of this is displaying certain character names or key locations
    as a different color than the rest of the text to stand out.

    The following string:
    \code
    str = "You shall burn in the ~01Fiery~00 pits of hell."
    \endcode
    Changes the font color of "Fiery" to use the font color set within the palette at index 01 before returning back
    to the default font color (white) at index ~00.
    \sa getFontColorPaletteEntry(), setFontColorPaletteEntry()


    \sa {Widget}, {Layout}, {AdventureLog}, {Engine}
*/
/*!
 * \qmlmethod void GuiSystem::addCombatText(string text, GYColor4 color, GYVector2 pos, const GYVector2 vel)
 * Emits a moving text object displaying the given \a{text} in the same style as combat damage numbers when giving or receiving damage.
 * The object begins at the location given by \a{pos} with the initial velocity given by \a{vel} and the text color given by \a{color}.
 * \sa addVictoryText()
 */
/*!
 * \qmlmethod void GuiSystem::setFgWidget(Widget* widget)
 * Sets \a{widget} as the active foreground widget, which will be rendered ontop of the scene.
 * \sa clearFgWidget()
 */
/*!
 * \qmlmethod void GuiSystem::clearFgWidget()
 * Clears the foreground widget.
 * \sa setFgWidget()
 */
/*!
 * \qmlmethod void GuiSystem::setBgWidget(Widget* widget)
 * Sets \a{widget} as the active background widget, which will be rendered beneath the map and the rest of the scene.
 * This is useful for areas like floating islands where you want a massive texture in the background showing through
 * beneath the map.
 * \sa clearBgWidget()
 */
/*!
 * \qmlmethod void GuiSystem::clearBgWidget()
 * Clears the background widget.
 * \sa setBgWidget()
 */
/*!
 * \qmlmethod void GuiSystem::setClearColor(GYColor color)
 * Sets the color the framebuffer is reset to at the beginning of each render pass. This defaults to black, but this
 * allows you to set it to any arbitrary \a{color}.
 */
/*!
 * \qmlmethod void GuiSystem::setAdvLogEnabled(bool enabled)
 * Enables or disables the \l{AdventureLog} GUI element. When disabled, it will no longer render, even if new text or
 * events are added to it. It is enabled by default.
 * \sa getAdvLog()
 */
/*!
 * \qmlmethod AdventureLog& GuiSystem::getAdvLog()
 * Returns a reference to the \l{AdventureLog} GUI widget.
 * \sa setAdvLogEnabled()
 */
/*!
 * \qmlmethod void GuiSystem::addVictoryText(GYVector2 pos, string text)
 * Emits a text object with the given \a{text} at the position given by \a{pos} within the scene
 * that animates with the Crash Bandicoot-style typewriter effect, where each character pops in
 * sequentially. The effect is also used for displaying the "level up" notification.
 * \sa addCombatText()
 */
/*!
 * \qmlmethod int, int GuiSystem::getResolution()
 *  Returns TWO integers, containing the ACTUAL pixel width and height of the current viewport. This is not effected
 *  by any camera scaling or zooming, and is purely the raw dimensions of the scene window or the device.
 */
/*!
 * \qmlmethod GYColor4 GuiSystem::getFontColorPaletteEntry(int index)
 * Returns the color assigned to the font pallette at the given \a{index}
 * \sa setFontColorPaletteEntry(), {Font Color Palette}
 *
 */
/*!
 * \qmlmethod GYColor4 GuiSystem::setFontColorPaletteEntry(int index, GYColor4 color)
 * Sets the font palette entry at the given \a{index} to \a{color}.
 * \sa getFontColorPaletteEntry(), {Font Color Palette}
 */


OOLUA_EXPORT_FUNCTIONS(GuiSystem,
                       addCombatText,
                       setFgWidget,
                       clearFgWidget,
                       setBgWidget,
                       clearBgWidget,
                       setClearColor,
                       setAdvLogEnabled,
                       addVictoryText,
                       setFontColorPaletteEntry,
                       getAdvLog)
OOLUA_EXPORT_FUNCTIONS_CONST(GuiSystem,
                             getResolution,
                             getFontColorPaletteEntry)
