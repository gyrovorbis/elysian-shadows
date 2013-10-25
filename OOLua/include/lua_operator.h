///////////////////////////////////////////////////////////////////////////////
///  @file lua_operator.h
///  Defines operators which are available in scripts in respect to Proxy_class
///  @author Liam Devine
///  \copyright
///  See licence.txt for more d etails. \n 
///////////////////////////////////////////////////////////////////////////////

#ifndef LUA_OPERATOR_H_
#	define LUA_OPERATOR_H_

#	include "lua_includes.h"
#	include "fwd_push_pull.h"
#   include "push_pointer_internal.h"
#   include "type_list.h"
#	include "oolua_userdata.h"
#	include "oolua_storage.h"

namespace OOLUA
{
	namespace INTERNAL 
	{
		//rhs is top of stack lhs is below (lhs op rhs)
		template<typename T>
		int lua_equal(lua_State*  const l)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::pull2cpp(l,rhs);
			INTERNAL::LUA_CALLED::pull2cpp(l,lhs);
			bool result (*lhs == *rhs);
			OOLUA::push(l,result);
			return 1;
		}

		template<typename T>
		int lua_less_than(lua_State*  const l)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::pull2cpp(l,rhs);
			INTERNAL::LUA_CALLED::pull2cpp(l,lhs);
			bool result (*lhs < *rhs);
			OOLUA::push(l,result);
			return 1;
		}

		template<typename T>
		int lua_less_than_or_equal(lua_State*  const l)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::pull2cpp(l,rhs);
			INTERNAL::LUA_CALLED::pull2cpp(l,lhs);
			bool result (*lhs <= *rhs);
			OOLUA::push(l,result);
			return 1;
		}

		//these following operator functions return a type that they are working on
		template<typename T>
		int lua_add(lua_State*  const l)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::pull2cpp(l,rhs);
			INTERNAL::LUA_CALLED::pull2cpp(l,lhs);
			T* result ( new T( *lhs + *rhs ) );
			INTERNAL::add_ptr<T>(l,result,false,OOLUA::Lua);
			return 1;
		}


		template<typename T>
		int lua_sub(lua_State*  const l)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::pull2cpp(l,rhs);
			INTERNAL::LUA_CALLED::pull2cpp(l,lhs);
			T* result ( new T( *lhs - *rhs ) );
			INTERNAL::add_ptr<T>(l,result,false,OOLUA::Lua);
			return 1;
		}

		template<typename T>
		int lua_mul(lua_State*  const l)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::pull2cpp(l,rhs);
			INTERNAL::LUA_CALLED::pull2cpp(l,lhs);
			T* result ( new T( *lhs * *rhs ) );
			INTERNAL::add_ptr<T>(l,result,false,OOLUA::Lua);
			return 1;
		}

		template<typename T>
		int lua_div(lua_State*  const l)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::pull2cpp(l,rhs);
			INTERNAL::LUA_CALLED::pull2cpp(l,lhs);
			T* result ( new T( *lhs / *rhs ) );
			INTERNAL::add_ptr<T>(l,result,false,OOLUA::Lua);
			return 1;
		}

#define DEFINE_OOLUA_OPERATOR_FUNCTION_(operation,lua_string_op)\
template<typename T, bool hasOperator >\
struct set_ ## operation ## _function\
{\
 	   static void set(lua_State*  const /*l*/,int /*const_metatable*/,int /*none_const_metatable*/){}\
};\
template<typename T>\
struct set_ ## operation ## _function<T, true> \
{\
 	   static void set(lua_State*  const l,int const_metatable,int none_const_metatable)\
 	   {\
			  lua_pushcfunction(l, lua_## operation<T>);\
			  int func = lua_gettop(l);\
			  lua_pushliteral(l, lua_string_op);\
			  int op_string = lua_gettop(l);\
			  lua_pushvalue(l,op_string);\
			  lua_pushvalue(l,func);\
			  lua_settable(l, const_metatable);\
			  lua_pushvalue(l,op_string);\
			  lua_pushvalue(l,func);\
			  lua_settable(l, none_const_metatable);\
			  lua_remove(l,op_string);\
			  lua_remove(l,func);\
       }\
};

DEFINE_OOLUA_OPERATOR_FUNCTION_(equal,"__eq")
DEFINE_OOLUA_OPERATOR_FUNCTION_(less_than,"__lt")
DEFINE_OOLUA_OPERATOR_FUNCTION_(less_than_or_equal,"__le")
DEFINE_OOLUA_OPERATOR_FUNCTION_(add,"__add")
DEFINE_OOLUA_OPERATOR_FUNCTION_(sub,"__sub")
DEFINE_OOLUA_OPERATOR_FUNCTION_(mul,"__mul")
DEFINE_OOLUA_OPERATOR_FUNCTION_(div,"__div")

#undef DEFINE_OOLUA_OPERATOR_FUNCTION_

		template<typename ClassType>
		struct class_or_base_has_tag_block
		{
			template <typename U> 
			static char (& check_for_tag_block(typename OOLUA::Proxy_class<U>::tag_block_check*))[1] ;
			template <typename U> 
			static char (& check_for_tag_block(...))[2];
			enum {value = sizeof( check_for_tag_block<ClassType >(0) ) == 1 ? 1 : 0} ;
		};
		
		template< typename ClassType, int TagBlockMaybeInClass_or_MaybeInABase>
		struct tag_block_is_same
		{
			enum {value = LVD::is_same< typename Proxy_class<ClassType>::tag_block_check,ClassType >::value };
		};
		template< typename ClassType>
		struct tag_block_is_same<ClassType,0>
		{
			enum {value = 0};
		};
		
		template<typename ClassType,int has_proxy_tags>
		struct proxy_tag_type
		{
			typedef typename Proxy_class<ClassType>::Tags type;
		};
		template<typename ClassType>
		struct proxy_tag_type<ClassType,0>
		{
			typedef TYPE::Null_type type;
		};
		
		template< typename ClassType>
		struct tag_type
		{
			enum { has_proxy_tags = tag_block_is_same<ClassType,class_or_base_has_tag_block<ClassType>::value >::value };
			typedef typename proxy_tag_type<ClassType,has_proxy_tags>::type type;
		
		};
		
		template<typename ProxyType,typename Tag>
		struct has_typedef
		{
			enum {Result = TYPELIST::IndexOf<typename tag_type<typename ProxyType::class_>::type,Tag>::value == -1 ? 0 : 1};
		};
	}
	
}

#endif //LUA_OPERATOR_H_
