/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef SHEET_MANAGER_ASSET_MANAGER_H
#define SHEET_MANAGER_ASSET_MANAGER_H

class SheetManagerAssets;

class SheetManagerAssetManager {
	friend class SheetManager;
private:

	static SheetManagerAssets *_assets;
public:
	static inline SheetManagerAssets *getSMAssets(void) { return _assets; }

};



#endif // SHEEMANAGERASSETMANAGER_H
