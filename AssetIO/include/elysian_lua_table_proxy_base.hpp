#ifndef ELYSIAN_LUA_TABLE_PROXY_BASE_HPP
#define ELYSIAN_LUA_TABLE_PROXY_BASE_HPP

#include "luatables.h"
#include "elysian_lua_manager.hpp"
#include <QDebug>

//DSL for autogenerating proxy fields
#define ELYSIAN_LUA_PROXY_FIELDS_BEGIN() \
    static constexpr LuaTableFieldProxy _proxyFields[] = {
    //static constexpr unsigned			_proxyFieldCount = n;

#define ELYSIAN_LUA_PROXY_FIELDS_END() }; \
    static constexpr unsigned _proxyFieldCount = sizeof(_proxyFields) / sizeof(LuaTableFieldProxy);

#define ELYSIAN_LUA_PROXY_FIELD_READ_WRITE(f, g, s) { #f, #g, #s },
#define ELYSIAN_LUA_PROXY_FIELD_READ_ONLY(f, g) { #f, #g, nullptr },

#define ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define ELYSIAN_LUA_PROXY_FIELD(...) ELYSIAN_LUA_PROXY_FIELD_GET_MACRO(__VA_ARGS__, ELYSIAN_LUA_PROXY_FIELD_READ_WRITE, ELYSIAN_LUA_PROXY_FIELD_READ_ONLY)(__VA_ARGS__)

#define ELYSIAN_LUA_PROXY_FIELDS_EXPORT(c) \
    constexpr unsigned				c::_proxyFieldCount; \
    constexpr LuaTableFieldProxy	c::_proxyFields[];

/*
#define ELYSIAN_OOLUA_PROXY_METHODS() \
    OOLUA_MEM_FUNC(void, fromTable, const OOLUA::Lua_table_ref&)

#define ELYSIAN_OOLUA_PROXY_METHODS_EXPORT() \
    fromTable

#define ELYSIAN_OOLUA_PROXY_METHODS_CONST_EXPORT()
*/

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
    class LuaTableProxyBase: public LuaManager {
	protected:

		static LuaTable _propertyProxyIndexTable;
		static LuaTable _propertyProxyNewIndexTable;
		static LuaTable _ooluaIndexTable;               //OOLUA's default index metamethod table
		static LuaTable _ooluaNewIndexTable;            //OOLUA's default new index metamethod table
        //static LuaTable _staticMetaTable;
        //LuaTable        _protoTable;

        void _onFieldAdded(const char*const key);
        void _onFieldChanged(const char*const key);

        static inline void _callPropertyProxyMethod(const bool write, const char*const /*key*/) {
            LuaManager::_protectedCall(write? 2 : 1, write? 0 : 1, nullptr);
        }

        static inline void  _registerPropertyMetaMethodTables(lua_State* l) {
			lua_getglobal(l, OOLUA::Proxy_class<T>::class_name);
            LuaManager::rPrintByIndex(-2, 99, OOLUA::Proxy_class<T>::class_name);

			//Register property pairs for index table <K=field, V=getFunc>
			lua_newtable(l);
            //qDebug() << "PROXY FIELD COUNT " << P::_proxyFieldCount;

			for(unsigned i = 0; i < P::_proxyFieldCount; ++i) {
                //Debug::printf(Debug::VERBOSE, "GET ACCESSOR - <%s, %s>", P::_proxyFields[i].key, P::_proxyFields[i].get);
				lua_getfield(l, -2, P::_proxyFields[i].get);
				lua_setfield(l, -2, P::_proxyFields[i].key);
			}
			_propertyProxyIndexTable.pull(l);

			//Register property pairs for newIndex table <K=field, V=setFunc>
			lua_newtable(l);
			for(unsigned i = 0; i < P::_proxyFieldCount; ++i) {
                if(P::_proxyFields[i].set == nullptr) continue; //read-only attribute!
               // Debug::printf(Debug::VERBOSE, "SET ACCESSOR - <%s, %s>", P::_proxyFields[i].key, P::_proxyFields[i].set);
				lua_getfield(l, -2, P::_proxyFields[i].set);
				lua_setfield(l, -2, P::_proxyFields[i].key);
			}
			_propertyProxyNewIndexTable.pull(l);

			lua_pop(l, 1);

		}

        inline static int _getNextKey(lua_State* l) {
            if(P::_proxyFieldCount <= 0) return 0;

            char key[100];
            int index = -1;

            if(lua_isstring(l, -1)) {
                strcpy(key, lua_tostring(l, -1));
                lua_pop(l, 1);
;
                for(unsigned i = 0; i < P::_proxyFieldCount; ++i) {
                    if(strcmp(key, P::_proxyFields[i].key) == 0) {
                        index = i;
                        break;
                    }
                }

                //Not even a valid key!
                if(index == -1) {
                    assert(0);
                    return 0;
                //We're at the end!
                } else if(index == (int)P::_proxyFieldCount-1) {
                    lua_pushnil(l);
                    return 1;
                //Return next key
                } else {
                    //qDebug() << "NEXT KEY " << P::_proxyFields[index+1].key;
                    lua_pushstring(l, P::_proxyFields[index+1].key);
                    return 1;
                }
            //assume this is a nil index, and we want the first key.
            } else {
                //qDebug() << "FIRST KEY " << P::_proxyFields[0].key;
                lua_pop(l, 1);
                lua_pushstring(l, P::_proxyFields[0].key);
                return 1;
            }
        }

        inline static int _iNextMetaMethodCFunction(lua_State* l) {
            if(lua_isnil(l, -2)) {
				//qDebug() << "__NEXT called with NIL";
                lua_pop(l, 2);
                return 0;
            } else {
				//qDebug() << "_iNextMetaMethodCFunction()";
            }

            unsigned index = lua_tonumber(l, -1);
			//qDebug() << "IPAIRS INDEX " << index;
            lua_pop(l, 1);
            if(++index > P::_proxyFieldCount) {
                lua_pop(l, 1);
                lua_pushnil(l);
                return 1;
            }

            lua_pushnumber(l, index);

            _propertyProxyIndexTable.push(l);
            lua_pushstring(l, P::_proxyFields[index-1].key);

            lua_rawget(l, -2);

            if(!lua_isnil(l, -1)) {
				//qDebug() << "found field!";
                lua_pushvalue(l, -4);

                LuaManager::_protectedCall(1, 1, nullptr);

                lua_remove(l, -4);
                lua_remove(l, -2);

                return 2;
            } else {
			   // qDebug() << "COULDNT FIND FIELD";
            }

            lua_pop(l, lua_gettop(l));
            return 0;

        }


        inline static int _nextMetaMethodCFunction(lua_State* l) {
            if(lua_isnil(l, -2)) {
			   // qDebug() << "__NEXT called with NIL";
                lua_pop(l, 2);
                return 0;
            } else {
			   // qDebug() << "_nextMetaMethodCFunction()";
            }

            if(_getNextKey(l)) {
			   // qDebug() << "getNextKey() - true";
                lua_pushvalue(l, -1);

                _propertyProxyIndexTable.push(l);
                lua_insert(l, -2);
                lua_rawget(l, -2);

                if(!lua_isnil(l, -1)) {
				  //  qDebug() << "found field!";
                    lua_pushvalue(l, -4);

                    LuaManager::_protectedCall(1, 1, nullptr);

                    lua_remove(l, -4);
                    lua_remove(l, -2);

                    return 2;
				} else;// qDebug() << "couldn't find field!";
            } else {
			 //   qDebug() << "getNextKey() - FALSE";
            }

            lua_pop(l, lua_gettop(l));
            return 0;

        }


        static int _lengthMetaMethodCFunction(lua_State* l) {
            lua_pop(l, lua_gettop(l));
            lua_pushinteger(l, P::_proxyFieldCount);
            return 1;
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

        static int _ipairsMetaMethodCFunction(lua_State* l) {
            //Pushes onto Lua stack:
            //1 - Userdata iterator function
            //2 - Userdata
            //3 - nil (first key)
            lua_pushcfunction(l, &_iNextMetaMethodCFunction);
            lua_insert(l, 1);
            lua_pushnumber(l, 0);

		   // qDebug() << "IPAIRS";

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

                //key not found here either, we ain't got it -- INSERT THE BITCH IF NEWINDEX, NOTIFY IF ESTK
				if(lua_isnil(l, -1)) {
					lua_pop(l, lua_gettop(l));

                    //retrieve object, notify it of change
                    //instance._onFieldAdded("ADDED");


					return 0;

				} else {
					lua_insert(l, 1);

                    //userdata is index 2
                   //  OOLUA::push(state, static_cast<T*>(static_cast<P*>(this)));


                    //Invoke proxy accessor method
                    _callPropertyProxyMethod(newIndex, key);

                    if(newIndex) {
                        //instance._onFieldChanged("CHANGED");
                        return 0;
                    } else {
                        return 1;
                    }

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
            lua_pushcfunction(l, &_luaFromTableCFunction);
            lua_setfield(l, -2, "fromTable");
			_ooluaIndexTable.pull(l);

			//Save OOLUA's method table for later
			lua_getfield(l, -1, "__newindex");
			_ooluaNewIndexTable.pull(l);

            P::_addMetaMethods(l);

            //Create a new __next metamethod for internal iteration
            //_propertyProxyIndexTable.push(l);
            //lua_setfield(l, -2, "__properties");

            //lua_pushcfunction(l, &_getNextKey);
            //lua_setfield(l, -2, "__nextkey");

			lua_pop(*LuaManager::_mainState, 1);

			assert(LuaManager::_stackDepth() == 0);

		}

        static void _addMetaMethods(lua_State* l) {
            //Use our __index metamethod
            lua_pushcfunction(l, &_indexMetaMethodCFunction);
            lua_setfield(l, -2, "__index");

            //Use our __newindex metamethod
            lua_pushcfunction(l, &_newIndexMetaMethodCFunction);
            lua_setfield(l, -2, "__newindex");

            //Use our __pairs metamethod
            lua_pushcfunction(l, &_pairsMetaMethodCFunction);
            lua_setfield(l, -2, "__pairs");

            lua_pushcfunction(l, &_ipairsMetaMethodCFunction);
            lua_setfield(l, -2, "__ipairs");

            //Create a new __next metamethod for internal iteration
            lua_pushcfunction(l, &_nextMetaMethodCFunction);
            lua_setfield(l, -2, "__next");

            lua_pushcfunction(l, &_lengthMetaMethodCFunction);
            lua_setfield(l, -2, "__len");
        }

        static int _luaFromTableCFunction(lua_State*const state) {
		//    qDebug() << "!!!!!IN THE C FUNCTION!";

            lua_pushnil(state);
            while (lua_next(state, -2) != 0) {

                _propertyProxyNewIndexTable.push(state);
                lua_pushvalue(state, -3);

                lua_rawget(state, -2);
                if(!lua_isnil(state, -1)) {
				  //  qDebug() << "found field!";
                    lua_pushvalue(state, -6);
                    lua_pushvalue(state, -4);

                    LuaManager::_protectedCall(2, 0, nullptr);
				 //   qDebug() << "returned";

                    lua_pop(state, 1);

                } else {
			  //      qDebug() << "DIDNT FIND FIELD";
                    lua_pop(state, 2);
                }

                //pop value
                lua_pop(state, 1);
            }

            //pop table
            lua_pop(state, lua_gettop(state));
            return 0;
        }

        void fromTable(OOLUA::Lua_table_ref ref) {
            lua_State*const state = *LuaManager::_mainState;

            OOLUA::push(state, static_cast<T*>(static_cast<P*>(this)));
            //OOLUA::push(state, static_cast<T*>(this));
            ref.push(state);

            lua_pushcfunction(state, &_luaFromTableCFunction);
            lua_insert(state, -3);
            LuaManager::_protectedCall(2, 0, nullptr);
        }

	};

/*
	template <typename P, typename T>
	LuaTable LuaTableProxy<P, T>::_staticMetaTable;
*/
	template <typename P, typename T>
    LuaTable LuaTableProxyBase<P, T>::_ooluaIndexTable;

	template <typename P, typename T>
    LuaTable LuaTableProxyBase<P, T>::_ooluaNewIndexTable;

	template <typename P, typename T>
    LuaTable LuaTableProxyBase<P, T>::_propertyProxyIndexTable;

	template <typename P, typename T>
    LuaTable LuaTableProxyBase<P, T>::_propertyProxyNewIndexTable;

    template <typename P, typename T>
    inline void LuaTableProxyBase<P,T>::_onFieldAdded(const char*const /*key*/) { }

    template <typename P, typename T>
    inline void LuaTableProxyBase<P,T>::_onFieldChanged(const char*const /*key*/) { }



}

#endif // ELYSIAN_LUA_TABLE_PROXY_H
