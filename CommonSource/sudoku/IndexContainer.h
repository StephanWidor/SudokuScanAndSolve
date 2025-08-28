#pragma once
#include "sudoku/Sheet.h"
#include <functional>
#include <vector>

namespace sudoku {

struct IndexContainer
{
    constexpr size_t operator[](size_t i) const { return cells[i]; }
    auto begin() const { return cells.begin(); }
    auto end() const { return cells.end(); }
    std::array<size_t, 9> cells;

    static const IndexContainer &row(size_t row);
    static const IndexContainer &col(size_t col);
    static const IndexContainer &box(size_t row, size_t col);
    static const IndexContainer &diagonal(size_t diagonalIndex);
};

inline bool empty(const Sheet &sheet, const IndexContainer &container)
{
    return std::all_of(container.begin(), container.end(),
                       [&sheet](size_t cellIndex) { return sheet[cellIndex] == 0u; });
}

inline bool allEdited(const Sheet &sheet, const IndexContainer &container)
{
    return std::all_of(container.begin(), container.end(),
                       [&sheet](size_t cellIndex) { return sheet[cellIndex] != 0u; });
}

inline void clear(Sheet &io_sheet, const IndexContainer &container)
{
    for (const auto cellIndex : container)
        io_sheet[cellIndex] = 0;
}

bool forAllContainers(std::function<bool(const IndexContainer &)> function, bool doDiagonals);

std::vector<const IndexContainer *> relevantContainers(size_t row, size_t col, bool considerDiaonals);

}    // namespace sudoku
