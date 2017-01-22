#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QString>

class FileUtilities {
private:

    static QString  _assetPath;


public:
    static QString  getAssetPath(void);
    static QString  getLevelPathFromAreaPath(const QString areaPath);
    static void     setAssetPath(const QString path);
    static QString  getAssetRelativePath(const QString path);
    static QString  getAbsoluteAssetPath(const QString path);
    static bool     removeDir(const QString dirName);
    static bool     copyDir(const QString &srcFilePath, const QString &tgtFilePath);

};



// ===== INLINEZ ======

inline QString FileUtilities::getAssetPath(void) { return _assetPath; }

#endif // FILEUTILS_H
