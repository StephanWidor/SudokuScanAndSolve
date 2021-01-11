#include "qt/FileSystem.h"
#include <QStandardPaths>
#ifdef ANDROID
#include <QtAndroid>
#endif

bool qt::FileSystem::requestPermission([[maybe_unused]] AccessType type)
{
#ifdef ANDROID
    QString permissionString = type == AccessType::Read ? "android.permission.READ_EXTERNAL_STORAGE" :
                                                          "android.permission.WRITE_EXTERNAL_STORAGE";
    return QtAndroid::checkPermission(permissionString) == QtAndroid::PermissionResult::Granted ||
           QtAndroid::requestPermissionsSync({permissionString}).value(permissionString) ==
             QtAndroid::PermissionResult::Granted;
#endif
    return true;
}

std::filesystem::path qt::FileSystem::appDataDir()
{
    std::filesystem::path appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    if (std::filesystem::exists(appDataPath) || std::filesystem::create_directory(appDataPath))
        return appDataPath;
    return {};
}
