#ifndef ELYSIAN_PATHFINDING_SYSTEM_PROXY_HPP
#define ELYSIAN_PATHFINDING_SYSTEM_PROXY_HPP

#include <oolua.h>
#include <system/elysian_pathfinding_system.hpp>

namespace OOLUA {
typedef elysian::PathfindingSystem PathfindingSystem;
typedef elysian::PathfindingSystem::Node PathfindingSystemNode;
}

OOLUA_PROXY(PathfindingSystemNode)
OOLUA_MEM_FUNC_CONST(maybe_null<PathfindingSystemNode*>, getPredecessor)
OOLUA_MEM_FUNC_CONST(int, getDistance)
OOLUA_PROXY_END

OOLUA_PROXY(PathfindingSystem)
OOLUA_TAGS(No_public_constructors, No_public_destructor)
OOLUA_MEM_FUNC(void, findAllPathsTo, const unsigned short, const unsigned short,
               const int)
OOLUA_MEM_FUNC_CONST(maybe_null<const PathfindingSystemNode*>, getTileNode,
                     const unsigned short, const unsigned short)
OOLUA_PROXY_END
#endif  // ELYSIAN_PATHFINDING_SYSTEM_PROXY_HPP
