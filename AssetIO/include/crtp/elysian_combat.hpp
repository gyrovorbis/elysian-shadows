#ifndef ELYSIAN_COMBAT_HPP
#define ELYSIAN_COMBAT_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_combat_base.hpp>
namespace elysian {
    typedef CombatBase Combat;
}
#else
#   include <tk_combat.h>
namespace elysian {
    typedef TkCombat Combat;
}
typedef TkCombat Combat;

#endif

#endif // ELYSIAN_COMBAT_HPP

