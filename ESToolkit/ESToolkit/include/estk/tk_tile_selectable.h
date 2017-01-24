#ifndef TILE_SELECTABLE_H
#define TILE_SELECTABLE_H

#include <QStyledItemDelegate>
#include <QMap>
#include <functional>
#include <tk_tile_selection.h>
#include <level/elysian_tile.hpp>
#include <tk_property_standard_item.h>
#include <tk_property_stack_model.h>

class TileSelectable;
class TileSelectableSelectionViewDelegate;

class TileSelectable:
        public TileSelection<TileEntry>,
        public Selectable,
        public PropertyStackModel
{
private:
    QVector<uint16_t>                           _tileIndices;
    Assets::LAYER_TYPE                          _layer            = Assets::LAYER_TILE1;
    Level* const                                _level            = nullptr;
    Area* const                                 _area             = nullptr;
    GY_RECT_PLANE_TYPE                          _planeType        = GY_RECT_PLANE_BOTTOM;

    ESTkStandardItem*                           _groupCol         = nullptr;
    ESTkStandardItem*                           _groupStandOn     = nullptr;
    ESTkStandardItem*                           _groupTrans       = nullptr;
    ESTkStandardItem*                           _groupNormal      = nullptr;
    ESTkStandardItem*                           _groupSpecular    = nullptr;

    std::function<QString (uint16_t)>           _groupColGet;
    std::function<void (uint16_t, QString)>     _groupColSet;

    std::function<bool (uint16_t)>              _groupStandOnGet;
    std::function<void (uint16_t, bool)>        _groupStandOnSet;

    std::function<bool (uint16_t)>              _groupTransGet;
    std::function<void (uint16_t, bool)>        _groupTransSet;

    std::function<bool (uint16_t)>              _groupNormalGet;
    std::function<void (uint16_t, bool)>        _groupNormalSet;

    std::function<bool (uint16_t)>              _groupSpecularGet;
    std::function<void (uint16_t, bool)>        _groupSpecularSet;



    //Shared delegate instance for displaying all TileSelectables
    static TileSelectableSelectionViewDelegate* _selectionViewDelegate;

protected:

    template<typename T>
    class TileSelectableUndoItem: public UndoItem {
    private:
        QMap<uint16_t, T>                   _prevValues;
        QMap<uint16_t, T>                   _newValues;
        ESTkStandardItem*                   _item         = nullptr;

        std::function<T (uint16_t)>         _getAccessor;
        std::function<void (uint16_t, T)>   _setAccessor;

    public:

        TileSelectableUndoItem(const QVector<uint16_t>& indices,
                               const T newVal,
                               const std::function<T (uint16_t)>& get,
                               const std::function<void (uint16_t, T)>& set,
                               ESTkStandardItem*const item,
                               const UndoItem::TYPE undoType);

        virtual void undo(void) override;
        virtual void redo(void) override;
    };


    template<typename T>
    T       _getTileGroupProperty(const std::function<T (uint16_t index)> operation, const T defaultValue) const;

    void    _populateTileIndicesList(void);

public:

    //Main constructor--based on QGraphicsItems that have been selected within the scene
    TileSelectable(const QList<QGraphicsItem *>& items, Area *const area, Level *const lev, const Assets::LAYER_TYPE layer);
    //Copy constructor
    TileSelectable(const TileSelectable &rval);
    //MEGA reesty constructor to optimize creation from MapView
    TileSelectable(const QRect &region, bool (*selectionArray)[ELYSIAN_AREA_MAP_WIDTH], Area* const area, Level* const lev, const Assets::LAYER_TYPE layer);
    //Set tile values yourself after construction (but size must static still)
    TileSelectable(const unsigned w, const unsigned h, Area* const area, Level* const level);

    virtual void                populateModel(void) override;
    virtual void                installSelectionViewCustomDelegates(QAbstractItemView *const view) override;
    virtual QAbstractItemModel* getModel(void) const override;
    Assets::LAYER_TYPE          getTileLayer(void) const { return _layer; }
    virtual Selectable::TYPE    getType(void) const override { return Selectable::TILE; }
    TileEntry					getPrevEntry(const unsigned x, const unsigned y) const;
    void						getScenePos(unsigned& x, unsigned& y) const;

    //void    updateSceneSelection(const unsigned sceneX, const unsigned sceneY);
    //void    clearSceneSelection(void);
    //void    clearPrevSceneSelection(void);

    //MapEntry modifiers
    void    elevateUp(void);
    void    elevateDown(void);
    void    elevateFromBase(void);
    void    toggleNormalFace(void);
    void    setElev(const unsigned elev);
    void    setPlane(const GY_RECT_PLANE_TYPE plane);
    void    setOrient(const GY_RECT_TEX_ORIENT orient);

    //Selection-View Tile Group Property Accessors
    QString getColliderString(void) const;
    void    setColliderString(const QString string);
    bool    getStandOn(void) const;
    void    setStandOn(const bool val);
    bool    getTransparency(void) const;
    void    setTransparency(const bool val);
    bool    getNormalMapEnabled(void) const;
    void    setNormalMapEnabled(const bool val);
    bool    getSpecularMapEnabled(void) const;
    void    setSpecularMapEnabled(const bool val);
    QIcon   updateColliderIcon(const QString string) const;

};

// ====== INLINEZ ======


template<typename T>
inline T TileSelectable::_getTileGroupProperty(const std::function<T (uint16_t index)> operation, const T defaultValue) const {
    T cumVal;
    auto it = _tileIndices.begin();

    for(; *it == 0 && it != _tileIndices.end(); ++it);

    if(it != _tileIndices.end()) {
        cumVal = operation(*it);
    } else {
        return defaultValue;
    }

    for(++it; it != _tileIndices.end(); ++it) {
        if(*it == 0) continue;
        if(operation(*it) != cumVal) return defaultValue;
    }
    return cumVal;
}

template <typename T>
inline TileSelectable::TileSelectableUndoItem<T>::TileSelectableUndoItem(const QVector<uint16_t>& indices,
                                                                      const T newVal,
                                                                       const std::function<T (uint16_t)>& get,
                                                                       const std::function<void (uint16_t, T)>& set,
                                                                       ESTkStandardItem*const item,
                                                                       const UndoItem::TYPE undoType):
    UndoItem(undoType), _item(item), _getAccessor(get), _setAccessor(set)
{
    for(auto it = indices.begin(); it != indices.end(); ++it) {
        _newValues[*it] = newVal;
    }

    for(auto it = indices.begin(); it != indices.end(); ++it) {
        _prevValues[*it] = get(*it);
    }

}


template <typename T>
void TileSelectable::TileSelectableUndoItem<T>::undo(void) {
    for(auto it = _prevValues.begin(); it != _prevValues.end(); ++it) {
        if(it.key() != 0) _setAccessor(it.key(), it.value());
    }
    if(_item) _item->forceUpdate();
}


template <typename T>
void TileSelectable::TileSelectableUndoItem<T>::redo(void) {
    for(auto it = _newValues.begin(); it != _newValues.end(); ++it) {
        if(it.key() != 0) _setAccessor(it.key(), it.value());
    }
    if(_item) _item->forceUpdate();
}






#endif // TILESELECTABLE_H
