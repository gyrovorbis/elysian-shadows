#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

#include "fileUtilities.h"

bool FileUtilities::RemoveDir(const QString dirName) {
	bool result = true;
		QDir dir(dirName);

		if (dir.exists(dirName)) {
			Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
				if (info.isDir()) {
					result = RemoveDir(info.absoluteFilePath());
				}
				else {
					result = QFile::remove(info.absoluteFilePath());
				}

				if (!result) {
					return result;
				}
			}
			result = dir.rmdir(dirName);
		}
		return result;
}
