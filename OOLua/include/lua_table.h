///////////////////////////////////////////////////////////////////////////////
///  @file lua_table.h
///  Wrapper around a table in Lua which allows quick and easy access.
///  @author Liam Devine
///  \copyright
///  See licence.txt for more details. \n 
///////////////////////////////////////////////////////////////////////////////
#ifndef LUA_TABLE_H_
#	define LUA_TABLE_H_

#	include "lua_includes.h"
#	include <string>
#	include "fwd_push_pull.h"
#	include "lua_ref.h"
#	include "oolua_config.h"
#	include "oolua_error.h"


namespace OOLUA
{
	/**	\class Table
		\brief Wrapper around a table in Lua which allows quick and easy access.
		\details
		Any value can be retrieved or set from the table via the use of the template
		member function. If the value asked for is not the correct type located
		in the position an error can be reported, the type of which depends on \ref OOLuaErrorReporting \n
		at: 
		 runs no checks on the table. Undefined if Lua instance is null or 
		(table or key is invalid) or value is not correct type.
		 safe_at:
		returns a bool to indicate for success.
		try_at:
		Function is only defined when exceptions are enabled.
		throws Runtime_error when the key is not in the table
   
	  The functions guarantee that the Lua stack after operations is restored
	  to the state when entered.
    
		See Unit Tests or CheatSheet for usage of constructor.
	*/
	class Table
	{
	public:
		/**@{**/
		
		/** \brief Default creates an object on which a call to valid returns false*/
		Table();
		
		/** \brief
			Initialises the reference to be an instance of the same registry 
			reference or an invalid table if ref.valid() == false
		*/
		explicit Table(Lua_table_ref const& ref);

		/** \brief Sets the lua_State and calls Lua_table::set_table */
		Table(lua_State*  const lua,std::string const& name);
		Table(Table const& rhs);
		
		/** \brief unimplemented*/
		Table& operator =(Table const& /*rhs*/);

		/**@}*/
		
		~Table()OOLUA_DEFAULT;
		
		/**@{*/
		void bind_script(lua_State*  const lua);
		
		/** \brief
			Order of trying to initilaise :
			\li name.empty() == true: Creates am invalid object.
			\li name found as a table in Lua global: Swaps the internal Lua_func_ref 
				with an instance initialised to an id obtained from the Lua registry.
			\li name found as a table in Lua registry: Swaps the internal Lua_func_ref 
				with an instance initialised to an id obtained from the Lua registry.
			\li else Swaps the internal Lua_func_ref with an uninitialised instance.
		*/
		void set_table(std::string const& name);
		
		/** \brief Initailises the internal Lua_func_ref to the id ref*/
		void set_ref(lua_State* const lua,int const& ref);
		
		/** \brief Swaps the internal Lua_func_ref and rhs.m_table_ref*/
		void swap(Table & rhs);
		
		/**@}*/

		/**@{*/
#if OOLUA_USE_EXCEPTIONS ==1 
		template<typename T,typename T1>void try_at(T const& key,T1& value);
#endif
		template<typename T,typename T1>bool safe_at(T const& key,T1& value);

		/**
			\brief
			\tparam T key type
			\tparam T1 value type
		 
			\note
			No error checking
			It is undefined to call this function when:
			\li table or the key are invalid
			\li table does not contain the key
			\li value is not the correct type
		 
			\see Lua_table::safe_at \see Lua_table::try_at
		 */
		template<typename T,typename T1>T1& at(T const& key,T1& value);
		/**@}*/

		/**@{*/
		/** \brief 
			Inserts the key value pair into the table if key is not present 
			else it updates the table's key entry
		*/
		template<typename T,typename T1>void set_value(T const& key,T1 const& value);
		/** \brief
			Removes the key from the table by setting it's value to nil
		*/
		template<typename T>void remove_value(T const& key);
		/**@}*/
		
		/** \brief 
			Returns a boolean which is the result of checking the state of the internal Lua_func_ref. */
		bool valid()const;
		
		
		typedef void(*traverse_do_function)(lua_State*);
		/** \deprecated*/
		void traverse(traverse_do_function do_);
		
		/** \cond INTERNAL*/
		bool push_on_stack(lua_State* l)const;
		bool pull_from_stack(lua_State* l);
		void lua_pull_from_stack(lua_State* l);
		void lua_get(lua_State* l,int idx);
		lua_State* state() const { return m_table_ref.m_lua; }
		/** \endcond*/
		


		
	private:
		bool get_table()const;
		void restore_stack(int const & init_stack_size)const;
		int initial_stack_size()const;
		Lua_table_ref m_table_ref;
	};

	inline Table::~Table(){}
	
	
	//no error checking
	//undefined if lua is null or (table or key is invalid) or value is not correct type
	template<typename T,typename T1>
	inline T1& Table::at(T const& key,T1& value)
	{
		get_table();//table
		push(m_table_ref.m_lua,key);//table key
		lua_gettable(m_table_ref.m_lua, -2);//table value
		pull(m_table_ref.m_lua, value);//table
		lua_pop(m_table_ref.m_lua,1);
		return value;
	}
	
	template<typename T,typename T1>
	inline void Table::set_value(T const& key,T1 const& value)
	{
		//record the stack size as we want to put the stack into the 
		//same state that it was before entering here
		//int init_stack_size = lua_gettop(m_lua);
		int const init_stack_size = initial_stack_size();
		if(!get_table())return;
		push(m_table_ref.m_lua,key);
		//table is now at -2 (key is at -1). 
		//push the new value onto the stack
		push(m_table_ref.m_lua,value);
		//table is not at -3 set the table
		lua_settable(m_table_ref.m_lua,-3);
		
		restore_stack(init_stack_size);
	}
	
	template<typename T>
	inline void Table::remove_value(T const& key)
	{
		//record the stack size as we want to put the stack into the 
		//same state that it was before entering here
		//int init_stack_size = lua_gettop(m_lua);
		int const init_stack_size = initial_stack_size();
		if(!get_table())return;
		push(m_table_ref.m_lua,key);
		//table is now at -2 (key is at -1). 
		//push the new value onto the stack
		lua_pushnil(m_table_ref.m_lua);
		//table is not at -3 set the table
		lua_settable(m_table_ref.m_lua,-3);
		
		restore_stack(init_stack_size);
	}
	
	
#if OOLUA_USE_EXCEPTIONS ==1 
	template<typename T,typename T1>
	inline void Table::try_at(T const& key,T1& value)
	{
		int const init_stack_size = initial_stack_size();
		try 
		{
			if(!get_table())throw OOLUA::Runtime_error("Table is invalid");
			push(m_table_ref.m_lua,key);
			lua_gettable(m_table_ref.m_lua, -2);
			if(lua_type(m_table_ref.m_lua,-1) == LUA_TNIL )
			{
				throw OOLUA::Runtime_error("key is not present in table");
			}
			pull(m_table_ref.m_lua, value);
			restore_stack(init_stack_size);
		}
		
		catch (...) 
		{
			restore_stack(init_stack_size);
			throw;
		}
		
		
	}
	template<typename T,typename T1>
	inline bool Table::safe_at(T const& key,T1& value)
	{
		try
		{
			try_at(key, value);
		}
		catch (...)
		{
			return false;
		}
		return true;
	}
#else
	
	template<typename T,typename T1>
	inline bool Table::safe_at(T const& key,T1& value)
	{
		//record the stack size as we want to put the stack into the 
		//same state that it was before entering here
		//int init_stack_size = lua_gettop(m_lua);
		int const init_stack_size = initial_stack_size();
		if(!get_table())return false;
		if(! push(m_table_ref.m_lua,key) )
		{
			restore_stack(init_stack_size);
			return false;
		}
		//table is now at -2 (key is at -1). lua_gettable now pops the key off
		//the stack and then puts the data found at the key location on the stack
		lua_gettable(m_table_ref.m_lua, -2);
		if(lua_type(m_table_ref.m_lua,-1) == LUA_TNIL )
		{
			restore_stack(init_stack_size);
			return false;
		}
		pull(m_table_ref.m_lua, value);
		restore_stack(init_stack_size);
		
		return true;
	}
#endif
	
	
	/**	\brief
		The table is at table_index which can be either absolute or pseudo in the stack
		table is left at the index.
	*/
	template<typename T,typename T1>
	inline void table_set_value(lua_State* lua,int table_index,T const& key,T1 const& value)
	{
		push(lua,key);
		push(lua,value);
		lua_settable(lua,table_index < 0 ? table_index-2 : table_index);
	}
	
	/**	\brief
	 The table is at table_index which can be either absolute or pseudo in the stack
	 table is left at the index.
	*/
    template<typename T,typename T1>
	inline bool table_at(lua_State* lua, int const table_index, T const& key,T1& value)
	{
		push(lua,key);//table key
		lua_gettable(lua, table_index < 0 ? table_index-1 : table_index);//table value
		return pull(lua, value);//table
	}
    
	/** 
		\brief Creates a new valid \ref OOLUA::Table
		\param [in] l
		\param [inout] t
		\post stack is the same on exit as entry
	 
	*/
	void new_table(lua_State* l,OOLUA::Table& t);

	/** 
		\brief Creates a new valid \ref OOLUA::Table "Table"
		\post stack is the same on exit as entry
	*/	
	OOLUA::Table new_table(lua_State* l);
	
	
	/**	\def oolua_ipairs (table)
		Helper for iterating over the array part of a table declares: 
		\li \arg _i_index_			: current index into the array
		\li \arg _oolua_array_index_: stack index at which table is located
		\li \arg lvm				: the table's lua_State
		
		\param table 
	 
		\note
		Returning from inside of the loop will not leave the stack clean
		unless you reset it.
		usage:
		\code{.cpp}
		oolua_ipairs(table)
		{
			if(_i_index_ == 99) 
			{
				lua_settop(lvm,_oolua_array_index-1);
				return "red balloons";
			}
		}
		oolua_ipairs_end()
		return "Not enough ballons to go bang."
		\endcode
	 */
#	define oolua_ipairs(table) \
	if( table.valid() ) \
	{ \
		lua_State* lvm = table.state(); \
		lua_checkstack(lvm, 2);\
		OOLUA::push(lvm,table); \
		int const _oolua_array_index_ = lua_gettop(lvm); \
		int _i_index_ = 1; \
		lua_rawgeti(lvm, _oolua_array_index_,_i_index_); \
		while (lua_type(lvm, -1)  != LUA_TNIL) \
		{ \

	/** \def oolua_ipairs_end()
		\see oolua_ipairs
	*/
#	define oolua_ipairs_end()\
			lua_settop(lvm, _oolua_array_index_); \
			lua_rawgeti(lvm, _oolua_array_index_,++_i_index_); \
		} \
		lua_settop(lvm,_oolua_array_index_-1); \
	}

	
	
	/** \def oolua_pairs(table)
		Helper for iterating over a table.\n
		Declares:
		\li _oolua_table_index_	: stack index at which table is located
		\li lvm					: the table's lua_State
	 
	 usage:
	 \code{.cpp}
	 oolua_pairs(table)
	 {
		\\do what ever
		lua_pop(value);\\leaving key at the top of stack
	 }
	 oolua_pairs_end()
	 \endcode
	*/
	
#	define oolua_pairs(table) \
	if( table.valid() ) \
	{ \
		lua_State* lvm = table.state(); \
		OOLUA::push(lvm,table); \
		int	const _oolua_table_index_ = lua_gettop(lvm); \
		lua_pushnil(lvm); \
		while (lua_next(lvm, _oolua_table_index_) != 0) 
	
	/** \def oolua_pairs_end()
		\see oolua_pairs
	*/
#	define oolua_pairs_end() \
		lua_pop(lvm, 1); \
	}
	
	
	/* \brief
		You must remove the value from the stack and leave the key
		do not call anything which may call tostring on the actual key
		duplicate it instead with lua_pushvalue then call the operation on the copy
	*/
	template<typename ClassType>
	inline void for_each_key_value(OOLUA::Table& table, ClassType* instance, void(ClassType::*func)(lua_State*) )
	{
		oolua_pairs(table)
		{
			(instance->*(func))(lvm);
		}
		oolua_pairs_end()
	}
}

#endif


