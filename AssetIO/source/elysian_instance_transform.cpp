#include "elysian_instance_transform.hpp"
#include "elysian_lua_wrappers.hpp"

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Vector2Proxy)
    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Vector3Proxy)
	ELYSIAN_LUA_PROXY_FIELDS_EXPORT(InstanceTransform)

    void InstanceTransform::setPosVec(const OOLUA::Lua_table_ref valTable) { static_cast<Vector3Proxy*>(&_pos)->fromTable(valTable); }
    void InstanceTransform::setSizeVec(const OOLUA::Lua_table_ref valTable) { static_cast<Vector3Proxy*>(&_size)->fromTable(valTable); }
    void InstanceTransform::setScaleVec(const OOLUA::Lua_table_ref valTable) { static_cast<Vector3Proxy*>(&_scale)->fromTable(valTable); }

}
