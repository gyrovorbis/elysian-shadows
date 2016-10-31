#include <proxy/elysian_entity_proxy.hpp>

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Behavior,
                                   addTrigger,
                                   removeTrigger)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Behavior,
                                getBehavName)

OOLUA_EXPORT_FUNCTIONS(LuaBehavior,
                        getSelfTable,
                        setTriggers,
                        setBehavName,
                        setParamTable,
                        getParamTable)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaBehavior,
                             getTriggers,
                             getScript)
