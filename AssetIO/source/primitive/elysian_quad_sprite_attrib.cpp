/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <AssetIO/primitive/elysian_quad_sprite_attrib.hpp>

namespace elysian {

const GYVector4 QuadSpriteAttrib::_verts[4] =
{
	{ -0.5f, -0.5f, 0.0f, 1.0f },
	{ 0.5f, -0.5f, 0.0f, 1.0f },
	{ -0.5f, 0.5f, 0.0f, 1.0f },
	{ 0.5f, 0.5f, 0.0f, 1.0f }
};

}
