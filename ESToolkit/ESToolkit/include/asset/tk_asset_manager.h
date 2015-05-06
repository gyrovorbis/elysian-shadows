#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <tk_selectable.h>
#include <tk_assets.h>

class TkLevel;
typedef TkLevel Level;

class Assets;
class ESToolkit;
class Selectable;
class Project;
class TkArea;
typedef TkArea Area;

class AssetManager {
friend class ESToolkit;
private:
    static Assets *_assets;

protected:
    Assets *getAssets(void) const { return _assets; }

    Selectable *getCurrentSelection(void) const { return _assets->getCurrentSelection(); }

    void setCurrentSelection(Selectable *const selection) { return _assets->setCurrentSelection(selection); }

    Selectable::TYPE getCurrentSelectionType(void) const { return _assets->getCurrentSelectionType(); }

    Area *getCurrentArea(void) const { return _assets->getCurrentArea(); }

    Level *getCurrentLevel(void) const { return _assets->getCurrentLevel(); }

    Project *getCurrentProject(void) const { return _assets->getCurrentProject(); }
};



#endif // ASSETMANAGER_H
