#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QString>

class FileUtilities {
public:

    static bool removeDir(const QString dirName);
    static bool copyDir(const QString &srcFilePath, const QString &tgtFilePath);

};


#endif // FILEUTILS_H
