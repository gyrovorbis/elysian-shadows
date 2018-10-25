#ifndef GYRO_VMU_QT_UTIL_HPP
#define GYRO_VMU_QT_UTIL_HPP

#include <evmu-core/gyro_vmu_vms.h>

#include <QPixmap>
#include <QMap>
#include <QStringList>

#define VMU_TEXT_CODEC                  "Shift-JIS"

struct VMSFileInfo;

class VmuQtUtils {

private:
    static QStringList       _supportedFileExtensions;
public:

    static QTextCodec*      japCodec;
    static bool             initialize(void);
    static bool             canOpenFile(QString filePath);
    static bool             loadVMUImage(VMUDevice* device, QString filePath, VMUFlashDirEntry** entry=nullptr, bool reloadGame=false);
};

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

    static QPixmap getIcon(VMUDevice* dev, const VMUFlashDirEntry* dirEntry);
    static bool exportAsGIF(const VMUFlashDirEntry* dirEntry, QString filename);
};

class VmuEyecatchCache {
public:
    static QMap<QString, QPixmap>  _eyecatchCache;

    static QPixmap loadPixmap(VMUDevice* dev, const VMUFlashDirEntry* dirEntry);

    static QPixmap getIcon(VMUDevice* dev, const VMUFlashDirEntry* dirEntry);

    static bool exportAsPNG(const VMUFlashDirEntry* dirEntry, QString filename);

};

#endif // GYRO_VMU_QT_UTIL_HPP
