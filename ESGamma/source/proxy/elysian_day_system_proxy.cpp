#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_day_system_proxy.hpp>




OOLUA_EXPORT_FUNCTIONS_CONST(DaySystem,
                             getDays,
                             getHours,
                             getMinutes,
                             getSunLightAttrib)
OOLUA_EXPORT_FUNCTIONS(DaySystem,
                       setTimeDilation,
                  //    setSunEnabled,
                       setTimeOfDay)

