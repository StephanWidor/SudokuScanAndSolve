#include "qt/FileSystem.h"
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>

namespace qt {

std::filesystem::path appDataDir()
{
    auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    if (!std::filesystem::exists(dir))
        std::filesystem::create_directories(dir);
    return dir;
}

static std::filesystem::path trainDataDir;

const std::filesystem::path &provideTrainDataDir()
{
    if (trainDataDir.empty())
    {
        if (auto newTrainDataDir = appDataDir() / "trainData";
            std::filesystem::exists(newTrainDataDir) || std::filesystem::create_directory(newTrainDataDir))
        {
            trainDataDir = std::move(newTrainDataDir);
            QDirIterator trainDataIt(":/trainData");
            while (trainDataIt.hasNext())
            {
                const auto src = trainDataIt.next();
                const auto dst = QString::fromStdString((trainDataDir / trainDataIt.fileName().toStdString()).string());
                if (!QFile::exists(dst))
                    QFile::copy(src, dst);
            }
        }
    }
    return trainDataDir;
}

}    // namespace qt
