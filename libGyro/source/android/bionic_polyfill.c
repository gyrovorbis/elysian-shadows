/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

/*
 * So Bionic libc (Android's C library) is missing some crucial
 * parts (or at least they aren't visible to the linker), so
 * we implement them here.
 */

int abs(int n)
{
	return (n < 0) ? -n : n;
}
