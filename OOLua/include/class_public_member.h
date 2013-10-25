///////////////////////////////////////////////////////////////////////////////
///  @file class_public_member.h
///  @author Liam Devine
///  \copyright 
///  See licence.txt for more details. \n 
///////////////////////////////////////////////////////////////////////////////

#ifndef CLASS_PUBLIC_MEMBER_H_
#	define CLASS_PUBLIC_MEMBER_H_

#include "fwd_push_pull.h"
#include "param_traits.h"

namespace OOLUA 
{
	/** \cond INTERNAL */
	namespace INTERNAL
	{
	
		template<typename T,int shouldBeByRef>
		struct shouldPushValueByReference 
		{
			enum {value = 0};
			static void push(lua_State* l,T* input)
			{
				OOLUA::push(l,*input);		
			}
		};
	
		template<typename T>
		struct shouldPushValueByReference<T,1>
		{
			enum {value = 1};
			static void push(lua_State* l ,T* input)
			{
				OOLUA::push(l,input);
			}
		};
	
		struct PushPublicMember 
		{
			template<typename T>
			static void push(lua_State* l, T* input)
			{
				shouldPushValueByReference<T, 
					//can not have a pointer to reference.
					//T will either be a pointer to type
					//or type
					OOLUA::INTERNAL::is_param_by_value<T>::value 
					&& has_a_proxy_type<typename OOLUA::INTERNAL::Raw_type<T>::type >::value >
						::push(l,input);
			}
		
		};
		
	}
	
	/** \endcond */
}

/** \addtogroup OOLuaDSL
 @{*/

	/** \addtogroup OOLuaExpressive Expressive
	@{*/

	/** \def OOLUA_MEM_GETN
		\hideinitializer
		\brief Generates a getter for a public member
		\details OOLUA_MEM_GETN(get_name,id)	
		\param get_name	Name of the proxy function which is generated
		\param id Public member name
	*/
#	define OOLUA_MEM_GETN(get_name,id)\
	int get_name(lua_State* l) const\
	{\
		OOLUA::INTERNAL::PushPublicMember::push(l,&m_this->id);\
		return 1;\
	}

	/** \def OOLUA_MEM_SETN
		\hideinitializer
		\brief Generates a setter for a public member
		\details OOLUA_MEM_SETN(set_name,id)
		\param set_name	Name of the proxy function which is generated
		\param id Public member name
	 */
#	define OOLUA_MEM_SETN(set_name,id)\
	int set_name(lua_State* l) \
	{\
		OOLUA::INTERNAL::LUA_CALLED::pull2cpp(l,m_this->id);\
		return 0;\
	}


	/**	@}*/


	/** \addtogroup OOLuaMinimalist Minimalist
	@{*/
		/**@{*/
	/** \def OOLUA_MGET
		\hideinitializer
		\details OOLUA_MGET(id)
		\brief Generates a getter for a public member
		\param id Public member name
	*/
#	define OOLUA_MGET(id) OOLUA_MEM_GETN(get_##id,id)

	/** \def OOLUA_MSET
		\hideinitializer
		\details OOLUA_MGET(id)
		\brief Generates a setter for a public member
		\param id Public member name
	 */
#	define OOLUA_MSET(id) OOLUA_MEM_SETN(set_##id,id)

	/** \def OOLUA_MGET_MSET
		\hideinitializer
		\brief Generates both a getter and setter for a public member
		\details OOLUA_MGET_MSET(id)
		\param id Public member name
	 */
#	define OOLUA_MGET_MSET(id) \
		OOLUA_MGET(id) \
		OOLUA_MSET(id)

		/**@}*/

	/**	@}*/

/**	@}*/
#endif //CLASS_PUBLIC_MEMBER_H_
