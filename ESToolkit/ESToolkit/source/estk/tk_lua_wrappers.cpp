#include "tk_lua_wrappers.h"
#include "tk_lua_manager.h"
#include <asset/tk_lua_table_proxy.h>
#include <proxy/elysian_entity_proxy.hpp>
//#include <model/tk_property_item_delegate.h>

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

    \sa Engine, Behavior
*/
EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Toolkit)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Toolkit)

/*!
 * \qmlmethod void Toolkit::setSelection(LuaVariant obj)
 * Sets the current Toolkit selection to \a{obj}, clearing the previous selection, updating the active layers,
 * and displaying it within \l{SelectionView}. The selection is cleared if \a{obj} is nil.
 *
 * Anything that can be iterated over as a Lua table is a valid selectable and will be handled generically.
 * If an Engine class/object has not been exported to Lua, it does not implement the Lua table metamethods and
 * thus cannot be selected, viewed, and modified generically through \l{SelectionView}.
 * and will not be expandable. The following types are built-in selection types that are handled uniquely
 * by ESTk (rather than as generic Lua tables)
 * 	\list
 *  \li \l{Level}
 *  \li \l{Area}
 *  \li \l{Entity}
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
 * \qmlmethod void Toolkit::searchDocs(string name)
 * Opens the ESTk documentation and performs a search on the query given by \a{name}. This is to conveniently
 * pull up the documentation for methods, types, and classes from the \l{Debugging#Real-Time Lua Terminal}{Real-Time Lua Terminal}.
 */

static inline int Toolkit_searchDocs(lua_State* const vm) {
    QString qstr;
    if(OOLUA::pull(vm, qstr)) {
        ESToolkit::searchDocs(qstr);
    }
    return 0;
}

static inline int Toolkit_openDocs(lua_State* const vm) {
    QString page, section;
    if(OOLUA::pull(vm, page)) {
        OOLUA::pull(vm, section);
        ESToolkit::openDoc(page, section);
    }
    return 0;
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



void TkLuaManager::_registerWrappers(void) {
    elysian::LuaManagerBase::_registerWrappers();

    _mainState->register_class<Toolkit>();
    _mainState->register_class<PropertyItemDelegate>();
    _mainState->register_class<EnumDropdownPropertyItemDelegate>();
    _mainState->register_class<SpinboxPropertyItemDelegate>();
    _mainState->register_class<FileBrowserPropertyItemDelegate>();
    _mainState->register_class_static<Toolkit>("setSelection", &Toolkit_setSelection);
    _mainState->register_class_static<Toolkit>("getSelection", &ESToolkit::pushSelection);
    _mainState->register_class_static<Toolkit>("findEntity", &Toolkit_findEntity);
    _mainState->register_class_static<Toolkit>("searchDocs", &Toolkit_searchDocs);
    _mainState->register_class_static<Toolkit>("openDocs", &Toolkit_openDocs);

    luaRegisterTkEnumsConstants(_mainState);
    TkLuaTableUserdata::staticInitialize();

}

