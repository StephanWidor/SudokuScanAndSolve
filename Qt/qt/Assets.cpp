#include "qt/Assets.h"
#include "qt/FileSystem.h"
#include <QDirIterator>
#include <QFile>

namespace {

std::filesystem::path initTrainData()
{
    const auto trainDataPath = qt::FileSystem::appDataDir() / "trainData/";
    if (!std::filesystem::exists(trainDataPath) && !std::filesystem::create_directory(trainDataPath))
        return {};
    QDirIterator trainDataIt(":/trainData");
    while (trainDataIt.hasNext())
    {
        const auto src = trainDataIt.next();
        const auto dst = QString::fromStdString(trainDataPath.string()) + trainDataIt.fileName();
        if (!QFile::exists(dst))
            QFile::copy(src, dst);
    }
    return trainDataPath;
}

}    // namespace

qt::Assets::Assets(): m_trainDataPath(initTrainData())
{}
