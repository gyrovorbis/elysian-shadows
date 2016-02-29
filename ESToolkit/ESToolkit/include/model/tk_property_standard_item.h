#ifndef PROPERTY_STANDARD_ITEM_H
#define PROPERTY_STANDARD_ITEM_H

#include <QStandardItem>
#include <QVector>
#include <QIcon>
#include <QDebug>
#include <tk_undo_item.h>

class QStyledItemDelegate;

enum class ESTkStandardItemType: uint32_t {
    Generic = QStandardItem::UserType+1,
    LuaTable
};

/*
  Base class for ALL custom QStandardItems within Toolkit.
  Provides accessors for resource management, so that the
  standard item can be shared between a model and undoItems.
*/
class ESTkStandardItem: public QStandardItem {
protected:
    QVector<ESTkStandardItem*>      _children;
    mutable QStyledItemDelegate*    _delegate       = nullptr;
    unsigned                        _refCount       = 0;
    bool                            _initialized    = false;
    bool                            _active         = false;
public:
    ESTkStandardItem(void) = default;

    ESTkStandardItem(const QString string): QStandardItem(string) {}

    virtual int type(void) const override { return (int)ESTkStandardItemType::Generic; }
  void acquire(void) { ++_refCount; }
  void release(void) { if(--_refCount == 0) delete this; }
  unsigned getRefCount(void) const { return _refCount; }
    virtual UndoItem::TYPE getUndoItemType(void) const { return UndoItem::UNDO_NONE; }

    void activate(void) {
        if(!_active) {
            insertingModelItem();
            _active = true;
        }
    }

    virtual void insertingModelItem(void) { }

    virtual void forceUpdate(void) {
        emitDataChanged();
        for(auto it : _children) {
            it->forceUpdate();
        }
    }

    using QStandardItem::emitDataChanged;

    void addChildItem(ESTkStandardItem*const item) { _children.push_back(item); }
    virtual QStyledItemDelegate* getStyledItemDelegate(void) const { return _delegate; }
};

template <bool UndoRedo, typename T, typename C>
struct _PropertyStandardItem;

/*
  PropertyStandardItem provides a mechanism for using existing standalone data
  structures within a QT StandardItemModel. It forces synchronization between
  the QStandardItem and an existing object by invoking the object's accessor methods.

  PropertyStandardItems can:
  - be read only (no set accessor specified)
  - automatically update the QStandardItem's icon if an updateIcon function is specified
  - automatically generate undo/redo events if the UndoRedo template parameter is true
 */

template <typename T, typename C, bool UndoRedo=false>
class PropertyStandardItem: public ESTkStandardItem {
  template<bool TUndoRedo, typename TT, typename CC>
  friend struct _PropertyStandardItem;
private:
    C*                  _objInst;
    T                   (C::*_getAccessor)(void) const;
    void                (C::*_setAccessor)(const T);
    QIcon               (C::*_updateIcon)(const T) const;
    QUndoStack*         _undoStack;
    UndoItem::TYPE      _undoType;
    bool                _ignoreCheck;
    bool                _displayValue = false;
    bool                _settingFlags = false;
  //unsigned _refCount;
protected:
    PropertyStandardItem(void) = default;
    void _latentInit(C *const object, T (C::*const get)(void)const,
                     void (C::*const set)(const T)=nullptr,
                     const UndoItem::TYPE undoType=UndoItem::TYPE_SIZE,
                     QUndoStack *stack=nullptr, QIcon (C::*const ico)(const T)const=nullptr)
    {
        _objInst = object;
        _getAccessor = get;
        _setAccessor = set;
        _updateIcon = ico;
        _undoStack = stack;
        _undoType = undoType;
        _ignoreCheck = true;

        /*setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable |
                 ((_setAccessor)? Qt::ItemIsEditable : Qt::NoItemFlags));*/
    }

public:
  PropertyStandardItem(C *const object, T (C::*const get)(void)const,
                         void (C::*const set)(const T)=nullptr,
             const UndoItem::TYPE undoType=UndoItem::TYPE_SIZE,
                         QUndoStack *stack=nullptr, QIcon (C::*const ico)(const T)const=nullptr):
    _objInst(object),
    _getAccessor(get),
    _setAccessor(set),
    _updateIcon(ico),
    _undoStack(stack),
        _undoType(undoType),
        _ignoreCheck(true)
    //  _refCount(0)
  {

        _settingFlags = true;
        //Prevent the asshole from immediately fetching and latently populating tables just because
        //flags are being set.
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable |
         ((_setAccessor)? Qt::ItemIsEditable : Qt::NoItemFlags));
        _settingFlags = false;

  }

    virtual void forceUpdate(void) override {
        ESTkStandardItem::forceUpdate();
        if(_updateIcon) setIcon((_objInst->*_updateIcon)((_objInst->*_getAccessor)()));
    }

    void unignoreCheck(void) { _ignoreCheck = false; }

  virtual UndoItem::TYPE getUndoItemType(void) const override { return _undoType; }

  /*
    Function ultimately called to update the internally encapsulated object's value.
    Invokes the set accessor, updates its icon (if required), then Q_SIGNALS an update
    to the view. No undo/redo logic is handled at this level. This should only be
    directly invoked by UndoProperty to prevent infinite undo/redo command generation.
   */
  void updateInternalObjectData(const T &value) {
    (_objInst->*_setAccessor)(value);
        forceUpdate(); //signal for view to update

  }

  virtual QVariant data(int role) const override {
        if(_settingFlags) return QVariant();
        T data = (_objInst->*_getAccessor)();

        switch((Qt::ItemDataRole)role) {
        case Qt::CheckStateRole:
            if(!isCheckable()) return QStandardItem::data(role);
            //Fucking terrible, but I don't want to specialize for bool.
            // TODO Marius: strict-aliasing warning here, might interfere with optimization
            else return *(reinterpret_cast<bool*>(&data))? Qt::Checked : Qt::Unchecked;
        case Qt::DisplayRole:
            //if(isCheckable())
            return _displayValue? QStandardItem::data(Qt::DisplayRole) : data;
        case Qt::EditRole:
            return data;
    default:
      return QStandardItem::data(role);
    }
  }

  virtual void setData(const QVariant &value, int role) override {
        //make sure shit actually changed
        T lastData;

        switch((Qt::ItemDataRole)role) {
        case Qt::CheckStateRole:
            if(!_ignoreCheck) {
                lastData = (_objInst->*_getAccessor)();
                if(lastData != value.value<T>()) {
                    _PropertyStandardItem<UndoRedo, T, C>::setDataEditRole(this, value);
                    return QStandardItem::setData(value, role);
                }
            }
            break;
        case Qt::DisplayRole:
            _displayValue = true;
            return QStandardItem::setData(value, role);
    case Qt::EditRole:
            lastData = (_objInst->*_getAccessor)();
            if(lastData != value.value<T>()) {
                _PropertyStandardItem<UndoRedo, T, C>::setDataEditRole(this, value);
            }
            break;
    default:
      return QStandardItem::setData(value, role);
    }
  }

};


/*
  Additional layer of template mindfuckery to allow
  PropertyStandardItems to automatically generate QUndoCommands
*/
template <typename T, typename C>
class UndoProperty: public UndoItem {
  template<bool TUndoRedo, typename TT, typename CC>
  friend struct _PropertyStandardItem;
private:
  T _prevValue;
  T _newValue;
  PropertyStandardItem<T, C, true> *const _item;

  //Only constructable via PropertyStandardItem
  UndoProperty(const TYPE type, const T &newValue,
         PropertyStandardItem<T, C, true> *const item):
    UndoItem(type),
    _newValue(newValue),
    _item(item)
  {
    //Why in FUCK'S CUNT can't the compiler handle this line?
    //_prevValue = _item->data(Qt::EditRole).value<T>();
    _item->acquire();
    QVariant variant = _item->data(Qt::EditRole);
    _prevValue = variant.value<T>();
  }

  virtual ~UndoProperty(void) { _item->release(); }

public:
    virtual void undo(void) override {
    _item->updateInternalObjectData(_prevValue);
  }
  \
    virtual void redo(void) override {
    _item->updateInternalObjectData(_newValue);
  }
};

/*External helper struct templates used by PropertyStandardItems.
  Since template specialization requires the first template parameter(s) to be explicit,
  and the <bool UndoRedo> template parameter is the last (and optional) template parameter
  within PropertyStandardItem, we had to move this logic elsewhere to allow us to partially
  specialize this parameter. C++ template limitation.
*/

//For PropertyStandardItems WITHOUT auto Undo/Redo
template<bool UndoRedo, typename T, typename C>
struct _PropertyStandardItem {
  static void setDataEditRole(PropertyStandardItem<T, C, UndoRedo> *item,
                                const QVariant &value)
    {
    item->updateInternalObjectData(value.value<T>());
  }

};

//For PropertyStandardItems WITH auto Undo/Redo
template<typename T, typename C>
struct _PropertyStandardItem<true, T, C> {
  static void setDataEditRole(PropertyStandardItem<T, C, true> *item,
                                const QVariant &value)
    {
    UndoItem *undoItem = new UndoProperty<T, C>(item->_undoType, value.value<T>(), item);
    item->_undoStack->push(undoItem);
  }

};

/*
Qt::NoItemFlags	0	It does not have any properties set.
Qt::ItemIsSelectable	1	It can be selected.
Qt::ItemIsEditable	2	It can be edited.
Qt::ItemIsDragEnabled	4	It can be dragged.
Qt::ItemIsDropEnabled	8	It can be used as a drop target.
Qt::ItemIsUserCheckable	16	It can be checked or unchecked by the user.
Qt::ItemIsEnabled	32	The user can interact with the item.
Qt::ItemIsTristate	64	The item is checkable with three separate state
  */
/*
template<typename T, typename C> template<>
Qt::ItemFlags PropertyStandardItem<T, C>::propertyItemFlags<T>(void) {

  return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | (_setAccessor)? Qt::ItemIsEditable : 0);
}

template<typename C> template<>
Qt::ItemFlags PropertyStandardItem<bool, C>::propertyItemFlags<bool>(void) {
  return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | (_setAccessor)? Qt::ItemIsEditable : 0);
}
*/



#endif // PROPERTY_STANDARD_ITEM_H
