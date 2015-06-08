#include <QDir>
#include <common/turfs.h>
#include <gyro_file_api.h>
#include <gyro_system_api.h>


#define GY_FILE_QT_DEBUG_DIR    "ESTkLogs"

extern "C" int gyFileInit(const GYFileInitParams*const params) {
    int retVal = 0;

    QString homeDirPath = (params && params->workingDir)? params->workingDir : QDir::currentPath();

    if(!QDir::setCurrent(homeDirPath)) {
        _gyLog(GY_DEBUG_FATAL, "Could not cd into working directory directory: %s", homeDirPath.toStdString().c_str());
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

    if(!QDir::setCurrent(homeDirPath)) {
        _gyLog(GY_DEBUG_FATAL, "Could not cd BACK into working directory directory: %s", homeDirPath.toStdString().c_str());
    }

    _gyLog(GY_DEBUG_CRITICAL, "CURRENT - %s", QDir::currentPath().toStdString().c_str());

    return turfs_init("ESTeam", "ESTk", NULL) == TURFS_RET_OK && retVal;
}
