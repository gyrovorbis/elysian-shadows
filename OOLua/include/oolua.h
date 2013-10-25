/** 
	\file oolua.h
	Header file for Object Oriented Lua.
*/



/**
	\mainpage
 
	\section OOLuaIntroduction Introduction
	\subsection OOLuaHipster Hipster
	OOLua is cross platform, test driven, dependancy free Open Source library which uses C++03 
	template metaprogramming and pre-processor magic to generate non intrusive proxies that 
	provide a fast bridge for the interaction of C++ classes with Lua; in addition it also 
	provides a thin abstraction layer for interfacing with the Lua stack. It supports muliple
	inheritance C++ classes without using C++ RTTI and does not use execeptions by default
	although they are easliy enabled.

	\subsection OOLuaNormal Normal
	OOLua is a library which makes it easy to use C++ classes in Lua and also operating
	on the stack using a typed interface for common operations.

	This is not a fully original work, instead it builds on ideas from binding 
	classes using <a href="http://lua-users.org/wiki/CppBindingWithLunar" target="_blank">Lunar</a> 
	and <a href="http://www.lua.org/notes/ltn005.html" target="_blank">Lua Technical Note 5</a>.
 
	\section OOLuaCompatibility Lua compatibility
	This version of the library is compatible with the following Lua implementations
		\li Rio Lua 5.1 and 5.2 http://www.lua.org
		\li LuaJIT 1.1.8 and 2.0 http://www.luajit.org/
 
	\section OOLuaLinks Links
		\li Project Home  http://code.google.com/p/oolua
		\li Issue tracker http://code.google.com/p/oolua/issues/list
		\li Mailing list http://groups.google.com/group/oolua-user
		\li Twitter http://twitter.com/OOLua
		\li <a href="http://oolua.googlecode.com/svn/docs/trunk/html/index.html">Latest trunk documentation</a>
		\li <a href="http://oolua.googlecode.com/svn/docs/branches/dsl/html/index.html">This documentation</a>
 
	\section OOLuaLicence Licence
	OOLua:
	\copyright
	The MIT License\n
	Copyright (c) 2009 - 2013 Liam Devine\n
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:\n
 	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.\n\n
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.\n
 
	Loki Type lists:
	\copydetails type_list.h
 
	Luna :
	\copydetails oolua_registration.h
*/


#ifndef OOLUA_H_
#	define OOLUA_H_


#	include <string>

#	include "lua_includes.h"
#	include "oolua_push_pull.h"
#	include "param_traits.h"
#	include "export_func_to_lua.h"
#	include "cpp_member_func.h"
#	include "oolua_c_func.h"
#	include "lua_operator.h"
#	include "proxy_class.h"
#	include "class_public_member.h"
#	include "lua_function.h"
#	include "lua_table.h"
#	include "lua_ref.h"
#	include "lua_stack_dump.h"
#	include "oolua_tags.h"
#	include "member_func_helper.h"
#   include "oolua_registration.h"
#   include "oolua_error.h"
#	include "oolua_version.h"
#	include "cpp_constructor.h"
#	include "oolua_va_args.h"


/** \namespace OOLUA
	\brief This is the root namespace of the library
	\details There are sub namespaces in OOLUA yet these are not intended for general
	usage, instead this namespace contains all such functions, structures ...
*/
namespace OOLUA
{

	/** \class Script
		\brief OOLua helper class 
		\details
		OOLua is purposely designed not to be dependent on the OOLUA::Script class
		and therefore passes around it's dependency of a lua_State instance.
		This class is only a helper and anything you can do with it 
		can be accomplished either via using a Lua_function struct, calling OOLUA 
		namespaced functions or using the Lua C API.
		
		Script provides the following :
		\li Scopes a lua_State pointer
		\li Provides access to the lua_State pointer via a cast operator and function
		\li Provides methods to register types
		\li Binds a Lua_function instance to call functions via Script::call
		\li Has member functions for a little state management
		\li Sets up the state to work with OOLua

		\note
		This class is not copy constructible or assignable.
		To accomplish this a counted reference to the lua_State would need to be
		maintained.
		\note If you do not want to or can not use this class please see 
		\ref OOLUA::setup_user_lua_state "setup_user_lua_state"
	*/
	class Script
	{
	public:
		/** Function object instance which can be used to call Lua functions*/
		Lua_function call;
		
		/**@{*/
		Script();
		~Script();
		/**@}*/

		/**@{*/	
		/** \brief Returns the stack count from the lua_State */
		int stack_count(){ return lua_gettop(m_lua); }
		/** \brief Convertion operator so that a \ref OOLUA::Script "Script" 
			instance can be passed in place of a lua_State pointer*/
		operator lua_State * () const {return m_lua;}
		/** \brief Sometimes you may want to be explicit \see Script::operator()  */
		lua_State * const & get_ptr() const {return m_lua;}
		/** \brief Performs a garbage collection on the state*/
		void gc();
		/**@}*/
		
		/**@{*/
		/** \brief Helper function \see OOLUA::register_class*/
		template<typename T>
		void register_class();
		/** \brief Helper function \see OOLUA::register_class*/
		template<typename T>
		void register_class(T* );
		/** \brief Helper function \see OOLUA::register_class_static */
		template<typename T,typename K,typename V>
		void register_class_static(K const& k, V const& v);
		/**@}*/
		
		/**@{*/
		/** \brief Helper function \see OOLUA::run_file */
		bool run_file(std::string const & filename);
		/** \brief Helper function \see OOLUA::load_file */
		bool load_file(std::string const & filename);
		/** \brief Helper function \see OOLUA::load_chunk */
		bool load_chunk(std::string const& chunk);
		/** \brief Helper function \see OOLUA::run_chunk */
		bool run_chunk(std::string const& chunk);
		/**@}*/

	protected:
		void close_down();
		Script(Script const& );
		Script& operator = (Script const& );
		lua_State* m_lua;

	};


	template<typename T>
	inline void Script::register_class()
	{
		OOLUA::register_class<T>(m_lua);
	}
	template<typename T>
	inline void Script::register_class(T* /*dummy*/)
	{
		register_class<T>();
	}

	template<typename T,typename K,typename V>
	inline void Script::register_class_static(K const& k, V const& v)
	{
			OOLUA::register_class_static<T>(m_lua,k,v);
	}
	
	
	/** 
		\brief If you want to use OOLua with a lua_State you already have active
		or supplied by some third party then calling this function
		adds the necessary tables and globals for it to work with OOLua.
		\param[in] l lua_State to be initialise by OOLua 
	*/
	void setup_user_lua_state(lua_State* l);
	
	/** 
		\brief Helper function to set a Lua global variable.
		\tparam T Type for instance
		\param[in] l lua_State
		\param[in] name Global name to set 
		\param[in] instance Any variable which is valid to push to the stack
		\return Boolean indicating if the operation was successful \see OOLuaErrorReporting
	*/
	template<typename T>
	bool set_global(lua_State* l, char const* name, T& instance)
	{
		bool result = OOLUA::push(l, instance);
		if (!result) return false;
		lua_setglobal(l, name);
		return true;
	}

	inline bool set_global(lua_State* l, char const* name, lua_CFunction instance)
	{
		lua_pushcclosure(l,instance,0);
		lua_setglobal(l, name);
		return true;
	}
	
	/** 
		\brief Helper function to set a Lua global variable to nil.
		\param[in] l lua_State
		\param[in] name Global name to set 
	*/
	void set_global_to_nil(lua_State*l, char const * name);
	
	/** 
		\brief Helper function to set a Lua global variable.
		\tparam T Type for instance
		\param[in] l lua_State
		\param[in] name Global name to query 
		\param[out] instance Any variable which is valid to pull from the stack
		\return Boolean indicating if the operation was successful \see OOLuaErrorReporting
	*/	
	template<typename T>
	bool get_global(lua_State* l, char const* name, T& instance)
	{
		lua_getglobal(l, name);
		return OOLUA::pull(l, instance);
	}
	
	/** \brief Uses the Lua C API to check if it is valid to move data between the states
		\param[in] vm0
		\param[in] vm1
		\returns true is vm0 and vm1 are different yet related states, else false
	*/
	bool can_xmove(lua_State* vm0,lua_State* vm1);
	/** \brief Loads a chunk leaving the resulting function on the stack
		\param lua [in] \copydoc lua_State
		\param chunk [in] 
	*/
	bool load_chunk(lua_State* lua, std::string const& chunk);
	/** \brief Loads and runs a chunk of code
		\param lua [in] \copydoc lua_State
		\param chunk [in] 
	*/
	bool run_chunk(lua_State* lua, std::string const& chunk);
	/** \brief Loads a file leaving the resulting function on the stack
		\param lua [in] \copydoc lua_State
		\param filename [in] 
	 */
	bool load_file(lua_State* lua, std::string const & filename);
	/** \brief Loads and runs the file
		\param lua [in] \copydoc lua_State
		\param filename [in] 
	*/
	bool run_file(lua_State* lua, std::string const & filename);
	
}//endof namepsace OOLUA




#endif //OOLUA_H_


