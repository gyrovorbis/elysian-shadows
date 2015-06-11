exists( ESDevCommandLineArgs.pri ) {
	message("Including ESDevCommandLineArgs.pri in project file")
	include( ESDevCommandLineArgs.pri )
}


#SPEED UP MINGW'S FUCKTARDEDLY SLOW DUMB ASS!
win32 {
        #QMAKE_CXX=../../../qtcreator/ESDev/ccache.exe g++
        #QMAKE_CC=../../../qtcreator/ESDev/ccache.exe gcc

    #DEFINES += QT_NO_OPENGL_ES_2
}

QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

CONFIG(debug, debug|release) {
#	QMAKE_CXXFLAGS += -O0
#	QMAKE_CFLAGS += -O0
}

CONFIG(release, debug|release) {
#	QMAKE_CXXFLAGS += -03
#	QMAKE_CFLAGS += -03
}

DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_Qt QT_NO_KEYWORDS QT_NO_DYNAMIC_CAST
QMAKE_CXXFLAGS += -std=c++11 -msse -Wno-unknown-warning-option #-fno-rtti -fno-exceptions
CONFIG += c++11
CONFIG -= exceptions rtti

#Required to get C++11x to work on OSX
macx {
  QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++ -Wno-macro-redefined
  LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
}

tests {
	DEFINES += ESTK_UNIT_TESTS
}

