exists( ESDevCommandLineArgs.pri ) {
    message("Including ESDevCommandLineArgs.pri in project file")
    include( ESDevCommandLineArgs.pri )
}

dreamcast {

    INCLUDEPATH=/home/jenkins/Dreamcast/cadcdev-kallistios/include \
        /home/jenkins/Dreamcast/cadcdev-kos-ports/include \
        /home/jenkins/Dreamcast/cadcdev-kallistios/kernel/arch/dreamcast/include \
        /home/jenkins/Dreamcast/cadcdev-kallistios/addons/include

    DEFINES+=_arch_dreamcast \
        _arch_sub_pristine \
        ELYSIAN_ENGINE
}

macx {
    QMAKE_MAC_SDK = macosx10.11
}

CONFIG -= exceptions
#CONFIG += warn_off


#SPEED UP MINGW'S FUCKTARDEDLY SLOW DUMB ASS!
win32 {
#    QMAKE_CFLAGS_RELEASE +=-g -O1
#    QMAKE_CXXFLAGS_RELEASE +=-g -O1
#    QMAKE_LFLAGS_RELEASE=
}

ios|android {
    DEFINES += ELYSIAN_ENGINE
}

macx {
    QMAKE_CXXFLAGS_WARN_ON -= -Wall
}

!dreamcast {
    QMAKE_CXXFLAGS += -Wall -Wno-unused-local-typedef -Wno-unknown-pragmas -Wno-inconsistent-missing-override
}
#QMAKE_CFLAGS += -std=c99

!iphoneos:!dreamcast:!android {
    QMAKE_CFLAGS += -msse
    QMAKE_CXXFLAGS += -msse
}

CONFIG(debug, debug|release) {
#	QMAKE_CXXFLAGS += -O0//
#	QMAKE_CFLAGS += -O0
}

CONFIG(release, debug|release) {
#	QMAKE_CXXFLAGS += -03
#	QMAKE_CFLAGS += -03
}

DEFINES += QT_NO_KEYWORDS QT_NO_DYNAMIC_CAST

android {
    DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_ANDROID
    QMAKE_CFLAGS    += -mfpu=neon
    QMAKE_CXXFLAGS  += -mfpu=neon
        #QMAKE_ANDROID_DEPLOYMENT_TARGET=8.2
        #QMAKE_ANDROID_TARGETED_DEVICE_FAMILY=1,2
} else: ios {
    DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_iOS
    iphonesimulator {
        DEFINES += GYRO_PLATFORM_iOS_SIMULATOR
    }
    iphoneos {
        #QMAKE_CFLAGS += -target armv8-apple-none-eabi -mcpu=cortex-a8 -mfpu=neon -miphoneos-version-min=7.0

        QMAKE_CXXFLAGS += -fno-rtti -fno-exceptions
        #QMAKE_CXXFLAGS += -mcpu=cortex-a8 -mfpu=neon -target armv8-apple-none-eabi -miphoneos-version-min=7.0å
        #QMAKE_CFLAGS += -target armv8-apple-none-eabi
        #QMAKE_CXXFLAGS += -target armv8-apple-none-eabi
        QMAKE_IOS_DEVICE_ARCHS=armv7
        QMAKE_IOS_DEPLOYMENT_TARGET=8.2
        QMAKE_IOS_TARGETED_DEVICE_FAMILY=1,2
        DEFINES += GYRO_PLATFORM_iOS_DEVICE
        #CONFIG+=neon
    }
}
else:dreamcast {
    DEFINES+=GYRO_PLATFORM=GYRO_PLATFORM_DC
} else {
    DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_Qt
}



    QMAKE_CXXFLAGS += -std=c++14
!dreamcast {
QMAKE_CXXFLAGS += -Wno-unknown-warning-option #-fno-rtti -fno-exceptions
    CONFIG += c++14
}

CONFIG -= exceptions rtti

#Required to get C++11x to work on OSX
macx {
  QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++ -Wno-macro-redefined
  LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
}

tests {
    DEFINES += ESTK_UNIT_TESTS
}


