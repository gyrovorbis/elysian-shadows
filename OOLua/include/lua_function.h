#ifndef LUA_FUNCTION_H_
#	define LUA_FUNCTION_H_

///////////////////////////////////////////////////////////////////////////////
///  @file lua_function.h
///  @author Liam Devine
///  \copyright 
///  See licence.txt for more details. \n 
///////////////////////////////////////////////////////////////////////////////
#	include "lua_includes.h"
#	include "fwd_push_pull.h"
#	include "lua_ref.h"
#	include "oolua_boilerplate.h"
namespace OOLUA
{

/** \cond INTERNAL */	
#if OOLUA_USE_EXCEPTIONS ==1 
#	define OOLUA_CALLER_TRY \
	try \
	{
#	define OOLUA_CALLER_HANDLE_FAIL \
	} \
	catch (...) \
	{ \
		lua_settop(m_lua, top); \
		throw; \
	} \
	return false; /*Shhhhhhhhhh*/
#else
#	define OOLUA_CALLER_TRY 
#	define OOLUA_CALLER_HANDLE_FAIL \
	lua_settop(m_lua, top); \
	return false; 
#endif
	
#define OOLUA_INTERNAL_FUNC_OPERATOR(NUM) \
	template<typename FUNC_TYPE OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	bool operator()(FUNC_TYPE const& func OOLUA_FCALL_PARAM_##NUM) \
	{ \
		int error_index(0); \
		const int top = get_top(); \
		OOLUA_CALLER_TRY \
		if( prep_function(func,NUM,error_index) \
				OOLUA_FCALL_PUSH_##NUM \
		) return call(NUM,error_index); \
		OOLUA_CALLER_HANDLE_FAIL \
	}
	
/** \endcond */	

	
	
	
	
/**
	\struct Lua_function
	\brief
	Struct which is used to call a Lua function.
	\details
	\ref Lua_function is a lua_State function caller object, the state in which
	it calls a function is specified in either the \ref Lua_function(lua_State*) 
	"constructor" or via \ref bind_script.
	This object provides function call operator overloads upto 
	\ref OOLuaConfigLuaParams "\"lua_params\"" count + 1 parameters the first of which being
	the function which is to be called and it's type maybe one of:
		\li std::string A function in Lua's global table
		\li \ref OOLUA::Lua_func_ref A reference to a function
		\li int A valid stack index
*/
struct Lua_function
{
	/** \brief Default constructor initialises the object
		\post Any call to a function call operator will cause
		an error until a lua_State is bound via \ref bind_script
	*/
	Lua_function();
	/** \brief Binds the state l to this instance */
	Lua_function(lua_State* l);

	/**@{*/
	/**
	 \brief Function call operator
	 \return Result indicating success
	 \tparam FUNC_TYPE
	 \see \ref OOLuaErrorReporting
	 */
	OOLUA_INTERNAL_FUNC_OPERATORS
	/**@}*/
	
	/** \brief Sets the state in which functions will be called*/
	void bind_script(lua_State* const lua);
private:
	int get_top();
	bool call(int const nparams, int const error_index);
	bool prep_function(Lua_func_ref const& func,int const nparams, int& error_index);
	bool prep_function(std::string const& func,int const nparams, int& error_index);
	bool prep_function(int const func,int const nparams, int& error_index);
	lua_State* m_lua;

};
}
#undef OOLUA_CALLER_TRY
#undef OOLUA_CALLER_HANDLE_FAIL


#endif 
