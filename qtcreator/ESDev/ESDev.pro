include(ESDev.pri)

TEMPLATE = subdirs
CONFIG += ordered
CONFIG 	+= console

SUBDIRS +=  ../../libGyro/qtcreator/libGyro\
    ../../Spine/qtcreator/Spine

estk {
    SUBDIRS += ../../Lua5.2/qtcreator/Lua5_2 \
    ../../OOLua/qtcreator/OOLua
}

tests {
    SUBDIRS += ../../UnitTests
}

vmu_child_app {
    DEFINES += VMU_CHILD_APP
    SUBDIRS += ../../ElysianVMU/qtcreator/ElysianVMU
}

estk {
    SUBDIRS += ../../ESToolkit/ESToolkit
}

vmu {
    SUBDIRS += ../../ElysianVMU/qtcreator/ElysianVMU
}


vulkan {
    DEFINES+=LIBGYRO_VULKAN
}


libGyro.depends = Spine

estk {

    OOLua.depends = Lua5_2

    ESToolkit.depends += libGyro \
      OOLua \
      Spine

}

tests {
    ESToolkit.depends += UnitTests
}

vmu {
    ElysianVMU.depends += libGyro
}

vmu_child_app {
    ElysianVMU.depends += libGyro
    ESToolkit.depends += ElysianVMU
}

