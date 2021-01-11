#pragma once
#include "base/utils.h"
#include <algorithm>
#include <filesystem>
#include <ostream>

namespace sudoku {

constexpr size_t cellIndex(size_t row, size_t col)
{
    return 9 * row + col;
}

constexpr size_t row(size_t cellIndex)
{
    return cellIndex / 9;
}

constexpr size_t col(size_t cellIndex)
{
    return cellIndex % 9;
}

constexpr size_t boxIndex(size_t row, size_t col)
{
    return 3 * (row / 3) + (col / 3);
}

using Value = uint8_t;

struct Sheet
{
    constexpr Value &operator[](size_t cellIndex) { return values[cellIndex]; }
    constexpr Value operator[](size_t cellIndex) const { return values[cellIndex]; }

    auto begin() { return values.begin(); }
    auto begin() const { return values.cbegin(); }
    auto end() { return values.end(); }
    auto end() const { return values.end(); }

    bool operator==(const Sheet &other) const { return values == other.values; }
    bool operator!=(const Sheet &other) const { return !operator==(other); }

    std::array<Value, 81> values = utils::makeFilledArray<81>(static_cast<Value>(0u));
};

inline bool empty(const Sheet &sheet)
{
    return std::all_of(sheet.begin(), sheet.end(), [](Value v) { return v == 0u; });
}

inline bool allEdited(const Sheet &sheet)
{
    return std::all_of(sheet.begin(), sheet.end(), [](Value v) { return v != 0u; });
}

/// \throw std::ios_base::failure
Sheet loadSheet(const std::filesystem::path &filePath);

bool saveSheet(const Sheet &sheet, const std::filesystem::path &filePath);

}    // namespace sudoku

std::ostream &operator<<(std::ostream &out, const sudoku::Sheet &sheet);
