///////////////////////////////////////////////////////////////////////////////
///  @file fwd_push_pull.h
///  Foward declarations of the push and pull methods to and from the script.
///  @author Liam Devine
///  \copyright
///  See licence.txt for more details. \n 

///////////////////////////////////////////////////////////////////////////////


#ifndef FORWARD_DECLARE_PUSH_PULL_H_
#	define FORWARD_DECLARE_PUSH_PULL_H_

#include <string>
#include "param_traits.h" //required for OOLUA::Owner

namespace OOLUA
{

    template<int ID>struct Lua_ref;
    class Table;

	template<typename T>struct lua_acquire_ptr;
	template<typename T>struct cpp_acquire_ptr;

	/**@{
		\brief Pushes an instance onto the top of the Lua stack
		\return 
		Depending on the \ref OOLuaConfig used the return value can 
		indicate success and failure.
		\see OOLuaErrorReporting
	*/
	bool push(lua_State* const s, bool const& value);
	bool push(lua_State* const s, std::string const& value);
	bool push(lua_State* const s, char const * const& value);
	bool push(lua_State* const s, double const& value);
	bool push(lua_State* const s, float const&  value);
	bool push(lua_State* const s, lua_CFunction const &  value);
	bool push(lua_State* const s, Table const &  value);
	bool push(lua_State* const s, Lua_ref<OOLUA::INTERNAL::FUNCTION> const &  value);
	template<typename T>
	bool push(lua_State* const s, T * const &  value);
	template<typename T>
	bool push(lua_State* const s, T * const &  value,OOLUA::Owner);
	template<typename T>
	bool push(lua_State* const s, lua_acquire_ptr<T>&  value);
	template<typename T>
	bool push(lua_State* const s, T const &  value);
	/**@}*/

	/**@{*
		\brief Pulls the top element off the stack popping it.
		\details In stack terms this is a top followed by pop.
		\return 
		Depending on the \ref OOLuaConfig used the return value can 
		indicate success and failure.
		\see OOLuaErrorReporting
	*/
	bool pull(lua_State* const s, bool& value);
	bool pull(lua_State* const s, std::string& value);
	bool pull(lua_State* const s, double& value);
	bool pull(lua_State* const s, float& value);
	bool pull(lua_State* const s, lua_CFunction& value);
	bool pull(lua_State* const s, Table&  value);
	bool pull(lua_State* const s, Lua_ref<1>&  value);
	template<typename T>
	bool pull(lua_State* const s, T *&  value);
	template<typename T>
	bool pull(lua_State* const s, T&  value);
	template<typename T>
	bool pull(lua_State* const s, cpp_acquire_ptr<T>&  value);	
	/**@}*/

	
	namespace INTERNAL
	{
		namespace LUA_CALLED
		{
			void pull2cpp(lua_State* const s, bool& value);
			void pull2cpp(lua_State* const s, std::string& value);
			void pull2cpp(lua_State* const s, double& value);
			void pull2cpp(lua_State* const s, float& value);
			void pull2cpp(lua_State* const s, lua_CFunction& value);
			void pull2cpp(lua_State* const s, Lua_ref<OOLUA::INTERNAL::FUNCTION>& value);
			void pull2cpp(lua_State* const s, Table&  value);
			void pull2cpp(lua_State* const s, Lua_ref<OOLUA::INTERNAL::TABLE>& value);
			
			template<typename T> 
			void pull2cpp(lua_State* const s, T& value);
			
			template<typename T>
			void pull2cpp(lua_State* const s, T *&  value);
			
			template<typename T>
			void pull2cpp(lua_State* const s, OOLUA::cpp_acquire_ptr<T>&  value);
			
			
			
			void get(lua_State* const s, int idx, bool& value);
			void get(lua_State* const s, int idx, std::string& value);
			void get(lua_State* const s, int idx, double& value);
			void get(lua_State* const s, int idx, float& value);
			void get(lua_State* const s, int idx, lua_CFunction& value);
			void get(lua_State* const s, int idx, Lua_ref<OOLUA::INTERNAL::FUNCTION>& value);
			void get(lua_State* const s, int idx, Table&  value);
			void get(lua_State* const s, int idx, Lua_ref<OOLUA::INTERNAL::TABLE>& value);
			
			template<typename T> 
			void get(lua_State* const s, int idx, T& value);
			
			template<typename T>
			void get(lua_State* const s, int idx, T *&  value);
			
			template<typename T>
			void get(lua_State* const s, int idx, OOLUA::cpp_acquire_ptr<T>&  value);
			
		}
	}
}



#endif

