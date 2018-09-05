# -------------------------------------------------
# Project created by QtCreator 2010-03-06T14:23:27
# And modified to all fuck by Falco ALL THE DAYS
# -------------------------------------------------
#include(../../qtcreator/ESDev/ESDev.pri)

TARGET = ESToolkit

DEFINES += ELYSIAN_OPENCL_ACCELERATED

TEMPLATE = app

dreamcast {
    TARGET_EXT=".elf"
}

vmu_child_app {
    DEFINES+=VMU_CHILD_APP

}

!ios: !dreamcast: !android {
    QT += sql gui opengl network help charts
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
    #QMAKE_INFO_PLIST=iOS_Info.plist
}

macx {
    QMAKE_INFO_PLIST=MacOS_Info.plist
}


!ios {
    #QMAKE_OBJCXXFLAGS_PRECOMPILE += -std=c++11 -stdlib=libc++
    #CONFIG += precompile_header
}
QMAKE_CXXFLAGS += -Wno-empty-body


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
    ../../game/include \
    ../../libGyro/include \
    ../../libGyro/api

!ios: !dreamcast: !android{
    DEPENDPATH += include
}

#PRIORITIZED TO REDUCE BUILD TIME ON FUCKING MINGW
INCLUDEPATH += ../../AssetIO/include \
    ../../ESGamma/include \
    ../../ESGamma/include/crtp \
    ../../ESGamma/include/gui \
    ../../ESGamma/include/lua \
    ../../ESGamma/include/system \
    ../../ESGamma/include/utility \
    ../../libGyro/api

!ios: !dreamcast: !android {
    INCLUDEPATH += include
    #PRECOMPILED_HEADER = include/tk_precompiled_headers.h
}

vmu_child_app {
    QT += serialport

    INCLUDEPATH += ../../ElysianVMU/app/qt/include \
                ../../ElysianVMU/evmu-core/include

    DEPENDPATH += ../../ElysianVMU/app/qt \
        ../../ElysianVMU/build/qtcreator/ElysianVMU
}

#SPLIT UP SO ITS STILL PRIORITIZED!!!
INCLUDEPATH += \
    ../../game/include \
    ../../libGyro/include \
    ../../libGyro/include/kinetic \
    ../../Lua5.2/include \
    ../../OOLua/include \
    ../../ESGamma/lib/include \
    ../../Spine/include

SOURCES += \
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
    ../../AssetIO/source/primitive/elysian_instance_transform.cpp \
    ../../AssetIO/source/primitive/elysian_material.cpp \
    ../../AssetIO/source/primitive/elysian_quad_sprite_attrib.cpp \
    ../../AssetIO/source/primitive/elysian_sprite_attrib.cpp \
    ../../AssetIO/source/utility/elysian_lua_table_proxy_base.cpp \
    ../../ESGamma/source/gui/elysian_adventure_log.cpp \
    ../../ESGamma/source/gui/elysian_font.cpp \
    ../../ESGamma/source/lua/elysian_lua_manager_base.cpp \
    ../../ESGamma/source/lua/elysian_lua_script.cpp \
    ../../ESGamma/source/lua/elysian_lua_singleton.cpp \
    ../../ESGamma/source/lua/elysian_lua_wrappers.cpp \
    ../../ESGamma/source/system/elysian_audio_system.cpp \
    ../../ESGamma/source/system/elysian_behavior_system.cpp \
    ../../ESGamma/source/system/elysian_character_system.cpp \
    ../../ESGamma/source/system/elysian_collision_system.cpp \
    ../../ESGamma/source/system/elysian_combat_system.cpp \
    ../../ESGamma/source/system/elysian_day_system.cpp \
    ../../ESGamma/source/system/elysian_dialog_system.cpp \
    ../../ESGamma/source/system/elysian_engine.cpp \
    ../../ESGamma/source/system/elysian_examinable_system.cpp \
    ../../ESGamma/source/system/elysian_light_system.cpp \
    ../../ESGamma/source/system/elysian_map_system_base.cpp \
    ../../ESGamma/source/system/elysian_party_system.cpp \
    ../../ESGamma/source/system/elysian_rigid_body_system.cpp \
    ../../ESGamma/source/system/elysian_skeleton_system.cpp \
    ../../ESGamma/source/system/elysian_system.cpp \
    ../../ESGamma/source/system/elysian_warp_system.cpp \
    ../../ESGamma/source/utility/elysian_debug_base.cpp \
    ../../ESGamma/source/utility/elysian_event_monitor.cpp \
    ../../ESGamma/source/utility/elysian_force_generator.cpp \
    ../../ESGamma/source/utility/elysian_sort_sweep.cpp \
    ../../ESGamma/source/utility/elysian_task_monitor_base.cpp \
    ../../ESGamma/source/system/elysian_timer_system_base.cpp \
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
    ../../ESGamma/source/proxy/elysian_audio_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_camera_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_character_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_combat_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_day_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_dialog_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_event_monitor_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_gui_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_level_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_light_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_particle_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_pathfinding_system_proxy.cpp \
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
    ../../ESGamma/source/proxy/elysian_input_system_proxy.cpp \
    ../../ESGamma/source/system/elysian_input_system_base.cpp \
    ../../game/source/behavior/elysian_playable_behavior.cpp \
    ../../ESGamma/source/system/elysian_pathfinding_system.cpp \
    ../../AssetIO/source/utility/elysian_lua_variant_base.cpp \
    ../../ESGamma/source/system/elysian_level_system_base.cpp \
    ../../game/source/behavior/elysian_state_machine_behavior.cpp \
    ../../ESGamma/source/system/elysian_gui_system_base.cpp \
    ../../ESGamma/source/gui/elysian_widget.cpp \
    ../../ESGamma/source/gui/elysian_layout.cpp \
    ../../ESGamma/source/gui/elysian_label_widget.cpp \
    ../../ESGamma/source/gui/elysian_frame_widget.cpp \
    ../../ESGamma/source/gui/elysian_entity_widget.cpp \
    ../../ESGamma/source/proxy/elysian_widget_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_layout_proxy.cpp \
    ../../ESGamma/source/gui/elysian_menu_widget.cpp \
    ../../ESGamma/source/system/elysian_event_system.cpp \
    ../../AssetIO/source/primitive/elysian_color_base.cpp \
    ../../ESGamma/source/system/elysian_particle_system_base.cpp \
    ../../AssetIO/source/level/elysian_event.cpp \
    ../../AssetIO/source/proxy/elysian_event_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_event_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_collision_system_proxy.cpp \
    ../../ESGamma/source/proxy/elysian_examinable_system_proxy.cpp \
    source/ESGamma/tk_input_system.cpp \
    ../../ESGamma/source/device/elysian_rumble_device.cpp \
    ../../game/source/pause_menu/es_options_menu.cpp \
    ../../game/source/pause_menu/es_pause_menu.cpp \
    ../../game/source/pause_menu/es_playable_pane.cpp \
    ../../game/source/pause_menu/es_party_menu.cpp \
    ../../game/source/pause_menu/es_inventory_menu.cpp \
    ../../ESGamma/source/system/elysian_compute_system.cpp \
    source/ESGamma/tk_render_system.cpp \
    ../../ESGamma/source/system/elysian_render_system_base.cpp \
    ../../ESGamma/source/gui/elysian_item_delegate.cpp \
    ../../ESGamma/source/system/elysian_thread_system.cpp \
    source/QESTkUtils/ui/widget/tk_pie_chart_widget.cpp \
    source/QESTkUtils/ui/widget/tk_line_chart.cpp


!ios: !dreamcast: !android {

    SOURCES += \
        source/AssetIO/tk_area.cpp \
        source/ESToolkit/core/tk_asset_manager.cpp \
        source/ESToolkit/core/tk_assets.cpp \
        source/AssetIO/tk_collidable.cpp \
        source/AssetIO/tk_combat.cpp \
        source/AssetIO/tk_elysian_tile_wrapper.cpp \
        source/AssetIO/tk_entity.cpp \
        source/AssetIO/tk_level.cpp \
        source/AssetIO/tk_light_source.cpp \
        source/AssetIO/tk_lua_table_proxy.cpp \
        source/AssetIO/tk_map_entry.cpp \
        source/AssetIO/tk_particle_emitter.cpp \
        source/ESToolkit/core/tk_project.cpp \
        source/AssetIO/tk_renderable.cpp \
        source/AssetIO/tk_skeleton.cpp \
        source/AssetIO/tk_rigid_body.cpp \
        source/AssetIO/tk_character.cpp \
        source/AssetIO/tk_warp.cpp \
        source/AssetIO/tk_sprite_attrib.cpp \
        source/AssetIO/tk_examinable.cpp \
        source/AssetIO/tk_lua_behavior.cpp \
        source/AssetIO/tk_dialog.cpp \
        source/AssetIO/tk_color.cpp \
        source/AssetIO/tk_dialog_screen.cpp \
        source/ESToolkit/ui/dock/tk_camera_dock_widget.cpp \
        source/ESToolkit/core/main.cpp \
        source/ESToolkit/core/tk_debug.cpp \
        source/ESToolkit/core/tk_file_utilities.cpp \
        source/QESTkUtils/tk_menu_stack.cpp \
        source/ESToolkit/core/tk_tile_sheet_utilities.cpp \
        source/ESToolkit/ui/item/tk_tree_node.cpp \
        source/ESToolkit/core/tk_undo_item.cpp \
        source/ESToolkit/ui/dialog/tk_engine_dialog.cpp \
        source/ESToolkit/ui/dialog/tk_engine_settings_dialog.cpp \
        source/ESToolkit/ui/dock/asset/tk_asset_dock_widget.cpp \
        source/ESToolkit/ui/widget/tk_engine_gl_scene_widget.cpp \
        source/ESToolkit/ui/dock/tk_entity_dock_widget.cpp \
        source/ESToolkit/core/tk_entity_selectable.cpp \
        source/ESToolkit/ui/widget/tk_entity_tree_widget.cpp \
        source/ESToolkit/ui/tk_estoolkit.cpp \
        source/ESToolkit/ui/dock/tk_layer_dock_widget.cpp \
        source/ESGamma/tk_lua_manager.cpp \
        source/ESGamma/tk_lua_wrappers.cpp \
        source/ESGamma/tk_map_system.cpp \
        source/ESToolkit/ui/widget/tk_map_view.cpp \
        source/ESToolkit/ui/item/tk_map_view_tile_graphics_item.cpp \
        source/ESToolkit/ui/dock/tk_selection_dock_widget.cpp \
        source/ESToolkit/ui/dock/tk_sheet_dock_widget.cpp \
        source/ESToolkit/ui/widget/tk_sheet_view.cpp \
        source/ESToolkit/ui/item/tk_sheet_view_tile_graphics_item.cpp \
        source/ESToolkit/core/tk_tile_selectable.cpp \
        source/ESToolkit/ui/delegate/tk_tile_selectable_selection_view_delegate.cpp \
        source/QESTkUtils/model/tk_lua_table_stack_model.cpp \
        source/QESTkUtils/model/tk_property_stack_model.cpp \
        source/QESTkUtils/model/tk_rows_joiner_proxy_model.cpp \
        source/ESToolkit/sheet_manager/tk_sheet_manager.cpp \
        source/ESToolkit/sheet_manager/tk_sheet_manager_asset_manager.cpp \
        source/ESToolkit/sheet_manager/tk_sheet_manager_dest_tile_graphics_item.cpp \
        source/ESToolkit/sheet_manager/tk_sheet_manager_dest_view.cpp \
        source/ESToolkit/sheet_manager/tk_sheet_manager_file_dock_widget.cpp \
        source/ESToolkit/sheet_manager/tk_sheet_manager_source_tile_graphics_Item.cpp \
        source/ESToolkit/sheet_manager/tk_sheet_manager_source_view.cpp \
        source/QESTkUtils/tile_engine/tk_source_view.cpp \
        source/ESToolkit/ui/dock/tk_history_dock_widget.cpp \
        source/QESTkUtils/ui/widget/tk_documentation_browser.cpp \
        source/ESToolkit/ui/dialog/tk_task_monitor_dialog.cpp \
        source/QESTkUtils/tk_network_access_manager.cpp \
        source/ESGamma/tk_timer_system.cpp \
        source/QESTkUtils/ui/widget/tk_gitlab_dialog.cpp \
        source/ESGamma/tk_camera_system.cpp \
        source/QESTkUtils/tk_lua_qt_utils.cpp \
        source/QESTkUtils/tk_mime_data.cpp \
        source/QESTkUtils/tk_mime_drop_manager.cpp \
        source/ESToolkit/ui/tk_estoolkit_menu_bar_slots.cpp \
        source/ESGamma/tk_level_system.cpp \
        source/QESTkUtils/ui/delegate/tk_property_item_delegate.cpp \
        source/QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.cpp \
        source/QESTkUtils/ui/delegate/tk_file_browser_property_item_delegate.cpp \
        source/QESTkUtils/ui/delegate/tk_spinbox_property_item_delegate.cpp \
        source/QESTkUtils/ui/delegate/tk_property_item_proxy_delegate.cpp \
        source/QESTkUtils/ui/delegate/tk_combobox_property_item_delegate.cpp \
        source/QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.cpp \
        source/ESToolkit/ui/widget/tk_selection_tree_view.cpp \
        source/QESTkUtils/tk_lua_mime_drop_manager.cpp \
        source/ESGamma/tk_particle_system.cpp \
        source/ESToolkit/ui/dock/tk_particle_attrib_preview_dock_widget.cpp \
        source/QESTkUtils/ui/widget/tk_2d_vector_widget.cpp \
        source/QESTkUtils/ui/delegate/tk_color_swatch_property_item_delegate.cpp \
        source/QESTkUtils/ui/delegate/tk_plain_text_edit_property_item_delegate.cpp \
        source/QESTkUtils/ui/delegate/tk_slider_property_item_delegate.cpp


} else {
        SOURCES += ../../ESGamma/source/system/main.cpp
}

HEADERS += \
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
    ../../AssetIO/include/utility/elysian_ring_buffer.hpp \
    ../../ESGamma/include/crtp/elysian_debug.hpp \
    ../../ESGamma/include/crtp/elysian_lua_manager.hpp \
    ../../ESGamma/include/crtp/elysian_map_system.hpp \
    ../../ESGamma/include/gui/elysian_adventure_log.hpp \
    ../../ESGamma/include/gui/elysian_font.hpp \
    ../../ESGamma/include/lua/elysian_lua_manager_base.hpp \
    ../../ESGamma/include/lua/elysian_lua_script.hpp \
    ../../ESGamma/include/lua/elysian_lua_singleton.hpp \
    ../../ESGamma/include/lua/elysian_lua_wrappers.hpp \
    ../../ESGamma/include/system/elysian_audio_system.hpp \
    ../../ESGamma/include/system/elysian_behavior_system.hpp \
    ../../ESGamma/include/system/elysian_character_system.hpp \
    ../../ESGamma/include/system/elysian_collision_system.hpp \
    ../../ESGamma/include/system/elysian_combat_system.hpp \
    ../../ESGamma/include/system/elysian_day_system.hpp \
    ../../ESGamma/include/system/elysian_dialog_system.hpp \
    ../../ESGamma/include/system/elysian_engine.hpp \
    ../../ESGamma/include/system/elysian_examinable_system.hpp \
    ../../ESGamma/include/system/elysian_light_system.hpp \
    ../../ESGamma/include/system/elysian_map_system_base.hpp \
    ../../ESGamma/include/system/elysian_party_system.hpp \
    ../../ESGamma/include/system/elysian_rigid_body_system.hpp \
    ../../ESGamma/include/system/elysian_skeleton_system.hpp \
    ../../ESGamma/include/system/elysian_system.hpp \
    ../../ESGamma/include/system/elysian_warp_system.hpp \
    ../../ESGamma/include/system/elysian_pathfinding_system.hpp \
    ../../ESGamma/include/utility/elysian_debug_base.hpp \
    ../../ESGamma/include/utility/elysian_event_monitor.hpp \
    ../../ESGamma/include/utility/elysian_force_generator.hpp \
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
    ../../ESGamma/include/proxy/elysian_audio_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_camera_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_character_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_combat_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_day_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_dialog_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_event_monitor_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_gui_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_level_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_light_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_particle_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_party_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_timer_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_rigid_body_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_pathfinding_system_proxy.hpp \
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
    ../../ESGamma/include/proxy/elysian_input_system_proxy.hpp \
    ../../ESGamma/include/system/elysian_input_system_base.hpp \
    ../../ESGamma/include/crtp/elysian_input_system.hpp \
    ../../game/include/behavior/elysian_playable_behavior.hpp \
    ../../AssetIO/include/utility/elysian_lua_stack.hpp \
    ../../AssetIO/include/utility/elysian_functional.hpp \
    ../../ESGamma/include/crtp/elysian_level_system.hpp \
    ../../ESGamma/include/system/elysian_level_system_base.hpp \
    ../../game/include/behavior/elysian_state_machine_behavior.hpp \
    ../../AssetIO/include/utility/elysian_oolua_string_traits.hpp \
    ../../ESGamma/include/system/elysian_gui_system_base.hpp \
    ../../ESGamma/include/crtp/elysian_gui_system.hpp \
    ../../ESGamma/include/gui/elysian_widget.hpp \
    ../../ESGamma/include/gui/elysian_layout.hpp \
    ../../ESGamma/include/gui/elysian_entity_widget.hpp \
    ../../ESGamma/include/gui/elysian_label_widget.hpp \
    ../../ESGamma/include/proxy/elysian_widget_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_layout_proxy.hpp \
    ../../AssetIO/include/utility/elysian_cpp_lua_callback.hpp \
    ../../ESGamma/include/gui/elysian_menu_widget.hpp \
    ../../AssetIO/include/utility/elysian_associative_aggregate_array.hpp \
    ../../ESGamma/include/system/elysian_event_system.hpp \
    ../../ESGamma/include/utility/elysian_performance_monitor.hpp \
    ../../AssetIO/include/crtp/elysian_color.hpp \
    ../../AssetIO/include/primitive/elysian_color_base.hpp \
    ../../ESGamma/include/system/elysian_particle_system_base.hpp \
    ../../ESGamma/include/crtp/elysian_particle_system.hpp \
    ../../AssetIO/include/primitive/elysian_rect.hpp \
    ../../AssetIO/include/level/elysian_event.hpp \
    ../../AssetIO/include/proxy/elysian_event_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_event_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_collision_system_proxy.hpp \
    ../../ESGamma/include/proxy/elysian_examinable_system_proxy.hpp \
    ../../ESGamma/include/device/elysian_rumble_device.hpp \
    ../../ESGamma/include/device/elysian_player_io_device.hpp \
    ../../ESGamma/include/gui/elysian_frame_widget.hpp \
    ../../game/include/pause_menu/es_options_menu.hpp \
    ../../game/include/pause_menu/es_pause_menu.hpp \
    ../../game/include/pause_menu/es_playable_pane.hpp \
    ../../game/include/pause_menu/es_party_menu.hpp \
    ../../game/include/pause_menu/es_inventory_menu.hpp \
    ../../ESGamma/include/system/elysian_compute_system.hpp \
    ../../ESGamma/include/crtp/elysian_render_system.hpp \
    include/ESGamma/tk_render_system.h \
    ../../ESGamma/include/system/elysian_render_system_base.hpp \
    ../../ESGamma/include/gui/elysian_item_model.hpp \
    ../../ESGamma/include/gui/elysian_item_delegate.hpp \
    ../../AssetIO/include/level/elysian_scene_object.hpp \
    ../../ESGamma/include/system/elysian_thread_system.hpp \
    include/QESTkUtils/ui/widget/tk_pie_chart_widget.h \
    include/QESTkUtils/ui/widget/tk_line_chart.h

!ios:!dreamcast: !android {
    HEADERS +=  \
        include/AssetIO/tk_area.h \
        include/ESToolkit/core/tk_asset_manager.h \
        include/ESToolkit/core/tk_assets.h \
        include/AssetIO/tk_audio_source.h \
        include/AssetIO/tk_behavior.h \
        include/AssetIO/tk_character.h \
        include/AssetIO/tk_collidable.h \
        include/AssetIO/tk_combat.h \
        include/AssetIO/tk_component.h \
        include/AssetIO/tk_elysian_tile_wrapper.h \
        include/AssetIO/tk_entity.h \
        include/AssetIO/tk_examinable.h \
        include/AssetIO/tk_level.h \
        include/AssetIO/tk_light_source.h \
        include/AssetIO/tk_lua_table_proxy.h \
        include/AssetIO/tk_map_entry.h \
        include/AssetIO/tk_particle_emitter.h \
        include/ESToolkit/core/tk_project.h \
        include/AssetIO/tk_renderable.h \
        include/AssetIO/tk_rigid_body.h \
        include/AssetIO/tk_skeleton.h \
        include/AssetIO/tk_warp.h \
        include/AssetIO/tk_lua_behavior.h \
        include/AssetIO/tk_depth_buff_entry.h \
        include/AssetIO/tk_sprite_attrib.h \
        include/AssetIO/tk_dialog_screen.h \
        include/AssetIO/tk_dialog.h \
        include/AssetIO/tk_color.h \
        include/AssetIO/tk_lua_variant.h \
        include/ESToolkit/ui/item/tk_checkered_alpha_rect_graphics_item.h \
        include/ESToolkit/core/tk_debug.h \
        include/ESToolkit/core/tk_file_utilities.h \
        include/QESTkUtils/tk_gl_utilities.h \
        include/QESTkUtils/tk_menu_stack.h \
        include/ESToolkit/core/tk_selectable.h \
        include/ESToolkit/ui/widget/tk_selection_scene.h \
        include/ESToolkit/core/tk_tile_image.h \
        include/ESToolkit/core/tk_tile_sheet_utilities.h \
        include/ESToolkit/ui/item/tk_tree_node.h \
        include/ESToolkit/core/tk_undo_item.h \
        include/ESToolkit/ui/dialog/tk_engine_dialog.h \
        include/ESToolkit/ui/dialog/tk_engine_settings_dialog.h \
        include/ESToolkit/ui/dock/asset/tk_asset_dock_widget.h \
        include/ESToolkit/ui/widget/tk_engine_gl_scene_widget.h \
        include/ESToolkit/ui/dock/tk_entity_dock_widget.h \
        include/ESToolkit/core/tk_entity_selectable.h \
        include/ESToolkit/ui/widget/tk_entity_tree_widget.h \
        include/ESToolkit/ui/tk_estoolkit.h \
        include/ESToolkit/ui/dock/tk_layer_dock_widget.h \
        include/ESGamma/tk_lua_manager.h \
        include/ESGamma/tk_lua_wrappers.h \
        include/ESGamma/tk_map_system.h \
        include/ESToolkit/ui/widget/tk_map_view.h \
        include/ESToolkit/ui/item/tk_map_view_depth_buffer_graphics_item.h \
        include/ESToolkit/core/tk_map_view_place_tile_group_undo_item.h \
        include/ESToolkit/ui/item/tk_map_view_tile_graphics_item.h \
        include/ESToolkit/ui/dock/tk_selection_dock_widget.h \
        include/ESToolkit/ui/dock/tk_sheet_dock_widget.h \
        include/ESToolkit/ui/widget/tk_sheet_view.h \
        include/ESToolkit/ui/item/tk_sheet_view_collider_graphics_item.h \
        include/ESToolkit/ui/item/tk_sheet_view_tile_graphics_item.h \
        include/ESToolkit/core/tk_tile_selectable.h \
        include/ESToolkit/ui/delegate/tk_tile_selectable_selection_view_delegate.h \
        include/QESTkUtils/model/tk_lua_table_stack_model.h \
        include/QESTkUtils/model/tk_property_stack_model.h \
        include/QESTkUtils/model/tk_property_standard_item.h \
        include/QESTkUtils/model/tk_rows_joiner_proxy_model.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_asset_manager.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_assets.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_dest_tile_graphics_item.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_dest_view.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_file_dock_widget.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_place_tile_group_undo_item.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_source_tile_graphics_item.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_source_view.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_tile_entry.h \
        include/ESToolkit/sheet_manager/tk_sheet_manager_tile_selection.h \
        include/QESTkUtils/tile_engine/tk_dest_view.h \
        include/QESTkUtils/tile_engine/tk_selection_item_group.h \
        include/QESTkUtils/tile_engine/tk_source_view.h \
        include/QESTkUtils/tile_engine/tk_tile_graphics_item.h \
        include/QESTkUtils/tile_engine/tk_tile_group_undo_item.h \
        include/QESTkUtils/tile_engine/tk_tile_selection.h \
        include/ESToolkit/ui/dock/tk_history_dock_widget.h \
        include/QESTkUtils/tile_engine/tk_tile_graphics_item_impl.h \
        include/QESTkUtils/tile_engine/tk_dest_view_impl.h \
        include/QESTkUtils/tk_lua_qt_utils.h \
        include/QESTkUtils/ui/widget/tk_documentation_browser.h \
        include/ESGamma/tk_task_monitor.h \
        include/ESToolkit/ui/dialog/tk_task_monitor_dialog.h \
        include/ESGamma/tk_timer_system.h \
        include/QESTkUtils/ui/widget/tk_gitlab_dialog.h \
        include/QESTkUtils//tk_network_access_manager.h \
        include/ESToolkit/ui/dock/tk_camera_dock_widget.h \
        include/ESGamma/tk_camera_system.h \
        include/ESGamma/tk_input_system.h \
        include/QESTkUtils/ui/delegate/tk_property_item_delegate.h \
        include/QESTkUtils/ui/delegate/tk_spinbox_property_item_delegate.h \
        include/QESTkUtils/ui/delegate/tk_file_browser_property_item_delegate.h \
        include/QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h \
        include/QESTkUtils/ui/delegate/tk_property_item_proxy_delegate.h \
        include/QESTkUtils/ui/delegate/tk_combobox_property_item_delegate.h \
        include/QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h \
        include/ESToolkit/ui/widget/tk_selection_tree_view.h \
        include/QESTkUtils/tk_lua_mime_drop_manager.h \
        include/ESGamma/tk_particle_system.h \
        include/ESToolkit/ui/dock/tk_particle_attrib_preview_dock_widget.h \
        include/QESTkUtils/ui/widget/tk_2d_vector_widget.h \
        include/QESTkUtils/ui/delegate/tk_color_swatch_property_item_delegate.h \
        include/ESToolkit/core/tk_lua_table_selectable.h \
        include/ESGamma/tk_gui_system.h \
        include/ESGamma/tk_level_system.h \
        include/QESTkUtils/tk_mime_data.h \
        include/QESTkUtils/tk_mime_drop_manager.h \
        include/QESTkUtils/ui/delegate/tk_plain_text_edit_property_item_delegate.h \
        include/QESTkUtils/ui/delegate/tk_slider_property_item_delegate.h
}


FORMS += form/ESToolkit/dock/tk_sheet_dock_widget.ui \
    form/ESToolkit/dock/tk_selection_dock_widget.ui \
    form/ESToolkit/dock/tk_layer_dock_widget.ui \
    form/ESToolkit/tk_estoolkit.ui \
    form/ESToolkit/dock/tk_entity_dock_widget.ui \
    form/ESToolkit/dock/tk_asset_dock_widget.ui \
    form/ESToolkit/sheet_manager/tk_sheet_manager.ui \
    form/ESToolkit/dock/tk_history_dock_widget.ui \
    form/ESToolkit/tk_engine_settings.ui \
    form/ESToolkit/sheet_manager/tk_sheet_manager_file_dock_widget.ui \
    form/QESTkUtils/tk_gitlab_dialog.ui \
    form/ESToolkit/dock/tk_camera_dock_widget.ui

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
    QMAKE_CXXFLAGS +=  -stdlib=libc++
    LIBS += -stdlib=libc++

    #CONFIG(release, debug|release) {
     #  QMAKE_POST_LINK = "macdeployqt TARGET"
    #}
}

opencl {
    LIBS += -framework OpenCL
    QMAKE_CXXFLAGS += -DELYSIAN_OPENCL_ACCELERATED

}

RESOURCES += resource/resource.qrc

vmu_child_app {
    RESOURCES += ../../ElysianVMU/app/qt/resource/evmu.qrc
}

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
    #    CONFIG(release, debug|release):
    release: QMAKE_POST_LINK = "sh ./macdeployes.sh"
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
      #  QMAKE_BUNDLE_DATA += deployment
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

    LIBS +=        \
        -lOpenCL
}

linux {
    DESTDIR = ../../executable
}


vmu_child_app {

    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../ElysianVMU/build/qtcreator/ElysianVMU/release/ -lElysianVMU
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../ElysianVMU/build/qtcreator/ElysianVMU/debug/ -lElysianVMU
    else:unix: LIBS += -L$$OUT_PWD/../../ElysianVMU/build/qtcreator/ElysianVMU -lElysianVMU

    INCLUDEPATH += $$PWD/../../ElysianVMU/build/qtcreator/ElysianVMU
    DEPENDPATH += $$PWD/../../ElysianVMU/build/qtcreator/ElysianVMU

    win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ElysianVMU/build/qtcreator/ElysianVMU/release/libElysianVMU.a
    else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ElysianVMU/build/qtcreator/ElysianVMU/debug/libElysianVMU.a
    else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ElysianVMU/build/qtcreator/ElysianVMU/release/ElysianVMU.lib
    else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../ElysianVMU/build/qtcreator/ElysianVMU/debug/ElysianVMU.lib
    else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../ElysianVMU/build/qtcreator/ElysianVMU/libElysianVMU.a
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libGyro-Kinetic/release/ -llibGyroKinetic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libGyro-Kinetic/debug/ -llibGyroKinetic
else:unix: LIBS += -L$$OUT_PWD/../../libGyro-Kinetic/ -llibGyroKinetic

INCLUDEPATH += $$PWD/../../libGyro-Kinetic
DEPENDPATH += $$PWD/../../libGyro-Kinetic

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro-Kinetic/release/liblibGyroKinetic.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro-Kinetic/debug/liblibGyroKinetic.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro-Kinetic/release/libGyroKinetic.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libGyro-Kinetic/debug/libGyroKinetic.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../libGyro-Kinetic/liblibGyroKinetic.a

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
        -lSDL2_net \
        -lSDL2_net.dll \
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
  LIBS += -L$${LIBGYRO_PATH}/lib/osx \
        -framework OpenGL \
        -framework OpenCL \
        -framework OpenAL \
        -lz \
        -F$${PWD}/../../ESGamma/lib/osx \
        -L$${PWD}/../../ESGamma/lib/osx \
        -F$${LIBGYRO_PATH}/lib/osx \
        -framework SDL2 \
        -framework SDL2_net \
        -framework Foundation \
        -framework Cocoa \
        -lobjc \
        -logg \
        -lvorbis \
        -lvorbisenc \
        -lvorbisfile \

}

ios {

    LIBS += -lstdc++ \
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
        -lSDL2 \
        -lSDL2_net \
        -lOpenCL \
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

