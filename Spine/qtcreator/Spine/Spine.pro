include(../../../qtcreator/ESDev/ESDev.pri)
TEMPLATE = lib
CONFIG += staticlib

CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../include \
				../../../libGyro/include
QMAKE_CFLAGS += -Wno-unused-parameter

SOURCES += \
    ../../src/spine/Animation.c \
    ../../src/spine/AnimationState.c \
    ../../src/spine/AnimationStateData.c \
    ../../src/spine/Atlas.c \
    ../../src/spine/AtlasAttachmentLoader.c \
    ../../src/spine/Attachment.c \
    ../../src/spine/AttachmentLoader.c \
    ../../src/spine/Bone.c \
    ../../src/spine/BoneData.c \
    ../../src/spine/BoundingBoxAttachment.c \
    ../../src/spine/Event.c \
    ../../src/spine/EventData.c \
    ../../src/spine/extension.c \
    ../../src/spine/IkConstraint.c \
    ../../src/spine/IkConstraintData.c \
    ../../src/spine/Json.c \
    ../../src/spine/MeshAttachment.c \
    ../../src/spine/RegionAttachment.c \
    ../../src/spine/Skeleton.c \
    ../../src/spine/SkeletonBounds.c \
    ../../src/spine/SkeletonData.c \
    ../../src/spine/SkeletonJson.c \
    ../../src/spine/Skin.c \
    ../../src/spine/SkinnedMeshAttachment.c \
    ../../src/spine/Slot.c \
    ../../src/spine/SlotData.c \
    ../../src/spine/spine-libgyro.c

HEADERS += \
    ../../include/spine/Animation.h \
    ../../include/spine/AnimationState.h \
    ../../include/spine/AnimationStateData.h \
    ../../include/spine/Atlas.h \
    ../../include/spine/AtlasAttachmentLoader.h \
    ../../include/spine/Attachment.h \
    ../../include/spine/AttachmentLoader.h \
    ../../include/spine/Bone.h \
    ../../include/spine/BoneData.h \
    ../../include/spine/BoundingBoxAttachment.h \
    ../../include/spine/Event.h \
    ../../include/spine/EventData.h \
    ../../include/spine/extension.h \
    ../../include/spine/IkConstraint.h \
    ../../include/spine/IkConstraintData.h \
    ../../include/spine/MeshAttachment.h \
    ../../include/spine/RegionAttachment.h \
    ../../include/spine/Skeleton.h \
    ../../include/spine/SkeletonBounds.h \
    ../../include/spine/SkeletonData.h \
    ../../include/spine/SkeletonJson.h \
    ../../include/spine/Skin.h \
    ../../include/spine/SkinnedMeshAttachment.h \
    ../../include/spine/Slot.h \
    ../../include/spine/SlotData.h \
    ../../include/spine/spine.h \
    ../../src/spine/Json.h
