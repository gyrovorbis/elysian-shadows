#include <ESToolkit/ESGamma/tk_lua_wrappers.h>
//#include <ESGamma/lua/elysian_lua_wrappers.hpp>
#include <ESToolkit/ESGamma/tk_lua_manager.h>
#include <ESToolkit/AssetIO/tk_lua_table_proxy.h>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
//#include <model/tk_property_item_delegate.h>

static inline int alertprint(lua_State* const l) {
    QMessageBox::critical(nullptr, "Alert", elysian::LuaManager::getVariadicStringFromStack());
    return 0;
}

/*!
    \qmltype Toolkit
    \since 1.6.1
    \inmodule Toolkit
    \brief Top-level static class for accessing Toolkit utilities

    The Toolkit class for ESTk is analogous ot the \l{Engine} class for ESGamma.
    It's a top-level Singleton-like, noninstantiable class containing only static
    methods for manipulating ESTk. Most of these are convenience utilities to be
    used within the \l{Debugging#Real-Time Lua Terminal}{Real-Time Lua Terminal} during development.

    \e {NOTE: The Toolkit class and its functionality ONLY exists in ESTk builds of Elysian Shadows,
        meaning it will not be accessible in standalone and release builds of the engine. Attempting to
        access it will result in an error. As such, limit usage to only Toolkit-specific portions of Lua
        code such as Toolkit-only Lua triggers and the \l{Debugging#Real-Time Lua Terminal}{Real-Time Lua Terminal},
        so that your code remains compatible with standalone ES builds.}

    \e {NOTE: ALL methods are static!}

    \section2 Example Interactive Dialog Box
      \code
           local retVal = Toolkit.messageBoxQuestion("What is the best Engine?",
                                                     { "Unity3D", "Unreal", "Elysian Shadows Toolkit" },
                                                     "Because it's an absolutely veeeery important question!")

           if(retVal == "Elysian Shadows Toolkit") then
               print("CORRECT ANSWER!")
           else
               print("Uhm... no,", retVal, "is wrong!")
           end

      \endcode

    \sa Engine, Behavior
*/
EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Toolkit)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Toolkit)

/*!
 * \qmlmethod void Toolkit::setSelection(LuaVariant obj)
 * Sets the current Toolkit selection to \a{obj}, clearing the previous selection, updating the active layers,
 * and displaying it within \l{Selection View}. The selection is cleared if \a{obj} is nil.
 *
 * Anything that can be iterated over as a Lua table is a valid selectable and will be handled generically.
 * If an Engine class/object has not been exported to Lua, it does not implement the Lua table metamethods and
 * thus cannot be selected, viewed, and modified generically through \l{Selection View}.
 * and will not be expandable. The following types are built-in selection types that are handled uniquely
 * by ESTk (rather than as generic Lua tables)
 * 	\list
 *  \li \l{Level}
 *  \li \l{Area}
 *  \li \l{Entity}
 * \endlist
 *
 *   \sa getSelection
 */

static inline int Toolkit_setSelection(lua_State* const /*vm*/) { //this could really support multi-select with arbitrary # of arguments...
    ESToolkit::select(LuaVariant::fromStack());
    return 0;
}

/*!
 * \qmlmethod LuaVariant Toolkit::getSelection()
 * Returns a Lua reference to the object set as the current selection within ESTk, or nil if nothing is selected.
 * \sa setSelection
 */
//ESToolkit::pushSelection(), apparently
/*!
 * \qmlmethod Entity* Toolkit::findEntity(string name)
 * Searches through the current \l{Area} to find an \l{Entity}, returning a reference to it if found or nil otherwise.
 *
 * Convenience shortcut for:
 * \c { Engine.getCurArea():findEntity(name) }
 * \sa Area, Entity
 */
static inline int Toolkit_findEntity(lua_State* const vm) {
    QString name;
    OOLUA::pull(vm, name);
    auto* entity = Debug::getCurrentArea()->findEntity(Q_CSTR(name));
    OOLUA::push(vm, entity);
    return entity? 1 : 0;
}

/*!
 * \qmlmethod void Toolkit::showStatusBarMessage(string message)
 * Displays the given \a{message} on the Status Bar of the main ESToolkit window, temporarily hiding the other information. You can use this for
 * global status alerts to the user or for debugging. After the timeout is expired (2 seconds default), the normal widgets and information
 * are restored to the status bar.
 */
static inline int Toolkit_showStatusBarMessage(lua_State* const vm) {
    QString qstr;
    if(OOLUA::pull(vm, qstr)) {
        ESToolkit::showStatusBarMessage(qstr);
    }
    return 0;
}


/*!
 * \qmlmethod void Toolkit::searchDocs(string name)
 * Opens the ESTk documentation and performs a search on the query given by \a{name}. This is to conveniently
 * pull up the documentation for methods, types, and classes from the \l{Debugging#Real-Time Lua Terminal}{Real-Time Lua Terminal}.
 * \sa openDocs
 */

static inline int Toolkit_searchDocs(lua_State* const vm) {
    QString qstr;
    if(OOLUA::pull(vm, qstr)) {
        ESToolkit::searchDocs(qstr);
    }
    return 0;
}

/*!
 * \qmlmethod void Toolkit::openDocs(string name)
 * Opens the ESTk documentation and sets the current page to the given \a{name}.
 * \sa searchDocs
 */
static inline int Toolkit_openDocs(lua_State* const vm) {
    QString page, section;
    if(OOLUA::pull(vm, page)) {
        OOLUA::pull(vm, section);
        ESToolkit::openDoc(page, section);
    }
    return 0;
}

/*!
 * \qmlmethod LuaVariant Toolkit::messageBoxQuestion(string message, array buttonNames, string informativeText)
 * Pops a modal dialog box with text given by \a{message} and push buttons on the bottom whose text comes
 * from the array of string names given by \a{buttonNames}. \a{InformativeText} is a submessage beneath \a{message}
 * containing more detailed information. The value to be returned is the string name of the button pressed by the user,
 * matching the button's corresponding \a{buttonNames} entry. If no button has been pressed (see below), the return value
 * will be nil, rather than a string.
 *
 *  \e { NOTE: Both \a{buttonNames} and \a{informativeText} are optional parameters, and both can optionally be
 *      omitted or explicitly set to nil. When \a{buttonNames} is nil, there will simply be no buttons displayed at the
 *      bottom of the dialog, and the return value will always be nil. When \a{informativeText} is nil, there is simply
 *      no detailed text, and the only text to be displayed is given by \a{message}. }
 *
 * \sa messageBoxTextPrompt
 */
static inline int Toolkit_messageBoxQuestion(lua_State* const vm) {
    QString msg;
    LuaVariant buttons;
    QString informativeText;

    int depth = LuaManager::stackDepth();
    if(depth >= 3) {
        OOLUA::pull(vm, informativeText);
    }
    if(depth >= 2) {
        OOLUA::pull(vm, buttons);
    }

    if(depth >= 1) {
        OOLUA::pull(vm, msg);
    }

    QString retVal = ESToolkit::messageBoxQuestion(msg, buttons, informativeText);

    if(!retVal.isEmpty()) {
        OOLUA::push(vm, retVal);
        return 1;
    }

    return 0;
}

/*!
 * \qmlmethod LuaVariant Toolkit::messageBoxTextPrompt(string message)
 * Pops a modal dialog box with the given \a {message} text with a Text Edit field
 * for the user to specify an arbitary string. The value is returned as a LuaVariant,
 * meaning it will be converted to boolean, nil, numbers, or any other Lua type if applicable,
 * otherwise the default will return as a string type.
 *
 * \sa messageBoxQuestion
 */
static inline int Toolkit_messageBoxTextPrompt(lua_State* const vm) {
    QString msg;
    OOLUA::pull(vm, msg);
    LuaVariant variant = ESToolkit::messageBoxTextPrompt(msg);
    return OOLUA::push(vm, variant);
}

/*!
    \qmltype PropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(PropertyItemDelegate)
EXPORT_OOLUA_FUNCTIONS_0_CONST(PropertyItemDelegate)

/*!
    \qmltype EnumDropdownPropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
OOLUA_EXPORT_FUNCTIONS_CONST(EnumDropdownPropertyItemDelegate)
OOLUA_EXPORT_FUNCTIONS(EnumDropdownPropertyItemDelegate,
                       insertEnumValue)

/*!
    \qmltype SpinboxPropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
OOLUA_EXPORT_FUNCTIONS_CONST(SpinboxPropertyItemDelegate)
OOLUA_EXPORT_FUNCTIONS(SpinboxPropertyItemDelegate,
                       setMaxValue,
                       setMinValue)

/*!
    \qmltype FileBrowserPropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
OOLUA_EXPORT_FUNCTIONS_CONST(FileBrowserPropertyItemDelegate)
OOLUA_EXPORT_FUNCTIONS(FileBrowserPropertyItemDelegate)

/*!
    \qmltype ComboboxPropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
OOLUA_EXPORT_FUNCTIONS_CONST(ComboBoxPropertyItemDelegate)
OOLUA_EXPORT_FUNCTIONS(ComboBoxPropertyItemDelegate)

/*!
    \qmltype DynamicContentPropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
OOLUA_EXPORT_FUNCTIONS_CONST(DynamicContentPropertyItemDelegate)
OOLUA_EXPORT_FUNCTIONS(DynamicContentPropertyItemDelegate)

/*!
    \qmltype ColorSwatchPropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
OOLUA_EXPORT_FUNCTIONS_CONST(QESTkUtils::ColorSwatchPropertyItemDelegate<Color4>)
OOLUA_EXPORT_FUNCTIONS(QESTkUtils::ColorSwatchPropertyItemDelegate<Color4>)

OOLUA_EXPORT_NO_FUNCTIONS(QESTkUtils::ColorSwatchPropertyItemDelegate<Vector4>)

/*!
    \qmltype PlainTextEditPropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
OOLUA_EXPORT_NO_FUNCTIONS(QESTkUtils::PlainTextEditPropertyItemDelegate)

/*!
    \qmltype SliderPropertyItemDelegate
    \since 1.6.1
    \inmodule Toolkit
    \brief Delegate
*/
OOLUA_EXPORT_NO_FUNCTIONS(QESTkUtils::SliderPropertyItemDelegate)

void TkLuaManager::_registerWrappers(void) {
    elysian::LuaManagerBase::_registerWrappers();

    lua_register(*_mainState, "alertprint", &alertprint);
    _mainState->register_class<Toolkit>();
    _mainState->register_class<PropertyItemDelegate>();
    _mainState->register_class<EnumDropdownPropertyItemDelegate>();
    _mainState->register_class<SpinboxPropertyItemDelegate>();
    _mainState->register_class<FileBrowserPropertyItemDelegate>();
    _mainState->register_class<QESTkUtils::ColorSwatchPropertyItemDelegate<Color4>>();
    _mainState->register_class_static<Toolkit>("setSelection", &Toolkit_setSelection);
    _mainState->register_class_static<Toolkit>("getSelection", &ESToolkit::pushSelection);
    _mainState->register_class_static<Toolkit>("findEntity", &Toolkit_findEntity);
    _mainState->register_class_static<Toolkit>("searchDocs", &Toolkit_searchDocs);
    _mainState->register_class_static<Toolkit>("openDocs", &Toolkit_openDocs);
    _mainState->register_class_static<Toolkit>("showStatusBarMessage", &Toolkit_showStatusBarMessage);
    _mainState->register_class_static<Toolkit>("messageBoxQuestion", &Toolkit_messageBoxQuestion);
    _mainState->register_class_static<Toolkit>("messageBoxTextPrompt", &Toolkit_messageBoxTextPrompt);

    luaRegisterTkEnumsConstants(_mainState);
    TkLuaTableUserdata::staticInitialize();

}

