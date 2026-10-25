# Copyright (c) 2013-2026 Elysian Shadows Team
# SPDX-License-Identifier: MIT
# Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.

(set -o igncr) 2>/dev/null && set -o igncr; # this comment is required
LIBGYRO_PATH=$1
cd ../executable
rm -rf *.dll
windeployqt --release --compiler-runtime ElysianVMU.exe
#cd ../libGyro/lib/win32
cp $LIBGYRO_PATH/lib/win32/SDL2.dll ./
cp $LIBGYRO_PATH/lib/win32/OpenAL32.dll ./
cp $LIBGYRO_PATH/lib/win32/SDL2_net.dll ./