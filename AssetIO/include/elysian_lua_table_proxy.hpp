#ifndef ELYSIAN_LUA_TABLE_PROXY_H
#define ELYSIAN_LUA_TABLE_PROXY_H

#include "luatables.h"
#include "elysian_lua_manager.hpp"

//DSL for autogenerating proxy fields
#define ELYSIAN_LUA_PROXY_FIELDS_BEGIN(n) \
	static constexpr unsigned			_proxyFieldCount = n; \
	static constexpr LuaTableFieldProxy _proxyFields[n] = {

#define ELYSIAN_LUA_PROXY_FIELDS_END() };

#define ELYSIAN_LUA_PROXY_FIELD_READ_WRITE(f, g, s) { #f, #g, #s },
#define ELYSIAN_LUA_PROXY_FIELD_READ_ONLY(f, g) { #f, #g, nullptr },

#define ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define ELYSIAN_LUA_PROXY_FIELD(...) ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(__VA_ARGS__, ELYSIAN_LUA_PROXY_FIELD_READ_WRITE, ELYSIAN_LUA_PROXY_FIELD_READ_ONLY)(__VA_ARGS__)


#define ELYSIAN_LUA_PROXY_FIELDS_EXPORT(c) \
	const unsigned				c::_proxyFieldCount; \
    const LuaTableFieldProxy	c::_proxyFields[];

namespace elysian {

    struct LuaTableFieldProxy {
		const char *const key;
		const char *const get;
		const char *const set;
	};

	/* P - Proxy class
	 * T - Class being proxied
	 */
	template <typename P, typename T=P>
	class LuaTableProxy: public LuaManager {
	protected:

		static LuaTable _propertyProxyIndexTable;
		static LuaTable _propertyProxyNewIndexTable;
		static LuaTable _ooluaIndexTable;               //OOLUA's default index metamethod table
		static LuaTable _ooluaNewIndexTable;            //OOLUA's default new index metamethod table
		static LuaTable _staticMetaTable;
		LuaTable        _protoTable;

		static inline void _registerPropertyMetaMethodTables(lua_State* l) {
			lua_getglobal(l, OOLUA::Proxy_class<T>::class_name);

			//Register property pairs for index table <K=field, V=getFunc>
			lua_newtable(l);
			for(unsigned i = 0; i < P::_proxyFieldCount; ++i) {
				lua_getfield(l, -2, P::_proxyFields[i].get);
				lua_setfield(l, -2, P::_proxyFields[i].key);
			}
			_propertyProxyIndexTable.pull(l);

			//Register property pairs for newIndex table <K=field, V=setFunc>
			lua_newtable(l);
			for(unsigned i = 0; i < P::_proxyFieldCount; ++i) {
                if(P::_proxyFields[i].set == nullptr) continue; //read-only attribute!
				lua_getfield(l, -2, P::_proxyFields[i].set);
				lua_setfield(l, -2, P::_proxyFields[i].key);
			}
			_propertyProxyNewIndexTable.pull(l);

			lua_pop(l, 1);

		}

		inline static int _nextMetaMethodCFunction(lua_State* l) {
			_propertyProxyIndexTable.push(l);
			lua_insert(l, -2);

			//Proxies userdata iteration to built-in next iterator on index table
			const int retVal = lua_next(l, -2);
			if(retVal) {
				lua_pushvalue(l, 1);

				LuaManager::_protectedCall(1, 1, nullptr);
			}

			lua_remove(l, 1);
			lua_remove(l, 1);

			//Return <Key, Value> pair or nothing on stack
			return retVal? 2 : 0;

		}

		static int _indexMetaMethodCFunction(lua_State* l) {
			return _metaMethodCFunction(l, _ooluaIndexTable, _propertyProxyIndexTable, false);
		}

		static int _newIndexMetaMethodCFunction(lua_State* l) {
			return _metaMethodCFunction(l, _ooluaNewIndexTable, _propertyProxyNewIndexTable, true);
		}

		static int _pairsMetaMethodCFunction(lua_State* l) {
			//Pushes onto Lua stack:
			//1 - Userdata iterator function
			//2 - Userdata
			//3 - nil (first key)
			lua_pushcfunction(l, &_nextMetaMethodCFunction);
			lua_insert(l, 1);
			lua_pushnil(l);

			return 3;
		}

		//CRTP Overridable?
		static int _metaMethodCFunction(lua_State* l, LuaTable& ooluaIndexTable, LuaTable& propertyIndexTable, const bool newIndex) {
			char key[200];

			//stack state
			//1 - table ref
			//2 - key
			//(3) - arg [__newIndex only]

			//1) try protoTable if exists
			//2) try staticMetaTable
			//3) try propertyMetaTable if exists

			strcpy(key, lua_tostring(l, 2));
			assert(key);

			lua_remove(l, 2);

			//check the oolua methods table first!
			ooluaIndexTable.push(l);
			lua_getfield(l, -1, key);
			lua_remove(l, -2);
			//key found in OOLUA's methods table
			if(!lua_isnil(l, -1)) {
				lua_remove(l, 1);

				return 1;

			//key not found, try property proxy tables
			} else {

				lua_remove(l, -1);
				propertyIndexTable.push(l);
				lua_getfield(l, -1, key);
				lua_remove(l, -2);

				//key not found here either, we ain't got it (call parents' table)
				if(lua_isnil(l, -1)) {
					lua_pop(l, lua_gettop(l));
					return 0;

				} else {
					lua_insert(l, 1);

				//Call proxy accessor method
				LuaManager::_protectedCall(newIndex? 2 : 1, newIndex? 0 : 1, nullptr);


				return newIndex? 0 : 1;

				}

			}
		}


	public:

		void		setMetaTable(const LuaTable& ref) const;
		void		setProtoTable(const LuaTable& ref) const;
		LuaTable	getProtoTable(void) const;

		//override OOLUA's Proxy Class metatable
		//do any kind of static registration bullshit for fields
		static void registerProxyMetaTable(lua_State *l) {
			_registerPropertyMetaMethodTables(l);


			assert(OOLUA::INTERNAL::class_name_is_already_registered(*LuaManager::_mainState, OOLUA::Proxy_class<T>::class_name));

			luaL_getmetatable(*LuaManager::_mainState, OOLUA::Proxy_class<T>::class_name); //userdata metatable

			//Save OOLUA's method table for later
			lua_getfield(l, -1, "__index");
			_ooluaIndexTable.pull(l);

			//Save OOLUA's method table for later
			lua_getfield(l, -1, "__newindex");
			_ooluaNewIndexTable.pull(l);

			//Use our __index metamethod
			lua_pushcfunction(l, &_indexMetaMethodCFunction);
			lua_setfield(l, -2, "__index");

			//Use our __newindex metamethod
			lua_pushcfunction(l, &_newIndexMetaMethodCFunction);
			lua_setfield(l, -2, "__newindex");

			//Use our __pairs metamethod
			lua_pushcfunction(l, &_pairsMetaMethodCFunction);
			lua_setfield(l, -2, "__pairs");

			//Create a new __next metamethod for internal iteration
			lua_pushcfunction(l, &_nextMetaMethodCFunction);
			lua_setfield(l, -2, "__next");

			lua_pop(*LuaManager::_mainState, 1);

			assert(LuaManager::_stackDepth() == 0);

		}

	};


	template <typename P, typename T>
	LuaTable LuaTableProxy<P, T>::_staticMetaTable;

	template <typename P, typename T>
	LuaTable LuaTableProxy<P, T>::_ooluaIndexTable;

	template <typename P, typename T>
	LuaTable LuaTableProxy<P, T>::_ooluaNewIndexTable;

	template <typename P, typename T>
	LuaTable LuaTableProxy<P, T>::_propertyProxyIndexTable;

	template <typename P, typename T>
	LuaTable LuaTableProxy<P, T>::_propertyProxyNewIndexTable;



}

#endif // ELYSIAN_LUA_TABLE_PROXY_H
