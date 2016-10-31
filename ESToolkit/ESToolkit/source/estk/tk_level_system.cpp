#include <estk/tk_level_system.h>

TkLevelSystem::TkLevelSystem(void) {
    connect(this, SIGNAL(areaChanged(Area*)), getAssets(), SLOT(onAreaChanged(Area*)));
    connect(this, SIGNAL(levelChanged(Level*)), getAssets(), SLOT(onLevelChanged(Level*)));
}

bool TkLevelSystem::switchArea(const char * const dir) {
    const auto success = elysian::LevelSystemBase::switchArea(dir);
    Q_EMIT areaChanged(_curArea);
    return success;
}

bool TkLevelSystem::switchLevel(const char * const dir) {
    const auto success = elysian::LevelSystemBase::switchLevel(dir);
    Q_EMIT levelChanged(_curLevel);
    return success;
}
