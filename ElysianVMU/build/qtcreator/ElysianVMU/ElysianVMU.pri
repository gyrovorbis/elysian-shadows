exists( EVMUCommandLineArgs.pri ) {
    message("Including EVMUCommandLineArgs.pri in project file $${PWD}")
    include( ESDevCommandLineArgs.pri )
}

DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_Qt

!iphoneos:!dreamcast:!android:!raspberrypi:!iphonesimulator {
    QMAKE_CFLAGS += -msse
    QMAKE_CXXFLAGS += -msse
}

