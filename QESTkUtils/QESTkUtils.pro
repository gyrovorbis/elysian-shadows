#-------------------------------------------------
#
# Project created by QtCreator 2018-07-23T10:48:33
#
#-------------------------------------------------

QT       += widgets network

TARGET = QESTkUtils
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    source/model/tk_lua_table_stack_model.cpp \
    source/model/tk_property_stack_model.cpp \
    source/model/tk_rows_joiner_proxy_model.cpp \
    source/tile_engine/tk_source_view.cpp \
    source/ui/delegate/tk_color_swatch_property_item_delegate.cpp \
    source/ui/delegate/tk_combobox_property_item_delegate.cpp \
    source/ui/delegate/tk_dynamic_content_property_item_delegate.cpp \
    source/ui/delegate/tk_enum_dropdown_property_item_delegate.cpp \
    source/ui/delegate/tk_file_browser_property_item_delegate.cpp \
    source/ui/delegate/tk_plain_text_edit_property_item_delegate.cpp \
    source/ui/delegate/tk_property_item_delegate.cpp \
    source/ui/delegate/tk_property_item_proxy_delegate.cpp \
    source/ui/delegate/tk_slider_property_item_delegate.cpp \
    source/ui/delegate/tk_spinbox_property_item_delegate.cpp \
    source/ui/widget/tk_2d_vector_widget.cpp \
    source/ui/widget/tk_documentation_browser.cpp \
    source/ui/widget/tk_gitlab_dialog.cpp \
    source/ui/widget/tk_pie_chart_view.cpp \
    source/tk_lua_mime_drop_manager.cpp \
    source/tk_lua_qt_utils.cpp \
    source/tk_menu_stack.cpp \
    source/tk_mime_data.cpp \
    source/tk_mime_drop_manager.cpp \
    source/tk_network_access_manager.cpp

HEADERS += \
    include/model/tk_lua_table_stack_model.h \
    include/model/tk_property_stack_model.h \
    include/model/tk_property_standard_item.h \
    include/model/tk_rows_joiner_proxy_model.h \
    include/tile_engine/tk_dest_view.h \
    include/tile_engine/tk_dest_view_impl.h \
    include/tile_engine/tk_selection_item_group.h \
    include/tile_engine/tk_source_view.h \
    include/tile_engine/tk_tile_graphics_item.h \
    include/tile_engine/tk_tile_graphics_item_impl.h \
    include/tile_engine/tk_tile_group_undo_item.h \
    include/tile_engine/tk_tile_selection.h \
    include/ui/delegate/tk_color_swatch_property_item_delegate.h \
    include/ui/delegate/tk_combobox_property_item_delegate.h \
    include/ui/delegate/tk_dynamic_content_property_item_delegate.h \
    include/ui/delegate/tk_enum_dropdown_property_item_delegate.h \
    include/ui/delegate/tk_file_browser_property_item_delegate.h \
    include/ui/delegate/tk_plain_text_edit_property_item_delegate.h \
    include/ui/delegate/tk_property_item_delegate.h \
    include/ui/delegate/tk_property_item_proxy_delegate.h \
    include/ui/delegate/tk_slider_property_item_delegate.h \
    include/ui/delegate/tk_spinbox_property_item_delegate.h \
    include/ui/widget/tk_2d_vector_widget.h \
    include/ui/widget/tk_documentation_browser.h \
    include/ui/widget/tk_gitlab_dialog.h \
    include/ui/widget/tk_pie_chart_view.h \
    include/tk_gl_utilities.h \
    include/tk_lua_mime_drop_manager.h \
    include/tk_lua_qt_utils.h \
    include/tk_menu_stack.h \
    include/tk_mime_data.h \
    include/tk_mime_drop_manager.h \
    include/tk_network_access_manager.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
