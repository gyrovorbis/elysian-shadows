TEMPLATE = subdirs
CONFIG += ordered
CONFIG += console


SUBDIRS +=  ../../ESToolkit/ESToolkit \
	../../libGyro/qtcreator/libGyro \
	../../Lua5.2/qtcreator/Lua5_2 \
	../../OOLua/qtcreator/OOLua
#    ../../ESGamma/qtcreator/ESGamma


#ESGamma.depends += libGyro \
 #       OOLua
ESToolkit.depends = libGyro \
	OOLua

