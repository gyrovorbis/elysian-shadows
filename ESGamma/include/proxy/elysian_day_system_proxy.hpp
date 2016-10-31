#ifndef ELYSIAN_DAY_SYSTEM_PROXY_HPP
#define ELYSIAN_DAY_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <system/elysian_day_system.hpp>

namespace OOLUA {
    typedef elysian::DaySystem  DaySystem;

}

OOLUA_PROXY(DaySystem)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MFUNC_CONST(getDays)
    OOLUA_MFUNC_CONST(getHours)
    OOLUA_MFUNC_CONST(getMinutes)
    OOLUA_MFUNC_CONST(getSunLightAttrib)
    OOLUA_MFUNC(setTimeDilation)
//    OOLUA_MFUNC(setSunEnabled)
    OOLUA_MEM_FUNC(void, setTimeOfDay, float)
OOLUA_PROXY_END


#endif // ELYSIAN_DAY_SYSTEM_PROXY_HPP

