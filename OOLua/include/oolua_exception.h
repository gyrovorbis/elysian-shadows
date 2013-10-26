///////////////////////////////////////////////////////////////////////////////
///  @file oolua_exception.h
///  @author Liam Devine
///  \copyright
///  See licence.txt for more details.
///////////////////////////////////////////////////////////////////////////////
#ifndef OOLUA_EXCEPTION_H_
#	define OOLUA_EXCEPTION_H_


#include "oolua_config.h"

/** \addtogroup OOLuaErrorReporting
@{*/
	/* \addtogroup OOLuaExceptionClasses Exception classes
	*/
	/** \struct std::runtime_error */

	/** \struct OOLUA::Syntax_error
		\brief Reports LUA_ERRSYNTAX
		\implements std::runtime_error
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::Runtime_error
		\brief Reports LUA_ERRRUN
		\extends std::runtime_error
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::Memory_error
		\brief Reports LUA_ERRMEM
		\extends std::runtime_error
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::File_error
		\brief Reports LUA_ERRFILE
		\extends std::runtime_error
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::Type_error
		\brief Reports that a type pulled from the stack was not the type that was asked for
		\extends std::runtime_error
		\see OOLuaErrorReporting
		\note Implicit casts such as a derived class to a base class are not type errors
	 */
	

/**@}*/

#if OOLUA_USE_EXCEPTIONS == 1

#	include "lua_includes.h"
#	include <stdexcept>
    namespace OOLUA
    {
		namespace ERROR 
		{
			struct PopTheStack{};
		}

        struct Syntax_error : public std::runtime_error
        {
            Syntax_error(lua_State* s)
                :std::runtime_error( lua_tostring(s, -1) )
            {}
			Syntax_error(lua_State* s,ERROR::PopTheStack*)
				:std::runtime_error( lua_tostring(s, -1) )
            {
				lua_pop(s,1);
			}
        };

        struct Runtime_error : public std::runtime_error
        {
            Runtime_error(lua_State* s)
                :std::runtime_error( lua_tostring(s, -1) )
            {}
			Runtime_error(lua_State* s,ERROR::PopTheStack*)
				:std::runtime_error( lua_tostring(s, -1) )
            {
				lua_pop(s,1);
			}
            Runtime_error(std::string const& str)
				:std::runtime_error( str )
            {}
        };

        struct Memory_error : public std::runtime_error
        {
            Memory_error(lua_State* s)
                :std::runtime_error( lua_tostring(s, -1) )
            {}
			Memory_error(lua_State* s,ERROR::PopTheStack*)
				:std::runtime_error( lua_tostring(s, -1) )
            {
				lua_pop(s,1);
			}
        };

        struct File_error : public std::runtime_error
        {
            File_error(lua_State* s)
                :std::runtime_error( lua_tostring(s, -1) )
            {}
			File_error(lua_State* s,ERROR::PopTheStack*)
				:std::runtime_error( lua_tostring(s, -1) )
            {
				lua_pop(s,1);
			}
        };

        struct Type_error : public std::runtime_error
        {
            Type_error(std::string const& str)
                :std::runtime_error( str )
            {}
        };

    }
# endif

#endif
