# Copyright (c) 2013-2026 Elysian Shadows Team
# SPDX-License-Identifier: MIT
# Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.

cd ../../executable
rm -rf *.dll
windeployqt --release --compiler-runtime ESToolkit.exe
cd ../libGyro/lib/win32
cp *.dll ../../../executable
chmod -R 777 ../../../executable