# -------------------------------------------------
# Project created by QtCreator 2010-03-06T14:23:27
# And modified to all fuck by Falco ALL THE DAYS
# -------------------------------------------------
include(../../qtcreator/ESDev/ESDev.pri)

TARGET = ESToolkit

TEMPLATE = app

dreamcast {
    TARGET_EXT=".elf"
}

!ios: !dreamcast: !android {
    QT += sql gui opengl network help
    CONFIG += console
    tests {
        CONFIG += testcase
        QT += testlib
    }
} else {
    QT -= core gui widgets
#    deployment.files = ../../assets
#    deployment.path =
#    QMAKE_BUNDLE_DATA += deployment
}

ios {
#    QMAKE_XCODE_CODE_SIGN_IDENTITY="Elysian Shadows"
    QMAKE_INFO_PLIST=ESToolkit_Info.plist
}


!ios {
    QMAKE_OBJCXXFLAGS_PRECOMPILE += -std=c++11 -stdlib=libc++
    CONFIG += precompile_header
}
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs -Wno-empty-body


ICON = resource/images/estk.icns

DEPENDPATH += 	../../AssetIO/include \
    ../../AssetIO/include/component \
    ../../AssetIO/include/crtp \
    ../../AssetIO/include/level \
    ../../AssetIO/include/primitive \
    ../../AssetIO/include/utility \
    ../../ESGamma/include \
    ../../ESGamma/include/crtp \
    ../../ESGamma/include/gui \
    ../../ESGamma/include/lua \
    ../../ESGamma/include/system \
    ../../ESGamma/include/utility \
    ../../libGyro/include \
    ../../libGyro/include/kinetic

!ios: !dreamcast: !android{
    DEPENDPATH += include \
        include/asset \
        include/model \
        include/estk \
        include/tile_engine \
        include/core \
        include/sheet_manager \
        include/dialog \
        include/widget
}

#PRIORITIZED TO REDUCE BUILD TIME ON FUCKING MINGW
INCLUDEPATH += ../../AssetIO/include \
    ../../ESGamma/include \
    ../../ESGamma/include/crtp \
    ../../ESGamma/include/gui \
    ../../ESGamma/include/lua \
    ../../ESGamma/include/system \
    ../../ESGamma/include/utility \
    ../../libGyro/include \
    ../../libGyro/include/kinetic \
    ../../Lua5.2/include \
    ../../OOLua/include \
    ../../ESGamma/lib/include \
    ../../Spine/include

!ios: !dreamcast: !android {
    INCLUDEPATH += include \
    include/asset \
    include/model \
    include/estk \
    include/tile_engine \
    include/core \
    include/sheet_manager \
    include/dialog \
    include/widget

    PRECOMPILED_HEADER = include/tk_precompiled_headers.h
}

SOURCES += ../../AssetIO/source/component/elysian_animation_base.cpp \
    ../../AssetIO/source/component/elysian_audio_source_base.cpp \
    ../../AssetIO/source/component/elysian_behavior_base.cpp \
    ../../AssetIO/source/component/elysian_character_base.cpp \
    ../../AssetIO/source/component/elysian_collidable_base.cpp \
    ../../AssetIO/source/component/elysian_combat_base.cpp \
    ../../AssetIO/source/component/elysian_examinable_base.cpp \
    ../../AssetIO/source/component/elysian_light_source_base.cpp \
    ../../AssetIO/source/component/elysian_particle_emitter_base.cpp \
    ../../AssetIO/source/component/elysian_renderable_base.cpp \
    ../../AssetIO/source/component/elysian_rigid_body_base.cpp \
    ../../AssetIO/source/component/elysian_skeleton_base.cpp \
    ../../AssetIO/source/component/elysian_warp_base.cpp \
    ../../AssetIO/source/level/elysian_area_base.cpp \
    ../../AssetIO/source/level/elysian_entity_base.cpp \
    ../../AssetIO/source/level/elysian_level_base.cpp \
    ../../AssetIO/source/level/elysian_level_manager.cpp \
    ../../AssetIO/source/level/elysian_tile.cpp \
    ../../AssetIO/source/level/elysian_zcoords.cpp \
    ../../AssetIO/source/primitive/elysian_color.cpp \
    ../../AssetIO/source/primitive/elysian_instance_transform.cpp \
    ../../AssetIO/source/primitive/elysian_material.cpp \
    ../../AssetIO/source/primitive/elysian_quad_sprite_attrib.cpp \
    ../../AssetIO/source/primitive/elysian_sprite_attrib.cpp \
    ../../AssetIO/source/utility/elysian_lua_table_proxy_base.cpp \
    ../../AssetIO/source/utility/luatables.cpp \
    ../../ESGamma/source/gui/elysian_adventure_log.cpp \
    ../../ESGamma/source/gui/elysian_font.cpp \
    ../../ESGamma/source/lua/elysian_lua_manager_base.cpp \
    ../../ESGamma/source/lua/elysian_lua_script.cpp \
    ../../ESGamma/source/lua/elysian_lua_singleton.cpp \
    ../../ESGamma/source/lua/elysian_lua_wrappers.cpp \
    ../../ESGamma/source/system/elysian_animation_system.cpp \
    ../../ESGamma/source/system/elysian_audio_system.cpp \
    ../../ESGamma/source/system/elysian_behavior_system.cpp \
    ../../ESGamma/source/system/elysian_character_system.cpp \
    ../../ESGamma/source/system/elysian_collision_system.cpp \
    ../../ESGamma/source/system/elysian_combat_system.cpp \
    ../../ESGamma/source/system/elysian_day_system.cpp \
    ../../ESGamma/source/system/elysian_dialog_system.cpp \
    ../../ESGamma/source/system/elysian_engine.cpp \
    ../../ESGamma/source/system/elysian_examinable_system.cpp \
    ../../ESGamma/source/system/elysian_gui_system.cpp \
    ../../ESGamma/source/system/elysian_level_system.cpp \
    ../../ESGamma/source/system/elysian_light_system.cpp \
    ../../ESGamma/source/system/elysian_map_system_base.cpp \
    ../../ESGamma/source/system/elysian_particle_system.cpp \
    ../../ESGamma/source/system/elysian_party_system.cpp \
    ../../ESGamma/source/system/elysian_rigid_body_system.cpp \
    ../../ESGamma/source/system/elysian_skeleton_system.cpp \
    ../../ESGamma/source/system/elysian_system.cpp \
    ../../ESGamma/source/system/elysian_warp_system.cpp \
    ../../ESGamma/source/utility/elysian_debug_base.cpp \
    ../../ESGamma/source/utility/elysian_event_monitor.cpp \
    ../../ESGamma/source/utility/elysian_force_generator.cpp \
    ../../ESGamma/source/utility/elysian_game_input.cpp \
    ../../ESGamma/source/utility/elysian_sort_sweep.cpp \
    ../../ESGamma/source/utility/elysian_task_monitor_base.cpp \
    ../../ESGamma/source/system/elysian_timer_system_base.cpp \
    ../../AssetIO/source/proxy/elysian_animation_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_area_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_audio_source_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_character_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_collidable_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_combat_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_component_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_entity_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_examinable_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_instance_transform_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_level_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_light_source_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_lua_behavior_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_particle_emitter_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_primitive_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_renderable_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_rigid_body_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_skeleton_proxy.cpp \
    ../../AssetIO/source/proxy/elysian_warp_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_animation_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_audio_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_camera_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_character_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_combat_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_day_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_dialog_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_event_monitor_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_game_input_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_gui_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_level_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_light_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_particle_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_party_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_rigid_body_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_timer_system_proxy.cpp \
    ../../AssetIO/source/component/elysian_lua_behavior_base.cpp \
    ../../ESGamma/source/system/elysian_camera_system_base.cpp \
    ../../ESGamma/source/utility/elysian_camera_shake_subprocess.cpp \
    ../../AssetIO/source/proxy/elysian_bit_mask_proxy.cpp \
    ../../AssetIO/source/utility/elysian_bit_mask.cpp \
    ../../ESGamma/source/utility/elysian_lua_table_ref.cpp \
    ../../ESGamma/source/utility/elysian_lua_variant.cpp \
    ../../AssetIO/source/proxy/elysian_dialog_proxy.cpp \
    ../../AssetIO/source/level/elysian_dialog_base.cpp \
    ../../AssetIO/source/level/elysian_dialog_screen_base.cpp \
    ../../ESGamma/source/system/elysian_input_system.cpp \
    ../../ESGamma/source/proxy/elysian_input_system_proxy.cpp




!ios: !dreamcast: !android {

    SOURCES += source/asset/tk_animation.cpp \
        source/asset/tk_area.cpp \
        source/asset/tk_asset_manager.cpp \
        source/asset/tk_assets.cpp \
        source/asset/tk_collidable.cpp \
        source/asset/tk_combat.cpp \
        source/asset/tk_elysian_tile_wrapper.cpp \
        source/asset/tk_entity.cpp \
        source/asset/tk_level.cpp \
        source/asset/tk_light_source.cpp \
        source/asset/tk_lua_table_proxy.cpp \
        source/asset/tk_map_entry.cpp \
        source/asset/tk_particle_emitter.cpp \
        source/asset/tk_project.cpp \
        source/asset/tk_renderable.cpp \
        source/core/main.cpp \
        source/core/tk_debug.cpp \
        source/core/tk_file_utilities.cpp \
        source/core/tk_menu_stack.cpp \
        source/core/tk_selectable.cpp \
        source/core/tk_tile_sheet_utilities.cpp \
        source/core/tk_tree_node.cpp \
        source/core/tk_undo_item.cpp \
        source/dialog/tk_engine_dialog.cpp \
        source/dialog/tk_engine_settings_dialog.cpp \
        source/dialog/tk_lua_explorer_dialog.cpp \
        source/estk/tk_asset_dock_widget.cpp \
        source/estk/tk_asset_tree_widget.cpp \
        source/estk/tk_engine_gl_scene_widget.cpp \
        source/estk/tk_entity_dock_widget.cpp \
        source/estk/tk_entity_selectable.cpp \
        source/estk/tk_entity_tree_widget.cpp \
        source/estk/tk_estoolkit.cpp \
        source/estk/tk_layer_dock_widget.cpp \
        source/estk/tk_lua_manager.cpp \
        source/estk/tk_lua_wrappers.cpp \
        source/estk/tk_map_system.cpp \
        source/estk/tk_map_view.cpp \
        source/estk/tk_map_view_tile_graphics_item.cpp \
        source/estk/tk_selection_dock_widget.cpp \
        source/estk/tk_sheet_dock_widget.cpp \
        source/estk/tk_sheet_view.cpp \
        source/estk/tk_sheet_view_tile_graphics_item.cpp \
        source/estk/tk_tile_selectable.cpp \
        source/estk/tk_tile_selectable_selection_view_delegate.cpp \
        source/model/tk_lua_table_stack_model.cpp \
        source/model/tk_property_stack_model.cpp \
        source/model/tk_rows_joiner_proxy_model.cpp \
        source/sheet_manager/tk_sheet_manager.cpp \
        source/sheet_manager/tk_sheet_manager_asset_manager.cpp \
        source/sheet_manager/tk_sheet_manager_dest_tile_graphics_item.cpp \
        source/sheet_manager/tk_sheet_manager_dest_view.cpp \
        source/sheet_manager/tk_sheet_manager_file_dock_widget.cpp \
        source/sheet_manager/tk_sheet_manager_source_tile_graphics_Item.cpp \
        source/sheet_manager/tk_sheet_manager_source_view.cpp \
        source/tile_engine/tk_source_view.cpp \
        source/widget/tk_history_dock_widget.cpp \
        source/widget/tk_documentation_browser.cpp \
        source/model/tk_task_monitor.cpp \
        source/widget/tk_pie_chart_view.cpp \
        source/dialog/tk_task_monitor_dialog.cpp \
        source/core/tk_network_access_manager.cpp \
        source/asset/tk_lua_behavior.cpp \
        source/estk/tk_timer_system.cpp \
        source/core/tk_gitlab_manager.cpp \
        source/dialog/tk_gitlab_dialog.cpp \
        source/asset/tk_skeleton.cpp \
        source/asset/tk_rigid_body.cpp \
        source/asset/tk_character.cpp \
        source/asset/tk_warp.cpp \
        source/asset/tk_sprite_attrib.cpp \
        source/model/tk_property_item_delegate.cpp \
        source/estk/tk_camera_dock_widget.cpp \
        source/asset/tk_examinable.cpp \
        source/estk/tk_camera_system.cpp \
        source/asset/tk_dialog.cpp \
        source/core/tk_lua_qt_utils.cpp

} else {
        SOURCES += ../../ESGamma/source/system/main.cpp
}

HEADERS += ../../AssetIO/include/component/elysian_animation_base.hpp \
    ../../AssetIO/include/component/elysian_audio_source_base.hpp \
    ../../AssetIO/include/component/elysian_behavior_base.hpp \
    ../../AssetIO/include/component/elysian_character_base.hpp \
    ../../AssetIO/include/component/elysian_collidable_base.hpp \
    ../../AssetIO/include/component/elysian_combat_base.hpp \
    ../../AssetIO/include/component/elysian_component_base.hpp \
    ../../AssetIO/include/component/elysian_examinable_base.hpp \
    ../../AssetIO/include/component/elysian_light_source_base.hpp \
    ../../AssetIO/include/component/elysian_particle_emitter_base.hpp \
    ../../AssetIO/include/component/elysian_renderable_base.hpp \
    ../../AssetIO/include/component/elysian_rigid_body_base.hpp \
    ../../AssetIO/include/component/elysian_warp_base.hpp \
    ../../AssetIO/include/crtp/elysian_animation.hpp \
    ../../AssetIO/include/crtp/elysian_area.hpp \
    ../../AssetIO/include/crtp/elysian_collidable.hpp \
    ../../AssetIO/include/crtp/elysian_component.hpp \
    ../../AssetIO/include/crtp/elysian_entity.hpp \
    ../../AssetIO/include/crtp/elysian_level.hpp \
    ../../AssetIO/include/crtp/elysian_lua_table_proxy.hpp \
    ../../AssetIO/include/crtp/elysian_renderable.hpp \
    ../../AssetIO/include/level/elysian_area_base.hpp \
    ../../AssetIO/include/level/elysian_direction.hpp \
    ../../AssetIO/include/level/elysian_entity_base.hpp \
    ../../AssetIO/include/level/elysian_level_base.hpp \
    ../../AssetIO/include/level/elysian_level_manager.hpp \
    ../../AssetIO/include/level/elysian_map_depth_buffer.hpp \
    ../../AssetIO/include/level/elysian_tile.hpp \
    ../../AssetIO/include/level/elysian_zcoords.hpp \
    ../../AssetIO/include/primitive/elysian_color.hpp \
    ../../AssetIO/include/primitive/elysian_instance_transform.hpp \
    ../../AssetIO/include/primitive/elysian_material.hpp \
    ../../AssetIO/include/primitive/elysian_model_attrib.hpp \
    ../../AssetIO/include/primitive/elysian_prim_attrib.hpp \
    ../../AssetIO/include/primitive/elysian_quad_sprite_attrib.hpp \
    ../../AssetIO/include/primitive/elysian_renderable_object.hpp \
    ../../AssetIO/include/primitive/elysian_textured_attrib.hpp \
    ../../AssetIO/include/utility/elysian_asset_typedefs.hpp \
    ../../AssetIO/include/utility/elysian_cstr_map.hpp \
    ../../AssetIO/include/utility/elysian_generic.hpp \
    ../../AssetIO/include/utility/elysian_lua_table_proxy_base.hpp \
    ../../AssetIO/include/utility/elysian_mem_pool.hpp \
    ../../AssetIO/include/utility/elysian_stl_algorithm.hpp \
    ../../AssetIO/include/utility/luatables.h \
    ../../ESGamma/include/crtp/elysian_debug.hpp \
    ../../ESGamma/include/crtp/elysian_lua_manager.hpp \
    ../../ESGamma/include/crtp/elysian_map_system.hpp \
    ../../ESGamma/include/gui/elysian_adventure_log.hpp \
    ../../ESGamma/include/gui/elysian_font.hpp \
    ../../ESGamma/include/lua/elysian_lua_manager_base.hpp \
    ../../ESGamma/include/lua/elysian_lua_script.hpp \
    ../../ESGamma/include/lua/elysian_lua_singleton.hpp \
    ../../ESGamma/include/lua/elysian_lua_wrappers.hpp \
    ../../ESGamma/include/system/elysian_animation_system.hpp \
    ../../ESGamma/include/system/elysian_audio_system.hpp \
    ../../ESGamma/include/system/elysian_behavior_system.hpp \
    ../../ESGamma/include/system/elysian_character_system.hpp \
    ../../ESGamma/include/system/elysian_collision_system.hpp \
    ../../ESGamma/include/system/elysian_combat_system.hpp \
    ../../ESGamma/include/system/elysian_day_system.hpp \
    ../../ESGamma/include/system/elysian_dialog_system.hpp \
    ../../ESGamma/include/system/elysian_engine.hpp \
    ../../ESGamma/include/system/elysian_examinable_system.hpp \
    ../../ESGamma/include/system/elysian_gui_system.hpp \
    ../../ESGamma/include/system/elysian_level_system.hpp \
    ../../ESGamma/include/system/elysian_light_system.hpp \
    ../../ESGamma/include/system/elysian_map_system_base.hpp \
    ../../ESGamma/include/system/elysian_particle_system.hpp \
    ../../ESGamma/include/system/elysian_party_system.hpp \
    ../../ESGamma/include/system/elysian_rigid_body_system.hpp \
    ../../ESGamma/include/system/elysian_skeleton_system.hpp \
    ../../ESGamma/include/system/elysian_system.hpp \
    ../../ESGamma/include/system/elysian_warp_system.hpp \
    ../../ESGamma/include/utility/elysian_debug_base.hpp \
    ../../ESGamma/include/utility/elysian_event_monitor.hpp \
    ../../ESGamma/include/utility/elysian_force_generator.hpp \
    ../../ESGamma/include/utility/elysian_game_input.hpp \
    ../../ESGamma/include/utility/elysian_sort_sweep.hpp \
    ../../ESGamma/include/utility/elysian_subprocess_manager.hpp \
    ../../AssetIO/include/crtp/elysian_behavior.hpp \
    ../../AssetIO/include/crtp/elysian_particle_emitter.hpp \
    ../../AssetIO/include/crtp/elysian_character.hpp \
    ../../AssetIO/include/crtp/elysian_rigid_body.hpp \
    ../../AssetIO/include/crtp/elysian_audio_source.hpp \
    ../../AssetIO/include/crtp/elysian_combat.hpp \
    ../../AssetIO/include/crtp/elysian_light_source.hpp \
    ../../AssetIO/include/crtp/elysian_examinable.hpp \
    ../../AssetIO/include/component/elysian_skeleton_base.hpp \
    ../../AssetIO/include/crtp/elysian_skeleton.hpp \
    ../../AssetIO/include/crtp/elysian_warp.hpp \
    ../../ESGamma/include/gui/elysian_sprite.hpp \
    ../../ESGamma/include/gui/elysian_quad_sprite.hpp \
    ../../ESGamma/include/utility/elysian_task_monitor_base.hpp \
    ../../ESGamma/include/crtp/elysian_task_monitor.hpp \
    ../../ESGamma/include/system/elysian_timer_system_base.hpp \
    ../../ESGamma/include/crtp/elysian_timer_system.hpp \
    ../../AssetIO/include/proxy/elysian_animation_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_area_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_audio_source_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_character_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_collidable_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_combat_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_component_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_entity_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_examinable_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_instance_transform_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_level_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_light_source_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_lua_behavior_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_particle_emitter_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_primitive_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_renderable_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_rigid_body_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_skeleton_proxy.hpp \
    ../../AssetIO/include/proxy/elysian_warp_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_animation_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_audio_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_camera_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_character_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_combat_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_day_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_dialog_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_event_monitor_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_game_input_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_gui_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_level_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_light_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_particle_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_party_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_timer_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_rigid_body_system_proxy.hpp \
    ../../AssetIO/include/component/elysian_lua_behavior_base.hpp \
    ../../AssetIO/include/crtp/elysian_lua_behavior.hpp \
    ../../AssetIO/include/level/elysian_depth_buff_entry_base.hpp \
    ../../AssetIO/include/crtp/elysian_depth_buff_entry.hpp \
    ../../ESGamma/include/system/elysian_camera_system_base.hpp \
    ../../ESGamma/include/crtp/elysian_camera_system.hpp \
    ../../ESGamma/include/utility/elysian_camera_shake_subprocess.hpp \
    ../../ESGamma/include/utility/elysian_bit_mask.hpp \
    ../../AssetIO/include/proxy/elysian_bit_mask_proxy.hpp \
    ../../AssetIO/include/crtp/elysian_sprite_attrib.hpp \
    ../../AssetIO/include/primitive/elysian_sprite_attrib_base.hpp \
    ../../AssetIO/include/utility/elysian_lua_table_ref.hpp \
    ../../AssetIO/include/proxy/elysian_dialog_proxy.hpp \
    ../../AssetIO/include/level/elysian_dialog_base.hpp \
    ../../AssetIO/include/crtp/elysian_dialog.hpp \
    ../../AssetIO/include/crtp/elysian_dialog_screen.hpp \
    ../../AssetIO/include/level/elysian_dialog_screen_base.hpp \
    ../../AssetIO/include/crtp/elysian_lua_variant.hpp \
    ../../AssetIO/include/utility/elysian_lua_variant_base.hpp \
    ../../ESGamma/include/system/elysian_input_system.hpp \
    ../../ESGamma/include/proxy/elysian_input_system_proxy.hpp

!ios:!dreamcast: !android {
    HEADERS +=  include/asset/tk_animation.h \
        include/asset/tk_area.h \
        include/asset/tk_asset_manager.h \
        include/asset/tk_assets.h \
        include/asset/tk_audio_source.h \
        include/asset/tk_behavior.h \
        include/asset/tk_character.h \
        include/asset/tk_collidable.h \
        include/asset/tk_combat.h \
        include/asset/tk_component.h \
        include/asset/tk_elysian_tile_wrapper.h \
        include/asset/tk_entity.h \
        include/asset/tk_examinable.h \
        include/asset/tk_level.h \
        include/asset/tk_light_source.h \
        include/asset/tk_lua_table_proxy.h \
        include/asset/tk_map_entry.h \
        include/asset/tk_particle_emitter.h \
        include/asset/tk_project.h \
        include/asset/tk_renderable.h \
        include/asset/tk_rigid_body.h \
        include/asset/tk_skeleton.h \
        include/asset/tk_warp.h \
        include/core/tk_checkered_alpha_rect_graphics_item.h \
        include/core/tk_debug.h \
        include/core/tk_file_utilities.h \
        include/core/tk_gl_utilities.h \
        include/core/tk_menu_stack.h \
        include/core/tk_selectable.h \
        include/core/tk_selection_scene.h \
        include/core/tk_tile_image.h \
        include/core/tk_tile_sheet_utilities.h \
        include/core/tk_tree_node.h \
        include/core/tk_undo_item.h \
        include/dialog/tk_engine_dialog.h \
        include/dialog/tk_engine_settings_dialog.h \
        include/dialog/tk_lua_explorer_dialog.h \
        include/estk/tk_asset_dock_widget.h \
        include/estk/tk_asset_tree_widget.h \
        include/estk/tk_engine_gl_scene_widget.h \
        include/estk/tk_entity_dock_widget.h \
        include/estk/tk_entity_selectable.h \
        include/estk/tk_entity_tree_widget.h \
        include/estk/tk_estoolkit.h \
        include/estk/tk_layer_dock_widget.h \
        include/estk/tk_lua_manager.h \
        include/estk/tk_lua_wrappers.h \
        include/estk/tk_map_system.h \
        include/estk/tk_map_view.h \
        include/estk/tk_map_view_depth_buffer_graphics_item.h \
        include/estk/tk_map_view_place_tile_group_undo_item.h \
        include/estk/tk_map_view_tile_graphics_item.h \
        include/estk/tk_selection_dock_widget.h \
        include/estk/tk_sheet_dock_widget.h \
        include/estk/tk_sheet_view.h \
        include/estk/tk_sheet_view_collider_graphics_item.h \
        include/estk/tk_sheet_view_tile_graphics_item.h \
        include/estk/tk_tile_selectable.h \
        include/estk/tk_tile_selectable_selection_view_delegate.h \
        include/model/tk_lua_table_stack_model.h \
        include/model/tk_property_stack_model.h \
        include/model/tk_property_standard_item.h \
        include/model/tk_rows_joiner_proxy_model.h \
        include/sheet_manager/tk_sheet_manager.h \
        include/sheet_manager/tk_sheet_manager_asset_manager.h \
        include/sheet_manager/tk_sheet_manager_assets.h \
        include/sheet_manager/tk_sheet_manager_dest_tile_graphics_item.h \
        include/sheet_manager/tk_sheet_manager_dest_view.h \
        include/sheet_manager/tk_sheet_manager_file_dock_widget.h \
        include/sheet_manager/tk_sheet_manager_place_tile_group_undo_item.h \
        include/sheet_manager/tk_sheet_manager_source_tile_graphics_item.h \
        include/sheet_manager/tk_sheet_manager_source_view.h \
        include/sheet_manager/tk_sheet_manager_tile_entry.h \
        include/sheet_manager/tk_sheet_manager_tile_selection.h \
        include/tile_engine/tk_dest_view.h \
        include/tile_engine/tk_selection_item_group.h \
        include/tile_engine/tk_source_view.h \
        include/tile_engine/tk_tile_graphics_item.h \
        include/tile_engine/tk_tile_group_undo_item.h \
        include/tile_engine/tk_tile_selection.h \
        include/widget/tk_history_dock_widget.h \
        include/tile_engine/tk_tile_graphics_item_impl.h \
        include/tile_engine/tk_dest_view_impl.h \
        include/model/tk_property_item_delegate.h \
        include/core/tk_lua_qt_utils.h \
        include/widget/tk_documentation_browser.h \
        include/model/tk_task_monitor.h \
        include/widget/tk_pie_chart_view.h \
        include/dialog/tk_task_monitor_dialog.h \
        include/estk/tk_timer_system.h \
        include/core/tk_gitlab_manager.h \
        include/dialog/tk_gitlab_dialog.h \
        include/tk_precompiled_headers.h \
        include/core/tk_network_access_manager.h \
        include/asset/tk_lua_behavior.h \
        include/asset/tk_depth_buff_entry.h \
        include/estk/tk_camera_dock_widget.h \
        include/estk/tk_camera_system.h \
        include/asset/tk_sprite_attrib.h \
        include/asset/tk_dialog_screen.h \
        include/asset/tk_dialog.h \
        include/asset/tk_lua_variant.h
}


FORMS += form/estk/tk_sheet_dock_widget.ui \
    form/estk/tk_selection_dock_widget.ui \
    form/estk/tk_layer_dock_widget.ui \
    form/estk/tk_estoolkit.ui \
    form/estk/tk_entity_dock_widget.ui \
    form/estk/tk_asset_dock_widget.ui \
    form/sheet_manager/tk_sheet_manager.ui \
    form/widget/tk_history_dock_widget.ui \
    form/dialog/tk_engine_settings.ui \
    form/sheet_manager/tk_sheet_manager_file_dock_widget.ui \
    form/dialog/tk_lua_explorer.ui \
    form/dialog/tk_gitlab_dialog.ui \
    form/estk/tk_camera_dock_widget.ui

#docs.target = ESToolkit.qhc
#docs.commands = qdoc ./ESToolkit.qdocconf --log-progress --debug $$escape_expand(\n\t) qcollectiongenerator ./ESToolkit.qhcp -o ../../assets/doc/ESToolkit.qhc

#QMAKE_EXTRA_TARGETS += docs

#PRE_TARGETDEPS += ESToolkit.qhc


#Create windows manifest requesting ESTk to be run as admin (to run dc-tool as admin)
#win32 {
#    WINSDK_DIR = C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A
#    WIN_PWD = $$replace(PWD, /, \\)
#    OUT_PWD_WIN = $$replace(OUT_PWD, /, \\)
#    QMAKE_POST_LINK = "$$WINSDK_DIR/bin/x64/mt.exe -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest) -outputresource:$$quote($$OUT_PWD_WIN\\${DESTDIR_TARGET};1)"
#}

#Required to get C++11x to work on OSX
macx {
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
    LIBS += -mmacosx-version-min=10.7 -stdlib=libc++

    #CONFIG(release, debug|release) {
     #  QMAKE_POST_LINK = "macdeployqt TARGET"
    #}
}

opencl {
    LIBS += -framework OpenCL
    QMAKE_CXXFLAGS += -DELYSIAN_OPENCL_ACCELERATED

}

RESOURCES += resource/resource.qrc

docs {

    DEFINES+=TOOLKIT_DOCUMENTATION_RESOURCES

    RESOURCES += resource/docs.qrc

#	PHONY_DEPS=. FUCK THIS SHIT SO HARD!!!!
#		docs.depends = FORCE
#		docs.target = $$PWD/out/ESToolkit.qhc
#		docs.commands = ./generate_docs.sh
#		docs.config += target_predeps
#		QMAKE_EXTRA_TARGETS += docs
#		PRE_TARGETDEPS += $$PWD/out/ESToolkit.qhc
    #}

#system(./generate_docs.sh)

}

macx {
    #DESTDIR = ../../executable
#	CONFIG(release, debug|release):
    QMAKE_POST_LINK =   "rm -rf $${TARGET}.app/Contents/Frameworks/SDL2.framework" $$escape_expand(\n\t) "cp -R ../../libGyro/lib/osx/SDL2.framework $${TARGET}.app/Contents/Frameworks" $$escape_expand(\n\t) "macdeployqt $${TARGET}.app -verbose=3"
}

dreamcast {
    DESTDIR = ../../executable
    QMAKE_POST_LINK="mv ../../executable/ESToolkit ../../executable/ESToolkit.elf"
}

ios {
    QMAKE_CXXFLAGS += -stdlib=libc++
    iphoneos {
        DESTDIR = ../../executable/ios
        deployment.files = ../../assets
        deployment.path =
        QMAKE_BUNDLE_DATA += deployment
    }
    iphonesimulator {
        DESTDIR = ../../executable/ios_sim
        deployment.files = ../../assets
        deployment.path =
        QMAKE_BUNDLE_DATA += deployment
    }
}

win32 {
    DESTDIR = ../../executable
}

linux {
    DESTDIR = ../../executable
}



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libGyro/qtcreator/libGyro/release/ -llibGyro
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libGyro/qtcreator/libGyro/debug/ -llibGyro
else:unix: LIBS += -L$$OUT_PWD/../../libGyro/qtcreator/libGyro/ -llibGyro

INCLUDEPATH += $$PWD/../../libGyro/qtcreator/libGyro
DEPENDPATH += $$PWD/../../libGyro/qtcreator/libGyro

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/release/liblibGyro.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/debug/liblibGyro.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/release/libGyro.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/debug/libGyro.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../libGyro/qtcreator/libGyro/liblibGyro.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../OOLua/qtcreator/OOLua/release/ -lOOLua
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../OOLua/qtcreator/OOLua/debug/ -lOOLua
else:unix: LIBS += -L$$OUT_PWD/../../OOLua/qtcreator/OOLua/ -lOOLua

INCLUDEPATH += $$PWD/../../OOLua/qtcreator/OOLua
DEPENDPATH += $$PWD/../../OOLua/qtcreator/OOLua

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/release/libOOLua.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/debug/libOOLua.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/release/OOLua.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/debug/OOLua.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../OOLua/qtcreator/OOLua/libOOLua.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/release/ -lLua5_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/debug/ -lLua5_2
else:unix: LIBS += -L$$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/ -lLua5_2

INCLUDEPATH += $$PWD/../../Lua5.2/qtcreator/Lua5_2
DEPENDPATH += $$PWD/../../Lua5.2/qtcreator/Lua5_2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/release/libLua5_2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/debug/libLua5_2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/release/Lua5_2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/debug/Lua5_2.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Lua5.2/qtcreator/Lua5_2/libLua5_2.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Spine/qtcreator/Spine/release/ -lSpine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Spine/qtcreator/Spine/debug/ -lSpine
else:unix: LIBS += -L$$OUT_PWD/../../Spine/qtcreator/Spine/ -lSpine

INCLUDEPATH += $$PWD/../../Spine/qtcreator/Spine
DEPENDPATH += $$PWD/../../Spine/qtcreator/Spine

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/release/libSpine.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/debug/libSpine.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/release/Spine.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/debug/Spine.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Spine/qtcreator/Spine/libSpine.a

tests {

    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../UnitTests/release/ -lUnitTests
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../UnitTests/debug/ -lUnitTests
    else:unix: LIBS += -L$$OUT_PWD/../../UnitTests -lUnitTests

    INCLUDEPATH += $$PWD/../../UnitTests
    DEPENDPATH += $$PWD/../../UnitTests

    win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../UnitTests/release/libUnitTests.a
    else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../UnitTests/debug/libUnitTests.a
    else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../UnitTests/release/UnitTests.lib
    else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../UnitTests/debug/UnitTests.lib
    else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../UnitTests/libUnitTests.a

}

win32 {
    QMAKE_CFLAGS += -msse -std=c99
    INCLUDEPATH += ../../libGyro/lib/win32
    LIBS += -L../../lib/win32 \
        #-llua52 \
        -L../../libGyro/lib/win32 \
        -lDbgHelp \
        -lglew32 \
        #-lglew32.dll \
        -lglu32 \
        -lopengl32 \
        -lopenal32.dll \
        #-lDbgHelp.dll \
        -lz \
        -lz.dll \
        -lSDL2 \
        -lSDL2Main \
        -lSDL2.dll \
       # -lmingw32 \
        -logg \
        -lvorbis \
        -lvorbis.dll \
        -lvorbisenc \
        -lvorbisenc.dll \
        -lvorbisfile \
        -limagehlp \
        -lvorbisfile.dll
        #-mconsole
        #-llua52
}

macx {
  LIBS +=  -L../../libGyro/lib/osx \
        -framework OpenGL \
        -framework OpenAL \
        -lz \
        -F../../ESGamma/lib/osx \
        -L../../ESGamma/lib/osx \
        -F../../libGyro/lib/osx \
        -framework SDL2 \
        -framework Foundation \
        -framework Cocoa \
        -lobjc \
        -logg \
        -lvorbis \
        -lvorbisenc \
        -lvorbisfile \
        -L../../ESGamma/lib/osx
}

ios {
    LIBS += \
        -lstdc++ \
        -framework OpenGLES \
        -framework GLKit \
        -framework UIKit \
        -framework Foundation \
        -framework CoreGraphics \
        -framework CoreMotion \
        -framework OpenAL \
        -F../../libGyro/lib/ios \
        -framework Ogg \
        -framework Vorbis
}

dreamcast {

    LIBS+=-L/home/jenkins/Dreamcast/cadcdev-kallistios/lib/dreamcast \
        -L/home/jenkins/Dreamcast/cadcdev-kallistios/addons/lib/dreamcast \
        -lstdc++ \
        -lm \
        -Wl,--start-group -lkallisti -lc -lgcc -Wl,--end-group \
        -T/home/jenkins/Dreamcast/cadcdev-kallistios/utils/ldscripts/shlelf.xc \
        -nodefaultlibs

} else: android {
    LIBS += -lEGL -lGLESv2 -landroid

}
else:linux {
    LIBS += \
        -lGL \
        -lopenal \
        -lGLU \
        -lGLEW \
        -lz \
        -logg \
        -lvorbis \
        -lvorbisenc \
        -lvorbisfile \
        -lX11 \
        -lpthread \
        -lrt
}

DISTFILES += \
    ESToolkit.qdocconf \
    ESToolkit.qhcp
