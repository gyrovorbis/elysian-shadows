#ifndef TK_LEVEL_SYSTEM_H
#define TK_LEVEL_SYSTEM_H

#include <system/elysian_level_system_base.hpp>
#include <asset/tk_asset_manager.h>

ELYSIAN_ASSET_TYPEDEF(Level)
ELYSIAN_ASSET_TYPEDEF(Area)

class TkLevelSystem:
    public QObject,
    public elysian::LevelSystemBase,
    public AssetManager
{
    Q_OBJECT
public:
            TkLevelSystem(void);

    bool    switchLevel(const char *const dir);
    bool    switchArea(const char *const dir);

Q_SIGNALS:

    void    areaChanged(Area* area);
    void    levelChanged(Level* level);

};

#endif // TK_LEVEL_SYSTEM_H
