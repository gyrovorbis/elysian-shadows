#ifndef OOLUA_CPP_CONSTRUCTOR_H_
#	define OOLUA_CPP_CONSTRUCTOR_H_

///////////////////////////////////////////////////////////////////////////////
///  @file cpp_constructor.h
///  @author Liam Devine
///  \copyright 
///  See licence.txt for more details. \n 
///////////////////////////////////////////////////////////////////////////////
#	include "lua_includes.h"
#	include "oolua_storage.h"
#	include "param_traits.h"
#	include "oolua_userdata.h"
#	include "member_func_helper.h"
#	include "oolua_parameter_helper.h"
#	include "oolua_converters.h"

#include "oolua_boilerplate.h"

#if defined __GNUC__ && defined __STRICT_ANSI__ 
/*shhhhh about va args and c99*/
#	pragma GCC system_header
#endif

/** \cond INTERNAL*/
 
namespace OOLUA
{
	namespace INTERNAL
	{
		template<typename Type,int HasNoDefaultTypedef>
		struct Constructor
		{
			static int construct(lua_State * l)
			{
				Type* obj = new Type;
				add_ptr(l,obj,false,Lua);
				return 1;
			}
		};
		
		template<typename Type>
		struct Constructor<Type,1>
		{
			static int construct(lua_State * l)
			{
				luaL_error(l,"%s %s %s","No parameters passed to the constructor of the type"
						   ,OOLUA::Proxy_class<Type>::class_name
						   ,"which does not have a default constructor.");
				return 0;//required by function sig yet luaL_error never returns
			}
		};
		
		template<typename T>
		inline int oolua_generic_default_constructor(lua_State* l)
		{
			int const stack_count = lua_gettop(l);
			if(stack_count == 0 )
			{
				return Constructor<T,has_typedef<OOLUA::Proxy_class<T>, OOLUA::No_default_constructor>::Result>::construct(l);
			} 
			luaL_error(l,"%s %d %s %s","Could not match",stack_count,"parameter constructor for type",OOLUA::Proxy_class<T>::class_name);
			return 0;/*required by function sig yet luaL_error never returns*/
		}
	}
}

#define OOLUA_CONSTRUCTOR_GENERATE_NUM(NUM) \
namespace OOLUA \
{ \
	namespace INTERNAL \
	{ \
		template<typename Class OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
		struct Constructor##NUM \
		{ \
			static int construct(lua_State* l) \
			{ \
				int index(1); \
				if( OOLUA_CONSTRUCTOR_PARAM_IS_OF_TYPE_##NUM ) \
				{ \
					valid_construct(l); \
					return 1; \
				} \
				return 0; \
			} \
			static void valid_construct(lua_State* l) \
			{ \
				int index(1); \
				OOLUA_CONSTRUCTOR_PARAM_##NUM \
				Class* obj = new Class( OOLUA_CONVERTER_PARAMS_##NUM ); \
				add_ptr(l,obj,false,Lua); \
			} \
		}; \
	} \
}


OOLUA_INTERNAL_CONSTRUCTORS_GEN


#define OOLUA_CONSTRUCTORS_BEGIN \
static int oolua_factory_function(lua_State* l) \
{ \
	int const stack_count = lua_gettop(l);

/** \endcond*/

/** \addtogroup OOLuaDSL
 @{
	\def OOLUA_CTOR
	\hideinitializer
	\brief Generates a constructor in a constructor block \see OOLUA_CTORS
	\details
	OOLUA_CTOR( ConstructorParameterList)
	\param ConstructorParameterList Comma seperated list of parameters
	\pre Size of ConstructorParameterList >0 and <= \ref OOLuaConfigConstructorParams "\"constructor_params\""
	\see \ref OOLuaConfigConstructorParams "constructor_params"
 */
#define OOLUA_CTOR(...) \
	MSC_PUSH_DISABLE_CONDITIONAL_CONSTANT_OOLUA \
	if( (stack_count == OOLUA_NARG(__VA_ARGS__) && TYPELIST::IndexOf<Type_list< __VA_ARGS__ >::type, calling_lua_state>::value == -1) \
	|| (stack_count == OOLUA_NARG(__VA_ARGS__)-1 && TYPELIST::IndexOf<Type_list< __VA_ARGS__ >::type, calling_lua_state>::value != -1) ) \
	{ \
		if( OOLUA_VA_CONSTRUCTOR(__VA_ARGS__)<class_ VA_PARAM_TYPES(__VA_ARGS__) >::construct(l) ) return 1; \
	} \
	MSC_POP_COMPILER_WARNING_OOLUA
/**@}*/

/** \cond INTERNAL*/
#define OOLUA_CONSTRUCTORS_END \
	if(stack_count == 0 ) \
	{ \
		return INTERNAL::Constructor<class_,INTERNAL::has_typedef<this_type, No_default_constructor>::Result>::construct(l); \
	} \
	luaL_error(l,"%s %d %s %s","Could not match",stack_count,"parameter constructor for type",class_name); \
	return 0;/*required by function sig yet luaL_error never returns*/  \
} \
	typedef class_ ctor_block_check;

/** \endcond*/

/** \addtogroup OOLuaDSL
@{
	\def OOLUA_CTORS
	\hideinitializer
	\brief Creates a block into which none default constructors can be defined with \ref OOLUA_CTOR
	\details OOLUA_CTORS(ConstructorEntriesList)
	\param ConstructorEntriesList List of \ref OOLUA_CTOR
	<p>
	To enable the construction of an instance which is a specific type, there must be 
	constructor(s) for that type registered with OOLua. \ref OOLUA_CTORS is the block into 
	which you can define none default constructor entries using \ref OOLUA_CTOR.
	<p>
	Constructors are the only real type of overloading which is permitted by OOLua 
	and there is an important point which should be noted. OOLua will try and match
	the number of parameters on the stack with the amount required by each OOLUA_CTOR
	entry and will look in the order they were defined. When interacting with the Lua
	stack certain types can not be differentiated between, these include some integral
	types such as float, int, double etc and types which are of a proxy class type or
	derived from that type. OOLua implicitly converts between classes in a hierarchy
	even if a reference is required. This means for example that if there are constructors
	such as Foo::Foo(int) and Foo::Foo(float) it will depend on which was defined first
	in the OOLUA_CTORS block as to which will be invoked for a call such as Foo.new(1).
 
	\see \ref OOLUA::No_default_constructor "No_default_constructor"
	\note An OOLUA_CTORS block without any \ref OOLUA_CTOR entries is invalid.
*/
#	define OOLUA_CTORS(ConstructorEntriesList) OOLUA_CONSTRUCTORS_BEGIN ConstructorEntriesList OOLUA_CONSTRUCTORS_END
/**@}*/


#endif 
