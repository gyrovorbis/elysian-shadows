
#include <proxy/elysian_level_system_proxy.hpp>

/*!
    \qmltype LevelSystem
    \since 1.6.0
    \inmodule Engine
    \brief Engine subsystem for managing the active level.

    The LevelSystem is the \l{Engine} subsystem for managing the active \l{Level}s
    and \l{Area}s. It caches old ones, loads new ones, and handles swapping between
    them.

    \section2 Retrieving the LevelSystem
    The LevelSystem object can be retrieved through the static \l{Engine} class:
    \code
    levelSys = Engine.getLevelSys()
    \endcode

    \section2 Caching Previous Areas
    The LevelSystem maintains a cache of previously visited \l{Area}s internally to
    reduce load times when swapping back to a previously visited \l{Area} and allowing
    for states such as broken barrels and enemy positions to persist beyond warps when
    reentering. Due to platform limitations, the total number of \l{Area}s that are cached
    is not guaranteed or deterministic. Something like a PC may cache 5 or so, while the
    Dreamcast will not cache any previous levels. Do not rely on caching behavior for anything.

    \sa Level, Area, Engine
*/

/*!
 * \qmlmethod string LevelSystem::getPrevAreaPath()
 *
 *  Returns the full asset-relative path to the \l{Area} that was active before the current one or nil if there was no active \l{Area}.
 * \sa Area, getPrevLevelPath
 */

/*!
 * \qmlmethod string LevelSystem::getPrevLevelPath()
 *
 *  Returns the full asset-relative path to the \l{Level} that was active before the current one or nil if there was no active \l{Level}.
 * \sa Level, getPrevAreaPath
 */

OOLUA_EXPORT_FUNCTIONS(LevelSystem)
OOLUA_EXPORT_FUNCTIONS_CONST(LevelSystem,
                                getPrevAreaPath,
                                getPrevLevelPath)
