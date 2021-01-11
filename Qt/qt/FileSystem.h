#pragma once
#include <QObject>
#include <filesystem>

namespace qt {

class FileSystem : public QObject
{
    Q_OBJECT
public:
    FileSystem(QObject *parent = nullptr): QObject(parent) {}

    enum class AccessType
    {
        Read,
        Write
    };
    Q_ENUM(AccessType)

    static std::filesystem::path appDataDir();

public slots:
    static bool requestPermission(AccessType);
};

}    // namespace qt
