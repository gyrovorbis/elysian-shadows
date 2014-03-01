TEMPLATE = subdirs
CONFIG += ordered
CONFIG += console

SUBDIRS += ../../libGyro/qtcreator/libGyro \
	../../OOLua/qtcreator/OOLua \
    ../../ESGamma/qtcreator/ESGamma \
#    ../../ESToolkit/ESToolkit \

ESGamma.depends += libGyro \
	OOLua
ESToolkit.depends = libGyro

