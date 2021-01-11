#pragma once
#include <filesystem>

namespace qt {

class Assets
{
public:
    Assets();

    const std::filesystem::path &trainDataPath() const { return m_trainDataPath; }

private:
    const std::filesystem::path m_trainDataPath;
};

}    // namespace qt
