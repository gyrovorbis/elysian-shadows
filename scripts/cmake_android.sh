#!/bin/sh
# CMake command for Android

SCRIPTDIR=`dirname $0`
TOOLCHAINDIR=$SCRIPTDIR/../libGyro/scripts/cmake/toolchains
pushd $TOOLCHAINDIR > /dev/null
TOOLCHAINDIR=`pwd`
popd > /dev/null

cmake -DGYRO_PLATFORM=GYRO_PLATFORM_ANDROID -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAINDIR/toolchain.android.cmake $@

