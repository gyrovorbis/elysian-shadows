exists( EVMUCommandLineArgs.pri ) {
    include( EVMUCommandLineArgs.pri )
}

DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_Qt

!iphoneos:!dreamcast:!android:!raspberrypi:!iphonesimulator {
    QMAKE_CFLAGS += -msse -std=c99
    QMAKE_CXXFLAGS += -msse
}

