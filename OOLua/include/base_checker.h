#ifndef OOLUA_BASE_CHECKER_H_
#	define OOLUA_BASE_CHECKER_H_

#include "type_list.h"
#include "lua_includes.h"
#include "oolua_userdata.h"
#include "proxy_class.h"

namespace OOLUA
{

	/** \cond INTERNAL*/
	template<typename T>
	void register_class(lua_State* l);
	
	namespace INTERNAL
	{

		template<typename T>
		void stack_top_type_is_base(lua_State* const l
								   ,Lua_ud* requested_ud
								   ,int const& userdata_index);
		
		template<typename T>
		bool ud_is_type(Lua_ud const* ud);
		
		template<typename ProxyStackType,typename BaseType,int DoWork>
		struct CastToRequestedProxyType;

        template<typename ProxyStackType,typename Bases, int BaseIndex,typename BaseType>
		struct Is_a_base;


		//cast the class pointer to the correct type and put it onto
		//of the stack
		template<typename ProxyStackType,typename BaseType,int DoWork = 1>
		struct CastToRequestedProxyType
		{
			static void* cast(lua_State* const l,int const& userdata_index)
			{
				//get the userdata
				//cast the class void ptr from the stack to the stacktype
				//then to base type to get correct offset
				return static_cast<BaseType*>(
							static_cast<typename ProxyStackType::class_* > ( 
										( static_cast<Lua_ud*>( lua_touserdata(l, userdata_index) ) )->void_class_ptr
																			)
											  );
			}
		};
		template<typename ProxyStackType,typename BaseType>
		struct CastToRequestedProxyType<ProxyStackType,BaseType,0>
		{
			static void* cast(lua_State* const /*l*/,int const& /*userdata_index*/)
			{return (void*)0;}
		};

		template<typename ProxyStackType,typename Bases, int BaseIndex,typename BaseType>
		struct Is_a_base
		{
			void operator()(lua_State * const l,int const& userdata_index,Lua_ud* requested_ud)
			{
				//is this a base
				if( ud_is_type<BaseType>(requested_ud) )
				{
					requested_ud->void_class_ptr = CastToRequestedProxyType<ProxyStackType,BaseType,1>::cast(l,userdata_index);
					return;
				}
				//check the next in the type list
				Is_a_base<
					ProxyStackType
					,Bases
					,BaseIndex + 1
					,typename TYPELIST::At_default< Bases, BaseIndex + 1, TYPE::Null_type >::Result
				> nextIsBase;
				nextIsBase(l,userdata_index,requested_ud);
			}
		};
		template<typename ProxyStackType,typename Bases, int BaseIndex>
		struct Is_a_base<ProxyStackType,Bases,BaseIndex,TYPE::Null_type>
		{
			void operator()(lua_State * const /*l*/
							,int const& /*userdata_index*/
							,INTERNAL::Lua_ud* /*requested_ud*/)
			{}//noop
		};


		template<typename T>
		inline void stack_top_type_is_base(lua_State* const l
										  ,Lua_ud* requested_ud
										  ,int const& userdata_index)
		{
			//ud... 
			INTERNAL::Is_a_base<OOLUA::Proxy_class<T>
					,typename OOLUA::Proxy_class<T>::AllBases
					,0
					,typename TYPELIST::At_default< typename OOLUA::Proxy_class<T>::AllBases,0,TYPE::Null_type >::Result
				> checkBases;
			checkBases(l,userdata_index,requested_ud);
		}
		
		template<typename T>
		inline bool ud_is_type(Lua_ud const* ud)
		{
			//some compilers need a hand it also enforces that the typedef in oolua_userdata is correct
			return ud->type_check ==  static_cast<oolua_type_check_function>(&OOLUA::register_class<T>);
		}
	}
	
	/** \endcond */
}
#endif
