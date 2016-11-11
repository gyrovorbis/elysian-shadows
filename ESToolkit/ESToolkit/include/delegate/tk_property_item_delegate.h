#ifndef TK_PROPERTY_ITEM_DELEGATE_H
#define TK_PROPERTY_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QVector>
#include <functional>
#include <estk/tk_lua_manager.h>
#include <asset/tk_asset_manager.h>

class QMenu;
class ESTkStandardItem;
class LuaTableFieldItem;

/* Default delegate type for SelectionView items. Also implements default functionality for Lua
 * proxy types by checking whether the PropertyItem type is Lua. If not, it returns default
 * behavior.
 */
class PropertyItemDelegate: public QStyledItemDelegate, public TkLuaManager, public AssetManager {
public:
    using MenuCallbackType                  = std::function<QMenu*(ESTkStandardItem*)>;

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
    MenuCallbackType    _menuCallback;
    QVector<QString>    _supportedUserData;


    //virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const = 0;
    //virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const = 0;
    //virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const = 0;

public:
                                PropertyItemDelegate(void) = default;

    virtual bool                editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    virtual QWidget*            createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void                        setContextMenuCallback(MenuCallbackType callback);
    virtual QMenu*              getContextMenu(ESTkStandardItem*const item);
    void                        addSupportedType(const int luaType);
    static LuaTableFieldItem*   modelIndexToLuaTableFieldItem(const QModelIndex& index);
    static ESTkStandardItem*    modelIndexToESTkStandardItem(const QModelIndex& index);

};




// ====== INLINEZ ======

inline void PropertyItemDelegate::addSupportedType(const int luaType) { _supportedTypes.push_back(luaType); }
inline void PropertyItemDelegate::setContextMenuCallback(MenuCallbackType callback) { _menuCallback = std::move(callback); }





#endif // TK_PROPERTY_ITEM_DELEGATE_H
