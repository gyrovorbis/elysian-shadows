TEMPLATE = subdirs
CONFIG 	+= console


SUBDIRS +=  ../../libGyro/qtcreator/libGyro \
	../../Lua5.2/qtcreator/Lua5_2 \
	../../OOLua/qtcreator/OOLua \
	../../Spine/qtcreator/Spine \
	../../ESToolkit/ESToolkit
#    ../../ESGamma/qtcreator/ESGamma

libGyro.depends = Spine

OOLua.depends = Lua5_2

#ESGamma.depends += libGyro \
 #       OOLua
ESToolkit.depends = libGyro \
	OOLua \
	Spine

tests {
	SUBDIRS += UnitTests

}

