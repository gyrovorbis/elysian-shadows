#include <QDir>
#include <common/turfs.h>
#include <gyro_file_api.h>
#include <gyro_system_api.h>


#define GY_FILE_QT_DEBUG_DIR    "ESTkLogs"

extern "C" int gyFileInit(const char *author, const char *name)
{
    int retVal = 0;
    if (author == NULL || name == NULL) {
        /* Require an app author & name to identify us */
        return 0;
    }

    QString homeDirPath = QDir::homePath();

    if(!QDir::setCurrent(homeDirPath)) {
        _gyLog(GY_DEBUG_FATAL, "Could not cd into user directory: %s", homeDirPath.toStdString().c_str());
    } else {

        if(QDir(GY_FILE_QT_DEBUG_DIR).exists() || QDir().mkdir(GY_FILE_QT_DEBUG_DIR)) {
            QDir debugDir(GY_FILE_QT_DEBUG_DIR);
            if(debugDir.exists() && QDir::setCurrent(debugDir.path())) {
                _gyLog(GY_DEBUG_VERBOSE, "Creating log files at %s", debugDir.absolutePath().toStdString().c_str());
                retVal = 1;
            } else {
                _gyLog(GY_DEBUG_FATAL, "Could not cd into debug log file directory: %s", debugDir.path().toStdString().c_str());
            }

        } else {
            _gyLog(GY_DEBUG_FATAL, "Could not access debug log file directory: %s/%s", homeDirPath.toStdString().c_str(), GY_FILE_QT_DEBUG_DIR);
        }

    }

    return turfs_init(author, name, NULL) == TURFS_RET_OK && retVal;
}
