///////////////////////////////////////////////////////////////////////////////
///  @file lua_ref.h
///  A wrapper for a lua registry reference in a struct 
///  so that the type is different to an int. Typedefs two types Lua_table_ref & \n
///  Lua_ref.
///  @author Liam Devine
///  \copyright
///  See licence.txt for more details. \n 
///////////////////////////////////////////////////////////////////////////////
#ifndef LUA_REF_H_
#	define LUA_REF_H_

#include "lua_includes.h"


#ifdef __GNUC__
#	define OOLUA_DEFAULT __attribute__ ((visibility("default")))
#else
#	define OOLUA_DEFAULT
#endif	

namespace OOLUA
{
	class Table;
	
	/** \cond INTERNAL*/
	namespace INTERNAL 
	{
		void pull_handle_invalid(lua_State* const lua,int id);
		bool push_reference_if_possible(int const ref, lua_State* const from,lua_State* const to);
	}
	/** \endcond */
	
	/**	\struct Lua_ref
		\brief A typed wrapper for a Lua value
		\details
		The Lua_ref templated class stores a reference using Lua's 
		reference system luaL_ref and luaL_unref. This class also stores the 
		lua_State for which the reference is valid for so that one from a 
		different Lua universe is difficult to be used incorrectly. A reference 
		from the same Lua universe yet a different lua_State is \ref valid
		to be used in the universe.
		<p>
		The class takes ownership of any reference passed either to the
		\ref Lua_ref(lua_State* const,int const&) "two argument constructor"
		or the \ref set_ref function. On going out of scope a \ref valid
		reference is guaranteed to be released, you can also force a release
		by passing an instance to \ref swap on which \ref valid returns false.
		<p>
		There are two special values for the reference which Lua provides,
		both of which OOLua will treat as an invalid reference:
			\li LUA_REFNIL luaL_ref return value to indicate it encountered a
				nil object at the location the ref was asked for
			\li LUA_NOREF guaranteed to be different from any reference return 
				by luaL_ref
		\tparam ID Lua type as returned by lua_type
		\note
			\li Universe A call to luaL_newstate or lua_newstate creates a
		Lua universe and a universe is completely independant of any other
		universe. lua_newthread and coroutine.create create a lua_State
		in an already existing universe.\n
		Term first heard in a mailing list post by Mark Hamburg
	*/
	template<int ID>
	struct Lua_ref
	{
		/**
			\brief
			Sets the lua_State and reference for the instance
			\note this does not preform any validation on the parameters and it
			is perfectly acceptable to pass parameters such that a call to
			valid will return false.
		*/
		Lua_ref(lua_State* const lua,int const& ref);
		
		/**
			\brief
			Sets the lua_State for the instance and initialises the instance 
			so that a call to valid will return false.
		*/
		explicit Lua_ref(lua_State* const lua);
		
		/** 
			\brief
			Initialises the instance so that a call to valid will return false.
		 */
		Lua_ref();
		
		//unimplemented
		Lua_ref& operator =(Lua_ref const& /*rhs*/);
		
		/** 
			\brief
			Creates a copy of rhs
			\details
			If rhs is valid then creates a new Lua reference to the value which rhs
			refers to, else intialises this instead so that a \ref Lua_ref::valid 
			call returns false.
		 */
		Lua_ref(Lua_ref const& rhs) OOLUA_DEFAULT;
		
		~Lua_ref()OOLUA_DEFAULT;
		
		/**
			\brief
			Returns true if both the Lua instance is not NULL and the registry reference
			is not invalid.
		*/
		bool valid()const;
		/**
			\brief
			Sets the stored reference and state.
			\details
			Releases any currently stored reference and takes ownership of the passed
			reference.
		*/
		void set_ref(lua_State* const lua,int const& ref)OOLUA_DEFAULT;
		
		/**
			\brief
			Swaps the Lua instance and the registry reference with rhs.
			\details
			Swaps the lua_State and reference with rhs, this is a
			simple swap and does not call luaL_ref therefore it will not
			create any new references.
		*/
		void swap(Lua_ref & rhs);
		
		/** \cond INTERNAL*/
		
		bool push(lua_State* const lua)const;
		bool pull(lua_State* const lua) OOLUA_DEFAULT ;
		bool lua_push(lua_State* const lua)const;
		bool lua_pull(lua_State* const lua);
		bool lua_get(lua_State* const lua, int idx);
		
		/**
			\brief
			Yes I know this is bad but it makes other things work nicely.
			Can you just pretend you have never seen it ?
		*/
		lua_State* state() const { return m_lua; }
		/** \endcond*/
		
		int const& ref()const;
	private:
		/** \cond INTERNAL Yes I know this is bad \endcond*/
		friend class  Table;
		bool pull_if_valid(lua_State* l);
		void release();
		lua_State* m_lua;
		int m_ref;
	};
	
	/** \cond INTERNAL*/
	template<int ID>
	Lua_ref<ID>::Lua_ref(lua_State* const lua,int const& ref)
		:m_lua(lua),m_ref(ref)
	{}
	template<int ID>
	Lua_ref<ID>::Lua_ref(lua_State* const lua)
		:m_lua(lua),m_ref(LUA_NOREF)
	{}
	template<int ID>
	Lua_ref<ID>::Lua_ref()
		:m_lua(0),m_ref(LUA_NOREF)
	{}
	
	template<int ID>
	Lua_ref<ID>::Lua_ref(Lua_ref<ID> const& rhs)
		:m_lua(0),m_ref(LUA_NOREF)
	{
		if (rhs.valid()) 
		{
			m_lua = rhs.m_lua;
			lua_rawgeti(m_lua, LUA_REGISTRYINDEX, rhs.m_ref );
			m_ref = luaL_ref(m_lua, LUA_REGISTRYINDEX);
		}
	}
	
	template<int ID>
	Lua_ref<ID>::~Lua_ref()
	{
		release();
	}
	template<int ID>
	bool Lua_ref<ID>::valid()const
	{
		return m_lua && m_ref != LUA_REFNIL && m_ref != LUA_NOREF;
	}
	template<int ID>
	int const& Lua_ref<ID>::ref()const
	{
		return m_ref;
	}
	template<int ID>
	inline void Lua_ref<ID>::set_ref(lua_State* const lua,int const& ref)
	{
		release();
		m_ref = ref;
		m_lua = lua;
	}
	template<int ID>
	void Lua_ref<ID>::release()
	{
		if( valid() )
		{
			luaL_unref(m_lua,LUA_REGISTRYINDEX,m_ref);
		}
		m_ref = LUA_NOREF;
	}
	template<int ID>
	void Lua_ref<ID>::swap(Lua_ref & rhs)
	{
		lua_State* tl (rhs.m_lua);
		int tr (rhs.m_ref);
		rhs.m_lua = m_lua;
		rhs.m_ref = m_ref;
		m_lua = tl;
		m_ref = tr;
	}

	template<int ID>
	bool Lua_ref<ID>::push(lua_State* const lua)const
	{
		if (!valid() ) {
			lua_pushnil(lua);
			return true;
		}
		return  INTERNAL::push_reference_if_possible(m_ref,m_lua,lua) 
					&& lua_type(lua, -1) == ID;
	}
	
	template<int ID>
	bool Lua_ref<ID>::lua_push(lua_State* const lua)const
	{
		if (!valid() ) {
			lua_pushnil(lua);
			return true;
		}
		else if( lua != m_lua )
		{
			luaL_error(lua,"The reference is not valid for this Lua State");
			return false;
		}
		lua_rawgeti(m_lua, LUA_REGISTRYINDEX, m_ref );
		return  lua_type(m_lua, -1) == ID;
	}
	
	
	template<int ID>
	bool Lua_ref<ID>::pull_if_valid(lua_State* const l)
	{
		if (lua_gettop(l) == 0)  return false;
		const int type = lua_type(l,-1);
		if( type == ID )
		{
			set_ref( l, luaL_ref(l, LUA_REGISTRYINDEX) );
			return true;
		}
		else if( type == LUA_TNIL )
		{
			release();
			return true;
		}
		return false;
	}
	template<int ID>
	bool Lua_ref<ID>::pull(lua_State* const lua) 
	{
		if( !pull_if_valid(lua) )
		{
			INTERNAL::pull_handle_invalid(lua,ID);
			return false;
		}
		return true;
	}
	
	
	template<int ID>
	bool Lua_ref<ID>::lua_pull(lua_State* const lua) 
	{
		if( !pull_if_valid(lua) )
		{
			luaL_error(lua,
					   "pulling incorrect type from stack. This is a ref to id %d, stack contains %s"
					   ,ID
					   ,lua_typename(lua,lua_type(lua, -1))
						);
			return false;
		}
		return true;
	}
	template<int ID>
	bool Lua_ref<ID>::lua_get(lua_State* const vm, int idx) 
	{
#define oolua_err_get() \
luaL_error(vm, \
"pulling incorrect type from stack. This is a ref to id %d, stack contains %s" \
,ID \
,lua_typename(vm,lua_type(vm, idx)) \
); \
return false/*never gets here*/ 
		
		
		if (lua_gettop(vm) < idx) { oolua_err_get(); }
		const int type = lua_type(vm,idx);
		if( type == ID )
		{
			lua_pushvalue(vm, idx);
			set_ref( vm, luaL_ref(vm, LUA_REGISTRYINDEX) );
			return true;
		}
		else if( type == LUA_TNIL )
		{
			release();
			return true;
		}
		oolua_err_get();
#undef oolua_err_get
	}
	/** \endcond*/
	
	/** \typedef Lua_table_ref
		\brief Typedef helper for a LUA_TTABLE registry reference
	 */
	typedef Lua_ref<LUA_TTABLE> Lua_table_ref;
	/** \typedef Lua_func_ref
		\brief Typedef helper for a LUA_TFUNCTION registry reference
	*/
	typedef Lua_ref<LUA_TFUNCTION> Lua_func_ref;
	
	
}
#endif //LUA_REF_H_
