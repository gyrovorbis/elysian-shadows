#ifndef DEFAULT_TRAIT_CALLER_H_
#	define DEFAULT_TRAIT_CALLER_H_

//* \cond INTERNAL*/

struct lua_State;

namespace OOLUA
{
	namespace INTERNAL
	{
		/*Two structures are used here instead of one and specialising so that compile 
		 errors are forced instead of a runtime error when an incorrect macro is used */
		template<typename func_type, typename type = func_type>
		struct default_traits_caller;
		template<typename func_type, typename type = func_type>
		struct default_traits_const_caller;
		
		template<typename this_type,typename func_type>
		int proxy_member_function_with_default_traits(lua_State* l,this_type* this_,func_type mfptr)
		{
			return default_traits_caller<func_type>::call(l,this_,mfptr);
		}
		template<typename this_type,typename func_type>
		int proxy_const_member_function_with_default_traits(lua_State* l,this_type* this_,func_type mfptr)
		{
			return default_traits_const_caller<func_type>::call(l,this_,mfptr);
		}
		
		template<typename this_type,typename func_type>
		int constproxy_member_function_with_default_traits(lua_State* l,this_type* this_,func_type mfptr)
		{
			return default_traits_const_caller<func_type>::call(l,this_,mfptr);
		}
		
		
		template<typename func_type,typename type = func_type>
		struct default_c_traits_caller;
		template<typename func_type>
		int proxy_c_function_with_default_traits(lua_State* l,func_type fptr)
		{
			return default_c_traits_caller<func_type>::call(l,fptr);
		}
	}
}


#define OOLUA_GENERATE_DEFAULT_TRAIT_CALLER(num) \
namespace OOLUA \
{ \
	namespace INTERNAL \
	{ \
		template <class class_type, typename return_type OOLUA_COMMA_PREFIXED_TYPENAMES_##num ,typename func_type> \
		struct default_traits_caller<return_type (class_type::*) (OOLUA_COMMA_SEPERATED_TYPES_##num),func_type> \
		{ \
			typedef OOLUA::INTERNAL::return_type_traits<return_type > R; \
			static int call (lua_State* l, class_type* this_, func_type mfptr) \
			{ \
				OOLUA_PARAMS_DEFAULT_INTERNAL_##num(1) \
				OOLUA::INTERNAL::Proxy_caller< R, class_type, LVD::is_void<return_type >::value >:: template call<func_type OOLUA_COMMA_PREFIXED_PARAM_TYPES_##num >(l,this_,mfptr OOLUA_CALL_PARAMS_##num); \
				return R::out; \
			} \
		}; \
		template <class class_type, typename return_type OOLUA_COMMA_PREFIXED_TYPENAMES_##num , typename func_type> \
		struct default_traits_const_caller<return_type (class_type::*) (OOLUA_COMMA_SEPERATED_TYPES_##num)const,func_type> \
		{ \
			typedef OOLUA::INTERNAL::return_type_traits<return_type > R; \
			static int call (lua_State* l, class_type* this_, func_type mfptr) \
			{ \
				OOLUA_PARAMS_DEFAULT_INTERNAL_##num(1) \
				OOLUA::INTERNAL::Proxy_caller< R, class_type, LVD::is_void<return_type >::value >:: template call<func_type OOLUA_COMMA_PREFIXED_PARAM_TYPES_##num >(l,this_,mfptr OOLUA_CALL_PARAMS_##num); \
				return R::out; \
			} \
		}; \
		template <typename return_type OOLUA_COMMA_PREFIXED_TYPENAMES_##num ,typename func_type> \
		struct default_c_traits_caller<return_type (*) (OOLUA_COMMA_SEPERATED_TYPES_##num),func_type> \
		{ \
			typedef OOLUA::INTERNAL::return_type_traits<return_type > R; \
			static int call (lua_State* l, func_type fptr) \
			{ \
				OOLUA_PARAMS_DEFAULT_INTERNAL_##num(0) \
				OOLUA::INTERNAL::Proxy_none_member_caller< R, LVD::is_void<return_type >::value >:: template call<func_type OOLUA_COMMA_PREFIXED_PARAM_TYPES_##num >(l,fptr OOLUA_CALL_PARAMS_##num); \
				return R::out; \
			} \
		};\
	} \
}


OOLUA_GENERATE_DEFAULT_TRAIT_CALLERS

/** \endcond*/

#endif
