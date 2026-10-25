TEMPLATE = subdirs
CONFIG += ordered

#if this isn't set, assuming to be the top-level project in charge of
#setting all configuration paths
isEmpty(LIBGYRO_PATH) {
    include(ElysianVMU.pri)
    LIBGYRO_PATH = ../../../evmu-core/libGyro
    LIBGYRO_BUILD_PATH  = $${LIBGYRO_PATH}/qtcreator/libGyro
}

!vmu_child_app {
    SUBDIRS += $${LIBGYRO_BUILD_PATH}
}

SUBDIRS += ../../../evmu-core/build/qtcreator/EVMU-Core \
    ../EVMU-App

!vmu_child_app {
    EVMU-Core.depends = libGyro
    EVMU-App.depends = libGyro
}

EVMU-App.depends += EVMU-Core

