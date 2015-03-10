TEMPLATE = subdirs
CONFIG += ordered
CONFIG += console


SUBDIRS +=  ../../libGyro/qtcreator/libGyro \
	../../Lua5.2/qtcreator/Lua5_2 \
	../../OOLua/qtcreator/OOLua \
	../../Spine/qtcreator/Spine \
	../../ESToolkit/ESToolkit
#    ../../ESGamma/qtcreator/ESGamma


#ESGamma.depends += libGyro \
 #       OOLua
ESToolkit.depends = libGyro \
	OOLua \
	Spine

