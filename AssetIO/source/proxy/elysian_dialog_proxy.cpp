#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_dialog_proxy.hpp>

/*!
    \qmltype DialogScreen
    \since 1.6.0
    \inmodule ESToolkit
    \brief Base class for different types of dialog screens.

    A DialogScreen represents a single "screen" of dialog, containing up to 3 lines of text.
    The DialogSystem shows a single screen at a time and progresses to the next screen as the player
    advances the dialog. This is an abstract class and cannot be instantiated standalone. Instead
    use the child classes, DialogText and DialogPrompt.

    \sa {Dialog Trees}, Dialog, DialogSystem
*/
/*!
 * \qmlproperty string DialogScreen::text
 *
 * Text to be displayed during a single screen of dialog. Up to 3 lines are displayable,
 * and should be designated manually by newline characters '\\n'. The text box is the
 * same size on every platform, but you are responsible for ensuring each line fits within
 * the box.
 * \sa getText(), setText()
 */
/*!
 * \qmlproperty bool DialogScreen::trigger
 *
 * Boolean field used to designate whether or not completion of the screen should result
 * in the Lua trigger, onDialog(screen, option) being invoked for \l{Behavior}s attached to the
 * Entity set as the triggerEntity in the parent Dialog object. If the screen type is DialogText,
 * simply progressing to the next screen calls into Lua. If the screen type is DialogPrompt, the Lua
 * trigger will be called once the player chooses an option. The \a{screen} argument is the index of
 * the source screen, and the \a{option} argument is the index of the selected DialogOption or -1 for
 * no option if the screen is not of DialogPrompt type.
 * \sa isTrigger(), setTrigger(), Behavior
 */
/*!
 * \qmlmethod string DialogScreen::getText()
 *
 * Returns the value of the \l{text} property.
 * \sa text, setText()
 */
/*!
 * \qmlmethod void DialogScreen::setText(string text)
 *
 * Sets the value of the \l{text} property to \a{text}.
 * \sa text, getText()
 */
/*!
 * \qmlmethod bool DialogScreen::isTrigger()
 *
 * Returns the value of the \l{trigger} property.
 * \sa trigger, setTrigger()
 */
/*!
 * \qmlmethod void DialogScreen::setTrigger(bool val)
 *
 * Sets the value of the \l{trigger} property to \a{val}.
 * \sa trigger, isTrigger()
 */
/*!
 * \qmlmethod int DialogScreen::getIndex()
 *
 * Returns the index of the current screen in the overall Dialog object.
 * \sa Dialog
 */
/*!
 * \qmlmethod void DialogScreen::addSourceEntity(Entity* src)
 *
 * Adds the Entity, \a{src} as one of the sources of the screen. The source is
 * the Entity who is currently talking.
 * \sa getEntity()
 */
/*!
 * \qmlmethod Entity* DialogScreen::getEntity()
 *
 * Returns a reference to the first source Entity.
 * \sa addSourceEntity()
 */

OOLUA_EXPORT_FUNCTIONS(DialogScreen,
                       addSourceEntity,
                       setTrigger,
                       setText)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogScreen,
                             getText,
                             getIndex,
                             isTrigger,
                             getEntity)

/*!
    \qmltype DialogText
    \inherits DialogScreen
    \since 1.6.0
    \inmodule ESToolkit
    \brief Represents a single dialog screen of text.

    A DialogText object represents a single screen of dialog containing 3 lines of text,
    requiring the player to push a button to continue the dialog and progress to the next
    screen.

    \sa {Dialog Trees}, Dialog, DialogPrompt
*/
/*!
 * \qmlproperty int DialogText::nextIndex
 *
 * Field representing the index of the next screen to be displayed after the current one in a dialog.
 * A \a{nextIndex} of -1 signifies to the DialogSystem that the current screen is the last.
 * \sa getNextIndex(), setNextIndex(), Dialog
 */
/*!
 * \qmlmethod int DialogText::getNextIndex()
 *
 * Returns the value of the \l{nextIndex} property.
 * \sa nextIndex, setNextIndex()
 */
/*!
 * \qmlmethod void DialogText::setNextIndex(int index)
 *
 * Sets the value of the \l{nextIndex} property to \a{index}.
 * \sa nextIndex, getNextIndex()
 */

OOLUA_EXPORT_FUNCTIONS(DialogText,
                       setNextIndex)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogText,
                             getNextIndex)

/*!
    \qmltype DialogOption
    \since 1.6.0
    \inmodule ESToolkit
    \brief Represents a single option within a DialogPrompt.

    A DialogOption object represents a single option that the user can choose
    at the end of a DialogPrompt screen.

    \sa {Dialog Trees}, DialogPrompt, DialogText, Dialog
*/
/*!
 * \qmlproperty string DialogOption::text
 *
 * Field representing the text to be displayed within the dialog box correspondig to this option.
 * \sa getText(), setText()
 */
/*!
 * \qmlproperty int DialogOption::nextIndex
 *
 * Field for the index of the next screen that should be shown when the user selects this option.
 * A value of -1 ends the dialog when this option is selected.
 * \sa getNextIndex(), setNextIndex()
 */

/*!
 * \qmlmethod int DialogOption::getNextIndex()
 *
 * Returns the value of the \l{nextIndex} property.
 * \sa nextIndex, setNextIndex()
 */
/*!
 * \qmlmethod void DialogOption::setNextIndex(int index)
 *
 * Sets the value of the \l{nextIndex} property to \a{index}.
 * \sa nextIndex, getNextIndex()
 */
/*!
 * \qmlmethod string DialogOption::getText()
 *
 * Returns the value of the \l{text} property.
 * \sa text, setText()
 */
/*!
 * \qmlmethod void DialogOption::setText(string text)
 *
 * Sets the value of the \l{text} property to \a{text}.
 * \sa text, getText()
 */

OOLUA_EXPORT_FUNCTIONS(DialogOption,
                       setNextIndex,
                       setText)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogOption,
                             getNextIndex,
                             getText)


/*!
    \qmltype DialogPrompt
    \inherits DialogScreen
    \since 1.6.0
    \inmodule ESToolkit
    \brief Represents a screen of text with a list of choices for the user within a Dialog.

    A DialogPrompt is a single screen within a dialog containing 3 lines of text followed by
    a prompt with a list of options for the user to decide from. This can be used to ask the
    user a question and have him input his answer with a different response branching to a
    different screen of text or triggering an event in a Lua script.

    A DialogPrompt contains a list of \l{DialogOption}s, \l{options}.

    \sa {Dialog Trees}, DialogOption, DialogText, Dialog, DialogSystem
*/
/*!
 * \qmlproperty DialogOption* DialogPrompt::options
 *
 * Array of options to be displayed at the end of the screen for user input.
 * \sa addOption(), getOption(), setOption(), getOptionCount()
 */
/*!
 * \qmlmethod void DialogPrompt::addOption(string text, int nextIndex)
 *
 * Creates a DialogOption object with the given \a{text} and \a{nextIndex} fields
 * and adds it to the \l{options} array.
 * \sa options, getOption(), getOptionCount()
 */
/*!
 * \qmlmethod int DialogPrompt::getOptionCount()
 *
 * Returns the number of options contained within the prompt.
 * \sa options, getOption(), addOption()
 */
/*!
 * \qmlmethod DialogOption* DialogPrompt::getOption(int index)
 *
 * Returns a reference to the DialogOption located at the given \a{index} within the
 * \l{options} list.
 * \sa options, addOption(), setOption()
 */
/*!
 * \qmlmethod DialogOption* DialogPrompt::setOption(int index, DialogOption* option)
 *
 * Sets the option at the given \a{index} within the \l{options} list to \a{option},
 * or deletes it if \a{option} is nil. If \a{index} is one greater than the last element,
 * \a{option} is inserted into the array at that \a{index}.
 * \a{options} list.
 * \sa options, getOption(), addOption()
 */

OOLUA_EXPORT_FUNCTIONS(DialogPrompt,
                       addOption,
                       setOption)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogPrompt,
                             getOption,
                             getOptionCount)


/*!
    \qmltype Dialog
    \since 1.6.0
    \inmodule ESToolkit
    \brief Represents a multibranching, interactive conversation or dialog.

    Dialog can be used to represent a single screen of text, a linear progression of text,
    or a complex, branching series of textboxes with user input and Lua events. It is the input used by
    the DialogSystem for conversations, descriptions, or cutscenes that use the dialog box.

    \section2 Representation - Collection of DialogScreens
    A Dialog object contains an array of DialogScreen objects which are connected together to form a
    dialog tree. These DialogScreen objects can either be of DialogText type, for simply displaying 3
    lines of text, or DialogPrompt type for allowing the user to choose between a list of options. Each
    screen connects to another screen in the array by pointing to it as the next index.

    \section2 Creating a Dialog object
    \e {For configuring Dialog through the ESTk GUI, see \l{Dialog Trees}}.
    \code
    screens = {
        text = "HP and MP fully restored.\nWould you like to save your progress?",
        [1] = DialogPrompt:new {
            options = {
                [1] = DialogOption:new {
                    text = "Yes",
                    nextIndex = 2
                },
                [2] = DialogOption:new {
                    text = "No",
                },
            },
        },
        [2] = DialogText:new {
            text = "Well too goddamn bad! It's not implemented yet!"
        }
    },
    \endcode

    The preceding example will display "HP and MP fully restored." on one line,
    "Would you like to save your progress?" on the next line, then give the user the choice
    between the options, "Yes" and "No." Selecting "No" will immediately end the dialog,
    as the default value of DialogOption::nextIndex is -1, which ends the dialog. Selecting
    "Yes" causes the dialog to continue to screen 2, as that's set as the \l{DialogOption}'s
    nextIndex value. Screen 2 will display "Well too goddamn bad! It's not implemented yet!"
    before exiting, as the default value for DialogText::nextScreen is also -1, which ends
    the dialog.

    \section2 Triggering Lua Events from Dialog
    Events triggered from Dialog use the Behavior::onDialog() trigger to pass the index of the source DialogScreen,
    and the index of the selected DialogOption (if there was one) to Lua, which can then perform any kind of special
    logic, such as adding someone to your party, giving you an item, or triggering a quest event.

    The source code of a minimal script to use this functionality is as follows:
    \code
    --Signify to the engine that the script uses the onDialog() trigger.
    function setScriptAttrib(attrib)
        attrib:addSupportedTrigger(Behavior.ON_DIALOG)
    end

    --[[
        This trigger is called every time a DialogScreen is left with the
        Dialog::trigger property set to true.
    ]]--
    function onDialog(self, screenIndex, optionIndex)
        print("Just completed screen at index ",screenIndex)
        if(optionIndex ~= -1) then -- -1 means no option was selected (Text-only screens)
            print("Just selected option at index ",optionIndex)
        end
    end
    \endcode

    Before this Behavior can be triggered from a Dialog, two more things must be configured.
    \list
    \li Set the Entity with the attached Behavior script as the trigger Entity in the Dialog. See Dialog::setTriggerEntity().
    \li Set the DialogScreen::trigger property to true for every screen that should call into the Lua script.
    \endlist

    Once this has been done, every DialogScreen in the Dialog with its trigger property set to true should result
    in the onDialog() trigger being called for the Entity set as the trigger Entity.

    \section2 Dialog with Lua Trigger Demo Area
    You can find a live demo area with a sample dialog and Lua events here: \l {file:/assets/level/Examples/area/DialogEventDemo}
    The Lua Behavior script containing the Dialog event handlers is here: \l {file:/assets/level/Examples/area/DialogEventDemo/dialog_event_behav.lua}.
    The Dialog and Behavior have been attached to the SavePoint Entity. Triggering dialog should print the corresponding DialogScreen
    index and DialogOption index when progressing to the next screen after examining the save point.

    \sa {Dialog Trees}, DialogScreen, DialogText, DialogPrompt, DialogSystem
*/
/*!
 * \qmlproperty DialogScreen* Dialog::screens
 *
 * Array of DialogScreen objects representing the dialog. These screens are either of type
 * DialogText or DialogPrompt and are referenced by their array indices.
 * \sa getScreen(), setScreen(), getScreenCount(), DialogText, DialogPrompt
 */
/*!
 * \qmlproperty int Dialog::startScreen
 *
 * Index of the first screen in the dialog sequence. Defaults to the index of the first DialogScreen
 * in the \l{screens} array.
 * \sa getStartScreen(), setStartScreen()
 */
/*!
 * \qmlmethod DialogScreen* Dialog::getScreen(int index)
 *
 * Returns a reference to the DialogScreen object located at \a{index} in the \l{screens} array.
 * \sa screens, setScreen(), getScreenCount()
 */
/*!
 * \qmlmethod void Dialog::setScreen(int index, DialogScreen* screen)
 *
 * Sets the DialogScreen at the given \a{index} within the \l{screens} array to \a{screen} or
 * deletes it if \a{screen} is nil. Transfers ownership of \a{screen} from Lua to the engine.
 * \sa screens, getScreen(), getScreenCount()
 */
/*!
 * \qmlmethod int Dialog::getScreenCount()
 *
 * Returns the size of the \l{screens} array.
 * \sa screens, getScreen(), setScreen()
 */
/*!
 * \qmlmethod int Dialog::getStartScreen()
 *
 * Returns the value of the startScreen property.
 * \sa startScreen, setStartScreen()
 */
/*!
 * \qmlmethod void Dialog::setStartScreen(int screen)
 *
 * Sets the value of the startScreen property to \a{screen}.
 * \sa startScreen, getStartScreen()
 */
/*!
 * \qmlmethod DialogText* Dialog::addTextScreen(string text)
 *
 * Dynamically creates a DialogText object and sets its text property to \a{text},
 * returning a reference to the newly created object.
 * \sa DialogText, getScreen()
 */
/*!
 * \qmlmethod DialogPrompt* Dialog::addPromptScreen(string text)
 *
 * Dynamically creates a DialogPrompt object and sets its text property to \a{text},
 * returning a reference to the newly created object.
 * \sa DialogPrompt, getScreen()
 */
/*!
 * \qmlmethod void Dialog::setTriggerEntity(Entity* entity)
 *
 * Sets the trigger Entity of the dialog to \a{entity}. The trigger Entity is the Entity
 * whose Behavior::onDialog() triggers are called after \l{DialogScreen}s with their
 * DialogScreen::trigger property set to true. This is used for triggering dynamic events
 * from within a Dialog tree.
 * \sa getTriggerEntity()
 */
/*!
 * \qmlmethod Entity* Dialog::getTriggerEntity()
 *
 * Returns a reference to the Entity object set as the trigger.
 * \sa setTriggerEntity()
 */

OOLUA_EXPORT_FUNCTIONS(Dialog,
                       addTextScreen,
                       addPromptScreen,
                       setTriggerEntity,
                       setStartScreen,
                       setScreen)
OOLUA_EXPORT_FUNCTIONS_CONST(Dialog,
                             getScreen,
                             getScreenCount,
                             getTriggerEntity,
                             getStartScreen)

