/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/core/tk_undo_item.h>

const QString UndoItem::text[TYPE_SIZE] =
{
    "None",
    "Place Tile(s)",
    "Place Tile(s)",
    "Rename Area",
    "Area String",
    "Area Width",
    "Area Height",
    "Tile Collider",
    "Tile Animated",
    "Tile Anim Index",
    "Tile Anim Time",
    "Tile Stand On",
    "Tile Transparency",
    "Tile Normal Map",
    "Tile Specular Map",
    "Tile Group Collider",
    "Tile Group StandOn",
    "Tile Group Transparency",
    "Tile Group Normal Map",
    "Tile Group Specular Map",
    "Entity Deleted",
    "Lua Table Field"

};
