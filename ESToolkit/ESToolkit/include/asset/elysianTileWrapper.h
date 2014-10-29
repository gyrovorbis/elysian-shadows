#ifndef ELYSIANTILEWRAPPER_H
#define ELYSIANTILEWRAPPER_H

#include <QIcon>
#include "elysian_tile.hpp"
#include "assetManager.h"

class TkLevel;
typedef TkLevel Level;

/*
  We have to do this to hook elysian::Tile to PropertyStandardItem and add attributes
  to them that do not exist at the AssetIO level.
*/
class ElysianTileWrapper: public AssetManager {
public:
    typedef elysian::Tile::COLLIDER_TYPE COLLIDER_TYPE;

private:
    static QIcon **_icons;
    static const char _colliderStringFromEnum[elysian::Tile::COLLIDER_MAX][100];
    static const char _colliderIconNameFromEnum[elysian::Tile::COLLIDER_MAX][100];

    elysian::Tile*          _elysianTile = nullptr;
    unsigned int            _index = 0;
    Level*                  _level = nullptr;
public:

    void                    initialize(Level* lev, elysian::Tile* tile, const unsigned index);
    elysian::Tile*          getElysianTile(void) const;

    QString                 getColliderString(void) const;
    void                    setColliderString(const QString string);
    bool                    getNormalMapEnabled(void) const;
    bool                    getSpecularMapEnabled(void) const;
    void                    setNormalMapEnabled(const bool val);
    void                    setSpecularMapEnabled(const bool val);
    QIcon                   updateColliderIcon(const QString string) const;

    static COLLIDER_TYPE    getColliderEnumFromString(const QString string);
    static QString          getColliderStringFromEnum(const COLLIDER_TYPE type);
    static QIcon            getColliderIconFromEnum(const COLLIDER_TYPE type);
    static QIcon            getColliderIconFromString(const QString string);
    static void             allocIcons(void);
};

//======== INLINEZ =======


inline QString ElysianTileWrapper::getColliderString(void) const {
    return getColliderStringFromEnum(_elysianTile->colliderType);
}


inline QIcon ElysianTileWrapper::updateColliderIcon(const QString string) const {
    return getColliderIconFromString(string);
}


inline ElysianTileWrapper::COLLIDER_TYPE ElysianTileWrapper::getColliderEnumFromString(const QString string) {
    for(unsigned i = 0; i < elysian::Tile::COLLIDER_MAX; ++i) {
        if(strcmp(_colliderStringFromEnum[i], string.toStdString().c_str()) == 0) {
            return (elysian::Tile::COLLIDER_TYPE)i;
        }
    }
    Q_ASSERT(0); //Wasn't a valid collider name, dumbass.
    return elysian::Tile::COLLIDER_NONE;
}

inline QString ElysianTileWrapper::getColliderStringFromEnum(const COLLIDER_TYPE type) {
    return _colliderStringFromEnum[type];
}

inline QIcon ElysianTileWrapper::getColliderIconFromEnum(const COLLIDER_TYPE type) {
    return *_icons[type];
}

inline QIcon ElysianTileWrapper::getColliderIconFromString(const QString string) {
    return getColliderIconFromEnum(getColliderEnumFromString(string));
}

inline void ElysianTileWrapper::initialize(Level* lev, elysian::Tile* tile, const unsigned index) {
    _level = lev;
    _elysianTile = tile;
    _index = index;
}


#endif // ELYSIANTILEWRAPPER_H
