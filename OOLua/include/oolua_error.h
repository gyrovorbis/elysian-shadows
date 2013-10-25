#ifndef OOLUA_ERROR_H_
#   define OOLUA_ERROR_H_

#include "oolua_config.h"

#if OOLUA_USE_EXCEPTIONS == 1
#   include "oolua_exception.h"
#endif

#if OOLUA_DEBUG_CHECKS == 1
#   include <cassert>
#endif



#   include <string>
struct lua_State;
namespace OOLUA
{
	/** \addtogroup OOLuaErrorReporting
	@{*/
	
    /** \brief Reset the error state such that a call to OOLUA::get_last_error
		will return an empty string.
		\see OOLuaErrorReporting
		\note This function is a nop when \ref OOLUA_STORE_LAST_ERROR is not enabled
	*/
	void reset_error_value(lua_State*l);
    /** \brief Returns the last stored error
		\return empty string if there is not an error else the error message
		\see OOLuaErrorReporting
		\note This function is a nop when \ref OOLUA_STORE_LAST_ERROR is not enabled
	*/
    std::string get_last_error(lua_State*l);
	/**@}*/
	
	/** \cond INTERNAL*/
    namespace INTERNAL
    {
		void set_error_from_top_of_stack_and_pop_the_error(lua_State*l);
    }
	/** \endcond */

}

#endif

