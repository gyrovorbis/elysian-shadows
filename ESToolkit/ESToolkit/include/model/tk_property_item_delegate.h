#ifndef TK_PROPERTY_ITEM_DELEGATE_H
#define TK_PROPERTY_ITEM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QVector>
#include <estk/tk_lua_manager.h>

class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QMenu;
class ESTkStandardItem;

class PropertyItemDelegate: public QStyledItemDelegate, public TkLuaManager {
protected:
    bool            _initialized = false;
    bool            _addPairs   = true;
    QMenu*          _contextMenu = nullptr;
    QVector<int>    _supportedTypes = {
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

    QVector<QString> _supportedUserData;


    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const = 0;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const = 0;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const = 0;

public:
    PropertyItemDelegate(void) = default;

    virtual bool    editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    void            setContextMenu(QMenu*const menu);
    virtual QMenu*          getContextMenu(ESTkStandardItem*const item);
    void            addSupportedType(const int luaType);

};

class PropertyItemProxyDelegate: public QStyledItemDelegate {
    static PropertyItemDelegate*    _defaultTableDelegate;

    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual bool    editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
   // virtual QString displayText(const QVariant & value, const QLocale & locale) const override;

};
#if 0
class LuaPropertyItemProxyDelegate: public QStyledItemDelegate {
    static PropertyItemDelegate*    _defaultTableDelegate = nullptr;


};
#endif

class EnumDropdownPropertyItemDelegate: public PropertyItemDelegate {
private:
    QStringList    _stringNames;

protected:
    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  //  virtual QString displayText(const QVariant & value, const QLocale & locale) const override;


public:

    EnumDropdownPropertyItemDelegate(void) = default;
    EnumDropdownPropertyItemDelegate(QStringList strings);

    void insertEnumValue(const char* name);



};


class SpinboxPropertyItemDelegate: public PropertyItemDelegate {
private:
    int     _min = 0;
    int     _max = 100;

protected:
    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  //  virtual QString displayText(const QVariant & value, const QLocale & locale) const override;


public:

    SpinboxPropertyItemDelegate(void) = default;
    SpinboxPropertyItemDelegate(const int max, const int min);

    int   getMinValue(void) const;
    int   getMaxValue(void) const;
    void            setMinValue(const int min);
    void            setMaxValue(const int max);



};

class FileBrowserPropertyItemDelegate: public PropertyItemDelegate {
private:
    mutable QLineEdit*      _pathEdit       = nullptr;
    mutable QPushButton*    _browseButton   = nullptr;
    mutable QWidget*        _parentWidget   = nullptr;
    mutable QHBoxLayout*    _layout         = nullptr;
    bool                    _dir            = false;

protected:
    virtual QWidget*createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void    setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void    setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  //  virtual QString displayText(const QVariant & value, const QLocale & locale) const override;

public:

    FileBrowserPropertyItemDelegate(void);

        void            setDir(const bool val);


};







// ====== INLINEZ ======

inline void PropertyItemDelegate::setContextMenu(QMenu * const menu) { _contextMenu = menu; }
inline void PropertyItemDelegate::addSupportedType(const int luaType) { _supportedTypes.push_back(luaType); }

inline SpinboxPropertyItemDelegate::SpinboxPropertyItemDelegate(const int min, const int max): PropertyItemDelegate(), _min(min), _max(max) {}

inline void SpinboxPropertyItemDelegate::setMinValue(const int min) { _min = min; }
inline void SpinboxPropertyItemDelegate::setMaxValue(const int max) { _max = max; }

inline EnumDropdownPropertyItemDelegate::EnumDropdownPropertyItemDelegate(QStringList strings): PropertyItemDelegate(), _stringNames(strings) {}

inline void EnumDropdownPropertyItemDelegate::insertEnumValue(const char* name) {
    _stringNames.push_back(name);
}

inline void FileBrowserPropertyItemDelegate::setDir(const bool val) {
    _dir = val;
}



#endif // TK_PROPERTY_ITEM_DELEGATE_H
