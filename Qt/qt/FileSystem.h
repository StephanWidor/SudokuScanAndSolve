#pragma once
#include <filesystem>

namespace qt {

std::filesystem::path appDataDir();

inline std::filesystem::path stateFilePath()
{
    return appDataDir() / "state.yaml";
}

const std::filesystem::path &provideTrainDataDir();

}    // namespace qt
