/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_LEVEL_MANAGER_HPP
#define ELYSIAN_LEVEL_MANAGER_HPP

#include <AssetIO/utility/elysian_asset_typedefs.hpp>

ELYSIAN_ASSET_TYPEDEF(Level)
ELYSIAN_ASSET_TYPEDEF(Area)

namespace elysian {

	class LevelManager { 
	protected:
        static Level*   _curLevel;
        static Area*    _curArea;
	public:
        static Area*    getCurrentArea(void);
        static Level*   getCurrentLevel(void);
	}; 
	






    //===== INLINEZ =====
    inline Area* LevelManager::getCurrentArea(void) { return _curArea; }
    inline Level* LevelManager::getCurrentLevel(void) { return _curLevel; }
}

#endif

