/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VMU_QT_UTIL_HPP
#define GYRO_VMU_QT_UTIL_HPP

#include <evmu-core/gyro_vmu_vms.h>
#include <evmu-core/gyro_vmu_flash.h>
#include <QESTkUtils/tk_debug_log_stack.h>

#include <QPixmap>
#include <QMap>
#include <QStringList>

#define VMU_TEXT_CODEC                  "Shift-JIS"

struct VMSFileInfo;
struct IconDataFileInfo;
struct VmuExtraBgPvrFileInfo;

class QtVmuFlashDirEntry;

namespace QESTkUtils {

template <typename Derived,
          typename Key,
          typename Entry,
          template<class K, class E> class Container>
class AssetBank {
private:
    Container<Key, const Entry*>   _cache;
protected:
    static const Entry*     createEntry(VMUDevice* device, VMUFlashDirEntry* entry);
    static Key              createKey(VMUDevice* device, const VMUFlashDirEntry* entry);

public:
                            ~AssetBank(void);
    const Entry*            findEntry(const Key& key) const;
    const Entry*            getEntry(VMUDevice* device, VMUFlashDirEntry* entry);
    bool                    removeEntry(VMUDevice* dev, const VMUFlashDirEntry* entry);
    bool                    removeEntry(const Key& key);

};

template <typename Derived,
          typename Key,
          typename Entry,
          template<class K, class E> class Container>
const Entry* AssetBank<Derived, Key, Entry, Container>::createEntry(VMUDevice* device, VMUFlashDirEntry* dirEntry) {
    return Derived::createEntry(device, dirEntry);
}

template <typename Derived,
          typename Key,
          typename Entry,
          template<class K, class E> class Container>
Key AssetBank<Derived, Key, Entry, Container>::createKey(VMUDevice* device, const VMUFlashDirEntry* dirEntry) {
    return Derived::createKey(device, dirEntry);
}

template <typename Derived,
          typename Key,
          typename Entry,
          template<class K, class E> class Container>
AssetBank<Derived, Key, Entry, Container>::~AssetBank(void) {
    for(auto* entry : _cache) {
        delete entry;
    }
}

template <typename Derived,
          typename Key,
          typename Entry,
          template<class K, class E> class Container>
bool AssetBank<Derived, Key, Entry, Container>::removeEntry(const Key& key) {
    return _cache.remove(key);
}

template <typename Derived,
          typename Key,
          typename Entry,
          template<class K, class E> class Container>
bool AssetBank<Derived, Key, Entry, Container>::removeEntry(VMUDevice* dev, const VMUFlashDirEntry* entry) {
    removeEntry(Derived::createKey(dev, entry));
}

template <typename Derived,
          typename Key,
          typename Entry,
          template<class K, class E> class Container>
const Entry* AssetBank<Derived, Key, Entry, Container>::getEntry(VMUDevice* dev, VMUFlashDirEntry* dirEntry) {
    Key key = static_cast<Derived*>(this)->createKey(dev, dirEntry);
    const Entry* entry = nullptr;
    const auto& it = _cache.find(key);

    if(it != _cache.end()) {
        entry = it.value();
    } else {
        entry = createEntry(dev, dirEntry);
        if(entry) _cache.insert(key, entry);
    }

    return entry;
}

template <typename Derived,
          typename Key,
          typename Entry,
          template<class K, class E> class Container>
const Entry* AssetBank<Derived, Key, Entry, Container>::findEntry(const Key& key) const {
    const auto& it = _cache.find(key);

    return (it != _cache.end())? it.value() : nullptr;
}


}



struct VmuAnimatedIconCacheEntry {
    int                 currentFrame;
    int                 animSpeed;
    qint64              lastMs = 0;
    uint16_t            palette[VMU_VMS_ICON_PALETTE_SIZE];

    QVector<QPixmap>    pixmaps;

    bool                exportAsGif(QString filename);
    QPixmap             getPixmap(void);
};

class VmuAnimatedIconCache {
public:
    QMap<QString, VmuAnimatedIconCacheEntry*>
        static _iconCache;

    static VmuAnimatedIconCacheEntry* loadPixmaps(VMUDevice* dev, const VMUFlashDirEntry* dirEntry);

    static QPixmap getIcon(VMUDevice* dev, const VMUFlashDirEntry* dirEntry, VmuAnimatedIconCacheEntry** newEntry=nullptr);
    static VmuAnimatedIconCacheEntry* getEntry(VMUDevice* dev, const VMUFlashDirEntry* entry);
    static bool exportAsGIF(const VMUFlashDirEntry* dirEntry, QString filename);
};

class VmuEyecatchCache {
public:
    static QMap<QString, QPixmap>  _eyecatchCache;

    static QPixmap loadPixmap(VMUDevice* dev, const VMUFlashDirEntry* dirEntry);

    static QPixmap getIcon(VMUDevice* dev, const VMUFlashDirEntry* dirEntry);

    static bool exportAsPNG(const VMUFlashDirEntry* dirEntry, QString filename);

};



struct VmuIconDataBankKey {
    const VMUFlashDirEntry* entry           = nullptr;
    uint16_t                fatBlock        = VMU_FLASH_BLOCK_FAT_UNALLOCATED;
    int                     dirEntryIndex   = -1;
    size_t                  size            = 0;

    bool operator < (const VmuIconDataBankKey& rhs) const;
};


struct VmuIconDataBankEntry {
    QPixmap vmuIcon;
    QPixmap dcIcon;

    bool exportVmuIcon(QString filepath) const;
    bool exportDcIcon(QString filepath) const;
};

class VmuIconDataImageBank:
        public QESTkUtils::AssetBank<VmuIconDataImageBank,
                                     VmuIconDataBankKey,
                                     VmuIconDataBankEntry,
                                     QMap>
{
private:
    static QImage          iconDataVmuIconImage(const VMUDevice* dev, const IconDataFileInfo* icnDat);
    static QImage          iconDataDcIconImage(const VMUDevice* dev, const IconDataFileInfo* icnDat);

public:
    static VmuIconDataBankKey     createKey(VMUDevice* device, const VMUFlashDirEntry* dirEntry);
    static VmuIconDataBankEntry*  createEntry(VMUDevice* device, const VMUFlashDirEntry* dirEntry);
};


struct VmuExtraBgPvrBankKey {
    const VMUFlashDirEntry*         entry      = nullptr;
    bool operator < (const VmuExtraBgPvrBankKey& rhs) const;
};


struct VmuExtraBgPvrBankEntry {
    QPixmap image;
    bool exportImage(QString filepath) const;
};

class VmuExtraBgPvrBank:
        public QESTkUtils::AssetBank<VmuExtraBgPvrBank,
                                     VmuExtraBgPvrBankKey,
                                     VmuExtraBgPvrBankEntry,
                                     QMap>
{
private:
    static QImage                   extraBgPvrImage(const VMUDevice* dev, const VmuExtraBgPvrFileInfo* info);

public:
    static VmuExtraBgPvrBankKey     createKey(VMUDevice* device, const VMUFlashDirEntry* dirEntry);
    static VmuExtraBgPvrBankEntry*  createEntry(VMUDevice* device, const VMUFlashDirEntry* dirEntry);
};



struct VmuPsoImgBankKey {
    const VMUFlashDirEntry*         entry      = nullptr;
    bool operator < (const VmuPsoImgBankKey& rhs) const;
};

struct VmuPsoImgBankEntry {
    QPixmap     image;
    QStringList characterNames;
    bool        exportImage(QString filepath) const;
};

class VmuPsoImgBank:
        public QESTkUtils::AssetBank<VmuPsoImgBank,
                                     VmuPsoImgBankKey,
                                     VmuPsoImgBankEntry,
                                     QMap>
{
private:
    static QImage               createPsoImgImage(const QtVmuFlashDirEntry& dirEntry, QStringList& charNamesList);

public:
    static VmuPsoImgBankKey     createKey(VMUDevice* device, const VMUFlashDirEntry* dirEntry);
    static VmuPsoImgBankEntry*  createEntry(VMUDevice* device, VMUFlashDirEntry* dirEntry);
};

class VmuQtUtils {

public:
    /*
     *  Yes, the fact I need this struct is truly fucking shameful.
     *
     */
    struct ReloadState {
        bool    reloading;
        QString vmsPath;
        QString vmiPath;
        bool    autogenerate;
                ReloadState(void);
    };
private:
    static QStringList      _supportedFileExtensions;
    static QString          _romDir;
    static QString          _defaultImageTypeFilter;

    static VmuIconDataImageBank
                            _iconDataBank;
    static VmuExtraBgPvrBank
                            _extraBgPvrBank;

    static VmuPsoImgBank    _psoImgBank;

public:

    static QTextCodec*      japCodec;
    static bool             initialize(void);
    static bool             canOpenFile(QString filePath);
    static QString          getRomDir(void);
    static void             setRomDir(QString dir);
    static VmuIconDataImageBank&
                            getIconDataBank(void);
    static VmuExtraBgPvrBank&
                            getExtraBgPvrBank(void);
    static VmuPsoImgBank&   getPsoImgBank(void);

    static bool             loadVMUImage(VMUDevice* device,
                                         QString filePath,
                                         VMUFlashDirEntry** entry=nullptr,
                                         bool replaceGame=false,
                                         ReloadState reloadInfo=ReloadState());

    static QString          getDefaultImageTypeFilter(void);




};


//======= INLINEZ ========






inline QString VmuQtUtils::getDefaultImageTypeFilter(void) { return _defaultImageTypeFilter; }
inline bool VmuIconDataBankKey::operator<(const VmuIconDataBankKey& rhs) const {
    return (entry < rhs.entry);
}
inline bool VmuExtraBgPvrBankKey::operator<(const VmuExtraBgPvrBankKey& rhs) const {
    return (entry < rhs.entry);
}
inline bool VmuPsoImgBankKey::operator<(const VmuPsoImgBankKey& rhs) const {
    return (entry < rhs.entry);
}
inline VmuIconDataImageBank& VmuQtUtils::getIconDataBank(void) { return _iconDataBank; }
inline VmuExtraBgPvrBank& VmuQtUtils::getExtraBgPvrBank(void) { return _extraBgPvrBank; }
inline VmuPsoImgBank& VmuQtUtils::getPsoImgBank(void) { return _psoImgBank; }
inline QString VmuQtUtils::getRomDir(void) { return _romDir; }
inline void VmuQtUtils::setRomDir(QString dir) { _romDir = std::move(dir); }
inline VmuQtUtils::ReloadState::ReloadState(void):
    reloading(false),
    autogenerate(false){}

#endif // GYRO_VMU_QT_UTIL_HPP
