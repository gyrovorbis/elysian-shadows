///////////////////////////////////////////////////////////////////////////////
///  @file cpp_member_func.h
///  @author Liam Devine
///  \copyright
///  See licence.txt for more details. \n 
///////////////////////////////////////////////////////////////////////////////
#ifndef CPP_MEMBER_FUNC_H_
#	define CPP_MEMBER_FUNC_H_

#	include "param_traits.h"
#	include "oolua_boilerplate.h"
#	include "proxy_caller.h"
#	include "default_trait_caller.h"

#	include <cassert>
#	include "oolua_config.h"

/** \cond INTERNAL*/
#if defined __GNUC__ && defined __STRICT_ANSI__ 
#	pragma GCC system_header
#endif


/*OOLUA_MEMBER_BODY_N generates a member function with at least one parameter*/
#define OOLUA_MEMBER_BODY_N(mod,func_name,return_value,func , ... ) \
int func_name(lua_State* const l) mod \
{ \
	assert(m_this); \
	OOLUA_PARAMS_CONCAT(2,__VA_ARGS__) \
	typedef INTERNAL::return_type_traits<return_value > R; \
	typedef R::type (class_::*funcType )( OOLUA_PARAM_TYPE_CONCAT(__VA_ARGS__) ) mod ; \
	OOLUA::INTERNAL::Proxy_caller<R,class_,LVD::is_void< R::type >::value >::call<funcType OOLUA_TPARAMS_CONCAT(__VA_ARGS__) >(l,m_this,&class_::func OOLUA_PPARAMS_CONCAT(__VA_ARGS__)); \
	OOLUA_BACK_CONCAT(__VA_ARGS__) \
	return INTERNAL::lua_return_count< Type_list<R OOLUA_RETURNS_CONCAT(__VA_ARGS__) >::type> ::out; \
}

/*OOLUA_MEMBER_BODY_0 generates a member function with no parameters*/
#define OOLUA_MEMBER_BODY_0(mod,func_name,return_value,func) \
int func_name(lua_State* const l) mod\
{ \
	assert(m_this); \
	typedef INTERNAL::return_type_traits<return_value > R; \
	typedef R::type (class_::*funcType )() mod ; \
	OOLUA::INTERNAL::Proxy_caller<R,class_,LVD::is_void< R::type >::value >::call< funcType>(l,m_this,&class_::func ); \
	return INTERNAL::lua_return_count< Type_list<R  >::type> ::out; \
}

/*
 OOLUA_MEM_FUNC_N and OOLUA_MEM_FUNC_0
 A member function defined using OOLUA_MEM_FUNC*  (*=  |_CONST | _RENAME | _CONST_RENAME )
 will translate into a corresponding macro below using either a suffix of _N if there are 
 function parameters else _0
*/
#define OOLUA_MEM_FUNC_N(return_value,func , ...)					OOLUA_MEMBER_BODY_N(      ,func,return_value,func , __VA_ARGS__ )
#define OOLUA_MEM_FUNC_CONST_N(return_value,func , ...)				OOLUA_MEMBER_BODY_N(const ,func,return_value,func , __VA_ARGS__ )
#define OOLUA_MEM_FUNC_RENAME_N(name,return_value,func , ...)		OOLUA_MEMBER_BODY_N(      ,name,return_value,func , __VA_ARGS__ )
#define OOLUA_MEM_FUNC_CONST_RENAME_N(name,return_value,func , ...)	OOLUA_MEMBER_BODY_N(const ,name,return_value,func , __VA_ARGS__ )

#define OOLUA_MEM_FUNC_0(return_value,func)							OOLUA_MEMBER_BODY_0(      ,func,return_value,func)
#define OOLUA_MEM_FUNC_CONST_0(return_value,func)					OOLUA_MEMBER_BODY_0(const ,func,return_value,func)
#define OOLUA_MEM_FUNC_RENAME_0(name,return_value,func)				OOLUA_MEMBER_BODY_0(      ,name,return_value,func)
#define OOLUA_MEM_FUNC_CONST_RENAME_0(name,return_value,func)		OOLUA_MEMBER_BODY_0(const ,name,return_value,func)



#define OOLUA_MFUNC_GENERIC(mod,ProxyName,FunctionName) \
int ProxyName(lua_State* l) mod \
{ \
	return OOLUA::INTERNAL::mod##proxy_member_function_with_default_traits(l, m_this, &class_::FunctionName); \
}

/*
 OOLUA_MFUNC_INTERNAL(_CONST)_2 and OOLUA_MFUNC(_CONST)_INTERNAL_1
 A member function defined using OOLUA_MFUNC*  (*=  |_CONST )
 will translate into a corresponding macro below using and then to OOLUA_MFUNC_GENERIC
 */
#define OOLUA_MFUNC_INTERNAL_2(FunctionName,ProxyName)		OOLUA_MFUNC_GENERIC(	,ProxyName,FunctionName)
#define OOLUA_MFUNC_INTERNAL_1(FunctionName)				OOLUA_MFUNC_INTERNAL_2(FunctionName,FunctionName) 

#define OOLUA_MFUNC_CONST_INTERNAL_2(FunctionName,ProxyName)OOLUA_MFUNC_GENERIC(const,ProxyName,FunctionName)
#define OOLUA_MFUNC_CONST_INTERNAL_1(FunctionName)			OOLUA_MFUNC_CONST_INTERNAL_2(FunctionName,FunctionName) 


/** \endcond*/

#endif 

