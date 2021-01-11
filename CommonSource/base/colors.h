#pragma once
#include <array>
#include <cstdint>

namespace colors {

/// 32bit rgba color
struct Color
{
    constexpr uint8_t operator[](size_t i) const { return coordinates[i]; }
    uint8_t &operator[](size_t i) { return coordinates[i]; }

    std::array<uint8_t, 4> coordinates{255, 255, 255, 255};
};

constexpr Color transparent()
{
    return {{0, 0, 0, 0}};
}
constexpr Color white()
{
    return {{255, 255, 255, 255}};
}
constexpr Color lightGray()
{
    return {{217, 217, 217, 255}};
}
constexpr Color darkGray()
{
    return {{55, 55, 55, 255}};
}
constexpr Color orange()
{
    return {{204, 66, 0, 255}};
}

constexpr Color red()
{
    return {{255, 0, 0, 255}};
}

constexpr Color green()
{
    return {{0, 178, 0, 255}};
}

}    // namespace colors
