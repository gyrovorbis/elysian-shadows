include(ESDev.pri)

TEMPLATE = subdirs
CONFIG += ordered
CONFIG 	+= console

SUBDIRS +=  ../../libGyro/qtcreator/libGyro\
    ../../Spine/qtcreator/Spine

estk {
    SUBDIRS += ../../Lua5.2/qtcreator/Lua5_2 \
    ../../OOLua/qtcreator/OOLua \
    ../../ESToolkit/ESToolkit
}

tests {
    SUBDIRS += ../../UnitTests
}

vmu {
    SUBDIRS += ../../ElysianVMU/qtcreator/ElysianVMU
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
