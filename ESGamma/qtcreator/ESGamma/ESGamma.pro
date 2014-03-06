TEMPLATE = app
CONFIG += console
#CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
#CONFIG += -std=c99

DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_x86 \
    ELYSIAN_ENGINE

QMAKE_CXXFLAGS += -std=c++11 \
    -fno-rtti \
        -fno-exceptions

win32 {
	QMAKE_CXXFLAGS += -msse
        QMAKE_CFLAGS += -msse -std=c99
          # QMAKE_CFLAGS += -std=c99
}

#Required to get C++11x to work on OSX
macx {
  QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
  LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
  CONFIG += objective_c
}

INCLUDEPATH += ../../include \
    ../../lib/include \
    ../../../AssetIO/include \
    ../../../libGyro/include \
    ../../../libGyro/include/kinetic \
    ../../../OOLua/include

win32 {
    INCLUDEPATH += ../../../libGyro/lib/win32
}

macx {
    INCLUDEPATH += ../../../libGyro/lib/osx
    INCLUDEPATH += ../../../libGyro/lib/osx/SDL.framework/Headers
}

DEPENDPATH += ../../include \
    ../../lib/include \
    ../../../AssetIO/include \
    ../../../libGyro/include \
	../../../libGyro/include/kinetic \
	../../../libGyro/include/glal \
	../../../OOLua/include \
	../../../libGyro/source/glal/shaders \
	../../../libGyro/include/x86

HEADERS += \
    ../../include/elysian_warp_system.hpp \
    ../../include/elysian_timer_system.hpp \
    ../../include/elysian_system.hpp \
    ../../include/elysian_sprite.hpp \
    ../../include/elysian_rect.hpp \
    ../../include/elysian_quad_sprite.hpp \
    ../../include/elysian_quad.hpp \
    ../../include/elysian_party_system.hpp \
    ../../include/elysian_model.hpp \
    ../../include/elysian_map_system.hpp \
    ../../include/elysian_lua_wrappers.hpp \
    ../../include/elysian_lua_singleton.hpp \
    ../../include/elysian_lua_script.hpp \
    ../../include/elysian_lua_manager.hpp \
    ../../include/elysian_level_system.hpp \
    ../../include/elysian_item_system.hpp \
    ../../include/elysian_gui_system.hpp \
    ../../include/elysian_game_input.hpp \
    ../../include/elysian_friendly_system.hpp \
    ../../include/elysian_font.hpp \
    ../../include/elysian_engine.hpp \
    ../../include/elysian_enemy_system.hpp \
    ../../include/elysian_dialog_system.hpp \
    ../../include/elysian_debug.hpp \
    ../../include/elysian_collision_system.hpp \
    ../../include/elysian_character_system.hpp \
    ../../include/elysian_camera_system.hpp \
    ../../include/elysian_behavior_system.hpp \
    ../../include/elysian_animation_system.hpp \
    ../../include/elysian_adventure_log.hpp \
    ../../../AssetIO/include/elysian_zcoords.hpp \
    ../../../AssetIO/include/elysian_warp.hpp \
    ../../../AssetIO/include/elysian_tile.hpp \
    ../../../AssetIO/include/elysian_textured_attrib.hpp \
    ../../../AssetIO/include/elysian_stl_algorithm.hpp \
    ../../../AssetIO/include/elysian_sprite_attrib.hpp \
    ../../../AssetIO/include/elysian_renderable_object.hpp \
    ../../../AssetIO/include/elysian_renderable.hpp \
    ../../../AssetIO/include/elysian_rect_attrib.hpp \
    ../../../AssetIO/include/elysian_quad_sprite_attrib.hpp \
    ../../../AssetIO/include/elysian_quad_attrib.hpp \
    ../../../AssetIO/include/elysian_prim_attrib.hpp \
    ../../../AssetIO/include/elysian_pool.hpp \
    ../../../AssetIO/include/elysian_model_attrib.hpp \
    ../../../AssetIO/include/elysian_map_depth_buffer.hpp \
    ../../../AssetIO/include/elysian_lua_behavior.hpp \
    ../../../AssetIO/include/elysian_linked_list.hpp \
    ../../../AssetIO/include/elysian_level_manager.hpp \
    ../../../AssetIO/include/elysian_level.hpp \
    ../../../AssetIO/include/elysian_item.hpp \
    ../../../AssetIO/include/elysian_instance_transform.hpp \
    ../../../AssetIO/include/elysian_friendly.hpp \
    ../../../AssetIO/include/elysian_examinable.hpp \
    ../../../AssetIO/include/elysian_entity.hpp \
    ../../../AssetIO/include/elysian_enemy.hpp \
    ../../../AssetIO/include/elysian_direction.hpp \
    ../../../AssetIO/include/elysian_dialog.hpp \
    ../../../AssetIO/include/elysian_cstr_map.hpp \
    ../../../AssetIO/include/elysian_component.hpp \
    ../../../AssetIO/include/elysian_collidable.hpp \
    ../../../AssetIO/include/elysian_character.hpp \
    ../../../AssetIO/include/elysian_behavior.hpp \
    ../../../AssetIO/include/elysian_asset.hpp \
    ../../../AssetIO/include/elysian_area.hpp \
    ../../../AssetIO/include/elysian_animation.hpp \
    ../../../AssetIO/include/elysian_audio_source.hpp \
    ../../include/elysian_audio_system.hpp \
    ../../../AssetIO/include/elysian_generic.hpp \
    ../../include/elysian_shader_manager.hpp \
    ../../../AssetIO/include/elysian_light_source.hpp \
    ../../include/elysian_light_system.hpp

macx {
  OBJECTIVE_HEADERS += ../../../libGyro/lib/osx/SDLMain.h
}

SOURCES += \
    ../../source/main.cpp \
    ../../source/elysian_warp_system.cpp \
    ../../source/elysian_timer_system.cpp \
    ../../source/elysian_system.cpp \
    ../../source/elysian_quad_attrib.cpp \
    ../../source/elysian_party_system.cpp \
    ../../source/elysian_map_system.cpp \
    ../../source/elysian_lua_wrappers.cpp \
    ../../source/elysian_lua_singleton.cpp \
    ../../source/elysian_lua_script.cpp \
    ../../source/elysian_lua_manager.cpp \
    ../../source/elysian_level_system.cpp \
    ../../source/elysian_item_system.cpp \
    ../../source/elysian_gui_system.cpp \
    ../../source/elysian_game_input.cpp \
    ../../source/elysian_friendly_system.cpp \
    ../../source/elysian_font.cpp \
    ../../source/elysian_engine.cpp \
    ../../source/elysian_enemy_system.cpp \
    ../../source/elysian_dialog_system.cpp \
    ../../source/elysian_debug.cpp \
    ../../source/elysian_collision_system.cpp \
    ../../source/elysian_character_system.cpp \
    ../../source/elysian_camera_system.cpp \
    ../../source/elysian_behavior_system.cpp \
    ../../source/elysian_animation_system.cpp \
    ../../source/elysian_adventure_log.cpp \
    ../../../AssetIO/source/elysian_zcoords.cpp \
    ../../../AssetIO/source/elysian_warp.cpp \
    ../../../AssetIO/source/elysian_tile.cpp \
    ../../../AssetIO/source/elysian_renderable.cpp \
    ../../../AssetIO/source/elysian_lua_behavior.cpp \
    ../../../AssetIO/source/elysian_level_manager.cpp \
    ../../../AssetIO/source/elysian_level.cpp \
    ../../../AssetIO/source/elysian_item.cpp \
    ../../../AssetIO/source/elysian_friendly.cpp \
    ../../../AssetIO/source/elysian_examinable.cpp \
    ../../../AssetIO/source/elysian_entity.cpp \
    ../../../AssetIO/source/elysian_enemy.cpp \
    ../../../AssetIO/source/elysian_dialog.cpp \
    ../../../AssetIO/source/elysian_component.cpp \
    ../../../AssetIO/source/elysian_collidable.cpp \
    ../../../AssetIO/source/elysian_character.cpp \
    ../../../AssetIO/source/elysian_behavior.cpp \
    ../../../AssetIO/source/elysian_asset.cpp \
    ../../../AssetIO/source/elysian_area.cpp \
    ../../../AssetIO/source/elysian_animation.cpp \
    ../../../AssetIO/source/elysian_audio_source.cpp \
    ../../source/elysian_audio_system.cpp \
    ../../source/elysian_shader_manager.cpp \
    ../../../AssetIO/source/elysian_new.cpp \
    ../../../AssetIO/source/elysian_light_source.cpp \
    ../../source/elysian_light_system.cpp

macx {
  OBJECTIVE_SOURCES += ../../../libGyro/lib/osx/SDLMain.mm
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../libGyro/qtcreator/libGyro/release/ -llibGyro
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../libGyro/qtcreator/libGyro/debug/ -llibGyro
else:unix: LIBS += -L$$OUT_PWD/../../../libGyro/qtcreator/libGyro/ -llibGyro

INCLUDEPATH += $$PWD/../../../libGyro/qtcreator/libGyro
DEPENDPATH += $$PWD/../../../libGyro/qtcreator/libGyro

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../libGyro/qtcreator/libGyro/release/liblibGyro.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../libGyro/qtcreator/libGyro/debug/liblibGyro.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../libGyro/qtcreator/libGyro/liblibGyro.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../OOLua/qtcreator/OOLua/release/ -lOOLua
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../OOLua/qtcreator/OOLua/debug/ -lOOLua
else:unix: LIBS += -L$$OUT_PWD/../../../OOLua/qtcreator/OOLua/ -lOOLua

INCLUDEPATH += $$PWD/../../../OOLua/qtcreator/OOLua
DEPENDPATH += $$PWD/../../../OOLua/qtcreator/OOLua

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../OOLua/qtcreator/OOLua/release/libOOLua.a
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../OOLua/qtcreator/OOLua/debug/libOOLua.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../OOLua/qtcreator/OOLua/libOOLua.a


win32 {
    LIBS += -L../../lib/win32 \
        -llua52 \
        -L../../../libGyro/lib/win32 \
        -lopengl32 \
        -lopenal32.dll \
        -lglu32 \
        -lz \
        -lz.dll \
        -lmingw32 \
        -lSDLmain \
        -lSDL.dll \
        -logg \
        -lvorbis \
        #-lvorbis.dll \
        -lvorbisenc \
        #-lvorbisenc.dll \
        -lvorbisfile \
		-lglew32 \
		-lglew32.dll \
        #-lvorbisfile.dll \
        -mconsole
}

macx {
  LIBS +=  -L../../lib/osx \
    -framework OpenGL \
    -framework OpenAL \
    -lz \
    -F../../../libGyro/lib/osx \
    -L../../../libGyro/lib/osx \
    -llua52 \
    -framework Foundation \
    -framework Cocoa \
    -lobjc \
    -framework SDL \
    -logg \
    -lvorbis \
    -lvorbisenc \
    -lvorbisfile
}

linux {
    LIBS += \
        -llua5.2 \
        -lGL \
		-lopenal \
        -lGLU \
        -lz \
        -lSDLmain \
        -lSDL \
        -logg \
        -lvorbis \
        -lvorbisenc \
        -lvorbisfile \
        -lX11 \
        -lpthread
}

QMAKE_POST_LINK = cp -rf ../../../libGyro/source/glal/shaders/* ../../../Project/shaders

macx {
	CONFIG(release, debug|release) {
		DESTDIR = ../../../Project
		QMAKE_POST_LINK = "./ESGammaAppDereest.command"
	}
}

win32 {
    CONFIG(release, debug|release) {
        DESTDIR = ../../../Project
    }

}

linux {
    CONFIG(release, debug|release) {
        DESTDIR = ../../../Project
    }

}

