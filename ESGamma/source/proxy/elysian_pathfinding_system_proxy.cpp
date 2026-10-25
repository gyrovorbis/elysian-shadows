/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/proxy/elysian_pathfinding_system_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(PathfindingSystemNode)
OOLUA_EXPORT_FUNCTIONS_CONST(PathfindingSystemNode, getPredecessor, getDistance)

OOLUA_EXPORT_FUNCTIONS(PathfindingSystem, findAllPathsTo)
OOLUA_EXPORT_FUNCTIONS_CONST(PathfindingSystem, getTileNode)
