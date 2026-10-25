/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_PROPERTY_ITEM_DELEGATE_H
#define TK_PROPERTY_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QVector>
#include <functional>
#include <ESToolkit/ESGamma/tk_lua_manager.h>
#include <ESToolkit/core/tk_asset_manager.h>
#include <QCompleter>

class QMenu;
class ESTkStandardItem;
class LuaTableFieldItem;
class MimeDropManager;

/* Default delegate type for SelectionView items. Also implements default functionality for Lua
 * proxy types by checking whether the PropertyItem type is Lua. If not, it returns default
 * behavior.
 */
class PropertyItemDelegate: public QStyledItemDelegate, public TkLuaManager, public AssetManager {
    Q_OBJECT
public:
    using MenuCallbackType                  = std::function<QMenu*(ESTkStandardItem*)>;
    using CompleterCallbackType             = std::function<QStringList(ESTkStandardItem*)>;

protected:
    bool                _addPairs           = true;
    QVector<int>        _supportedTypes     = {
        LUA_TNIL,
        LUA_TBOOLEAN,
        LUA_TNUMBER,
        LUA_TSTRING,
        //LUA_TFUNCTION,
        //LUA_TTHREAD,
        LUA_TTABLE,
        //LUA_TLIGHTUSERDATA,
        LUA_TUSERDATA
    };
    MenuCallbackType        _menuCallback;
    CompleterCallbackType   _completerCallback;
    QVector<QString>        _supportedUserData;
    MimeDropManager*        _mimeDropManager    = nullptr;
    int                     _completerMode;
    bool                    _readOnly           = false;

    //virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const = 0;
    //virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const = 0;
    //virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const = 0;

public:
                                PropertyItemDelegate(bool readOnly=false);

    virtual bool                editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    virtual QWidget*            createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void                        setContextMenuCallback(MenuCallbackType callback);
    void                        setCompleterCallback(CompleterCallbackType callback, int mode=QCompleter::PopupCompletion);
    virtual QMenu*              getContextMenu(ESTkStandardItem*const item);

    virtual bool                canDropMimeData(const QMimeData* data, Qt::DropAction action, int row , int col, const QModelIndex& parent) const;
    virtual bool                dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int col, const QModelIndex& parent) const;
    void                        setMimeDropManager(MimeDropManager* mgr);

    void                        addSupportedType(const int luaType);
    bool                        isReadOnly(void) const;
    void                        setReadOnly(bool val=true);
    static LuaTableFieldItem*   modelIndexToLuaTableFieldItem(const QModelIndex& index);
    static ESTkStandardItem*    modelIndexToESTkStandardItem(const QModelIndex& index);

};




// ====== INLINEZ ======
inline PropertyItemDelegate::PropertyItemDelegate(bool readOnly): _readOnly(readOnly) {}
inline void PropertyItemDelegate::addSupportedType(const int luaType) { _supportedTypes.push_back(luaType); }
inline void PropertyItemDelegate::setContextMenuCallback(MenuCallbackType callback) { _menuCallback = std::move(callback); }
inline void PropertyItemDelegate::setMimeDropManager(MimeDropManager* mgr) { _mimeDropManager = mgr; }
inline void PropertyItemDelegate::setCompleterCallback(CompleterCallbackType callback, int mode) { _completerCallback = std::move(callback); _completerMode = mode; }
inline bool PropertyItemDelegate::isReadOnly(void) const { return _readOnly; }
inline void PropertyItemDelegate::setReadOnly(bool val) { _readOnly = val; }





#endif // TK_PROPERTY_ITEM_DELEGATE_H
