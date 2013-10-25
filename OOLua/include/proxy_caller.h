#ifndef PROXY_CALLER_H_
#	define PROXY_CALLER_H_

///////////////////////////////////////////////////////////////////////////////
///  @file proxy_caller.h
///  @remarks Warning this file was generated, edits to it will not persist if 
///  the file is regenerated.
///  @author Liam Devine
///  \copyright 
///  See licence.txt for more details. \n 
///////////////////////////////////////////////////////////////////////////////
#	include "member_func_helper.h"
#	include "param_traits.h"
#	include "oolua_converters.h"

#	include "oolua_boilerplate.h"

/** \cond INTERNAL*/

#	define OOLUA_INTERNAL_MEMBER_CALLER_NUM(NUM) \
	template<typename FuncType  OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	static void call(lua_State* const l,C* m_this,FuncType ptr2mem OOLUA_PULL_TYPE_PARAMS_##NUM ) \
	{ \
		OOLUA_CONVERTER_##NUM \
		typename R::type r( (m_this->*ptr2mem) (OOLUA_CONVERTER_PARAMS_##NUM) ); \
		Member_func_helper<R,R::owner>::push2lua(l,r); \
	}

#	define OOLUA_INTERNAL_MEMBER_CALLER_NO_RETURN_NUM(NUM) \
	template<typename FuncType  OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	static void call(lua_State* const /*l*/,C* m_this,FuncType ptr2mem OOLUA_PULL_TYPE_PARAMS_##NUM ) \
	{ \
		OOLUA_CONVERTER_##NUM \
		(m_this->*ptr2mem) (OOLUA_CONVERTER_PARAMS_##NUM); \
	}

#	define OOLUA_INTERNAL_C_CALLER_NUM(NUM) \
	template<typename FuncType  OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	static void call(lua_State* const l,FuncType ptr2func OOLUA_PULL_TYPE_PARAMS_##NUM ) \
	{ \
		OOLUA_CONVERTER_##NUM \
		typename R::type r( (*ptr2func) (OOLUA_CONVERTER_PARAMS_##NUM) ); \
		Member_func_helper<R,R::owner>::push2lua(l,r); \
	}

#	define OOLUA_INTERNAL_C_CALLER_NO_RETURN_NUM(NUM) \
	template<typename FuncType  OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	static void call(lua_State* const /*l*/,FuncType ptr2func OOLUA_PULL_TYPE_PARAMS_##NUM ) \
	{ \
		OOLUA_CONVERTER_##NUM \
		(*ptr2func) (OOLUA_CONVERTER_PARAMS_##NUM); \
	}


namespace OOLUA
{
	namespace INTERNAL
	{
		template <typename Return, typename Class, int ReturnIsVoid>struct Proxy_caller;
		template <typename Return, int ReturnIsVoid>struct Proxy_none_member_caller;
		
		
		template <typename R, typename C>
		struct Proxy_caller<R,C, 0 >
		{
			OOLUA_INTERNAL_MEMBER_CALLER_FUNCS
		};
		
		template <typename R, typename C>
		struct Proxy_caller<R,C, 1 >
		{
			OOLUA_INTERNAL_MEMBER_CALLER_NO_RETURN_FUNCS	
		};
		
		template <typename R> 
		struct Proxy_none_member_caller<R,0 > 
		{
			OOLUA_INTERNAL_C_CALLER_FUNCS
		};

		template <typename R >
		struct Proxy_none_member_caller<R, 1 >
		{
			OOLUA_INTERNAL_C_CALLER_NO_RETURN_FUNCS
		};
		
	}//end INTERNAL namespace
}//end OOLUA namespace

#undef OOLUA_INTERNAL_MEMBER_CALLER_NUM
#undef OOLUA_INTERNAL_MEMBER_CALLER_NO_RETURN_NUM
#undef OOLUA_INTERNAL_C_CALLER_NUM
#undef OOLUA_INTERNAL_C_CALLER_NO_RETURN_NUM


/**\endcond*/
#endif 
