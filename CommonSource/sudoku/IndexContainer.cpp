#include "sudoku/IndexContainer.h"
#include "base/utils.h"

namespace {

constexpr sudoku::IndexContainer makeRowContainer(size_t row)
{
    return {utils::makeArray<9>([row](size_t col) { return sudoku::cellIndex(row, col); })};
}

constexpr sudoku::IndexContainer makeColContainer(size_t col)
{
    return {utils::makeArray<9>([col](size_t row) { return sudoku::cellIndex(row, col); })};
}

constexpr sudoku::IndexContainer makeBoxContainer(size_t boxIndex)
{
    const auto startRow = 3 * (boxIndex / 3);
    const auto startCol = 3 * (boxIndex % 3);
    return {utils::makeArray<9>(
      [startRow, startCol](size_t i) { return sudoku::cellIndex(startRow + i / 3, startCol + i % 3); })};
}

constexpr sudoku::IndexContainer makeFirstDiagonalContainer()
{
    return {utils::makeArray<9>([](size_t i) { return sudoku::cellIndex(i, i); })};
}

constexpr sudoku::IndexContainer makeSecondDiagonalContainer()
{
    return {utils::makeArray<9>([](size_t i) { return sudoku::cellIndex(i, 8u - i); })};
}

constexpr std::array<sudoku::IndexContainer, 9> makeContainers(sudoku::IndexContainer (*makeContainer)(size_t i))
{
    return utils::makeArray<9>([makeContainer](size_t i) { return makeContainer(i); });
}

constexpr std::array<sudoku::IndexContainer, 2> makeDiagonalContainers()
{
    return {makeFirstDiagonalContainer(), makeSecondDiagonalContainer()};
}

constexpr const struct
{
    std::array<sudoku::IndexContainer, 9> rows = makeContainers(&makeRowContainer);
    std::array<sudoku::IndexContainer, 9> cols = makeContainers(&makeColContainer);
    std::array<sudoku::IndexContainer, 9> boxes = makeContainers(&makeBoxContainer);
    std::array<sudoku::IndexContainer, 2> diagonals = makeDiagonalContainers();
} allContainers;

}    // namespace

const sudoku::IndexContainer &sudoku::IndexContainer::row(size_t row)
{
    return allContainers.rows[row];
}
const sudoku::IndexContainer &sudoku::IndexContainer::col(size_t col)
{
    return allContainers.cols[col];
}
const sudoku::IndexContainer &sudoku::IndexContainer::box(size_t row, size_t col)
{
    return allContainers.boxes[boxIndex(row, col)];
}
const sudoku::IndexContainer &sudoku::IndexContainer::diagonal(size_t diagonalIndex)
{
    return allContainers.diagonals[diagonalIndex];
}

bool sudoku::forAllContainers(std::function<bool(const IndexContainer &)> function, bool doDiagonals)
{
    for (auto &container : allContainers.rows)
    {
        if (!function(container))
            return false;
    }
    for (auto &container : allContainers.cols)
    {
        if (!function(container))
            return false;
    }
    for (auto &container : allContainers.boxes)
    {
        if (!function(container))
            return false;
    }
    if (doDiagonals)
    {
        for (auto &container : allContainers.diagonals)
        {
            if (!function(container))
                return false;
        }
    }
    return true;
}

std::vector<const sudoku::IndexContainer *> sudoku::relevantContainers(size_t row, size_t col, bool considerDiaonals)
{
    std::vector<const sudoku::IndexContainer *> relevantContainers{&allContainers.rows[row], &allContainers.cols[col],
                                                                   &allContainers.boxes[sudoku::boxIndex(row, col)],
                                                                   nullptr, nullptr};
    relevantContainers.resize(3);
    if (considerDiaonals)
    {
        if (row == col)
            relevantContainers.push_back(&allContainers.diagonals.front());
        if (row == 8 - col)
            relevantContainers.push_back(&allContainers.diagonals.back());
    }
    return relevantContainers;
}
