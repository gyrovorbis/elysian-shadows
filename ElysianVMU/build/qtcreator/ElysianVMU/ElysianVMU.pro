TEMPLATE = subdirs

isEmpty(LIBGYRO_PATH) {
    LIBGYRO_PATH = ../../../evmu-core/libGyro
    LIBGYRO_BUILD_PATH  = $${LIBGYRO_PATH}/qtcreator/libGyro
}

SUBDIRS += ../EVMU-App \
            ../../../evmu-core/build/qtcreator/EVMU-Core

!vmu_child_app {
    SUBDIRS += $${LIBGYRO_BUILD_PATH}
    EVMU-Core.depends = libGyro
    EVMU-App.depends = libGyro
}

EVMU-App.depends += EVMU-Core

