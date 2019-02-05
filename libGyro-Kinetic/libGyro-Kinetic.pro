#-------------------------------------------------
#
# Project created by QtCreator 2018-07-23T10:57:20
#
#-------------------------------------------------

QT       -= core gui

TARGET = libGyroKinetic
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

isEmpty(LIBGYRO_PATH) {
    LIBGYRO_PATH = lib/libGyro
}

INCLUDEPATH +=  api \
                include \
                $${LIBGYRO_PATH}/api \
                $${LIBGYRO_PATH}/include

DEPENDPATH += $${LIBGYRO_PATH}


SOURCES += \
    source/gyro_kinetic_collision.c \
    source/gyro_kinetic_physics.c

HEADERS += \
    api/libGyro-Kinetic/gyro_kinetic.h \
    api/libGyro-Kinetic/gyro_kinetic_collider.h \
    api/libGyro-Kinetic/gyro_kinetic_collision.h \
    api/libGyro-Kinetic/gyro_kinetic_contact.h \
    api/libGyro-Kinetic/gyro_kinetic_kinetic.h \
    api/libGyro-Kinetic/gyro_kinetic_physics.h \
    api/libGyro-Kinetic/gyro_kinetic_static.h \
    v2_api/gyro_physics_simulation.h \
    v2_api/gyro_physics_primitives.h
