#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

#include <ESToolkit/core/tk_file_utilities.h>

QString FileUtilities::_assetPath;

void FileUtilities::setAssetPath(const QString path) {
    _assetPath = path;
    _assetPath.replace("\\", "/");
}

bool FileUtilities::removeDir(const QString dirName) {
	bool result = true;
		QDir dir(dirName);

		if (dir.exists(dirName)) {
			Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
				if (info.isDir()) {
                    result = removeDir(info.absoluteFilePath());
				}
				else {
					result = QFile::remove(info.absoluteFilePath());
				}

				if (!result) {
					return result;
				}
			}
			result = dir.rmdir(dirName);
        } else result = false;
		return result;
}

//https://qt.gitorious.org/qt-creator/qt-creator/source/1a37da73abb60ad06b7e33983ca51b266be5910e:src/app/main.cpp#L13-189
// taken from utils/fileutils.cpp. We can not use utils here since that depends app_version.h.
bool FileUtilities::copyDir(const QString &srcFilePath,
                            const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
            return false;
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        Q_FOREACH (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyDir(newSrcFilePath, newTgtFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}

QString FileUtilities::getAssetRelativePath(const QString path) {

    QString linuxPath = path;
    linuxPath.replace("\\", "/");

    bool found = false;
    QString result;

#if 1
    QStringList list = linuxPath.split("/");

    for(int i = 0; i < list.size(); ++i) {
        if(list[i] == "assets") {
            found = true;
        }

        if(found) {
            result += "/";
            result += list[i];
        }
    }

    return found? result : QString("NOT AN ASSET RELATIVE PATH");

#else

    result = linuxPath;
    result.replace(_assetPath, "/assets");

    return result;


#endif
}

QString FileUtilities::getAbsoluteAssetPath(const QString path) {


    QString relPath = path;
    QString absPath;
    // = "FUCKING REESTED PATH!!!";
    relPath.replace("\\", "/");
#if 1

    QStringList list = relPath.split("/");
    if(list.size() >= 2 && list[1] == "assets") {
        absPath = _assetPath;

        for(unsigned i = 2; i < (unsigned)list.size(); ++i) {
            absPath += "/";
            absPath += list[i];
        }
        return absPath;
    } else { //not even an asset-relative path to begin with...

        return path;
    }

#else
    relPath.replace("/assets", _assetPath);
    return relPath;
#endif

}

QString FileUtilities::getLevelPathFromAreaPath(const QString areaPath) {
    return areaPath.split("/area/")[0];
}
