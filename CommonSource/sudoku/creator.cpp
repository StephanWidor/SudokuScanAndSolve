#include "sudoku/creator.h"
#include "sudoku/solver.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <random>

namespace {

std::pair<size_t, sudoku::Value> randomNewValue(const sudoku::deduction::Data &data,
                                                std::default_random_engine &randomEngine)
{
    std::uniform_int_distribution<size_t> dist(0, 81);
    const auto cellIndex = [&, &sheet = data.deducedSheet()]() -> size_t {
        const auto numLeft = std::count(sheet.begin(), sheet.end(), 0);
        const size_t randomIndexOfLeft = dist(randomEngine) % numLeft;
        size_t count = 0;
        for (size_t cellIndex = 0; cellIndex < 81; ++cellIndex)
        {
            if (sheet[cellIndex] == 0)
            {
                if (count == randomIndexOfLeft)
                    return cellIndex;
                ++count;
            }
        }
        assert(false);    // we shouldn't get here!
        return 0;
    }();

    const auto value = [&, &possibleValues = data.possibleValues()[cellIndex]]() -> sudoku::Value {
        const size_t randomIndexOfPossible = dist(randomEngine) % (possibleValues.numPossible());
        size_t count = 0;
        for (sudoku::Value v = 1; v < 10; ++v)
        {
            if (possibleValues[v])
            {
                if (count == randomIndexOfPossible)
                    return v;
                ++count;
            }
        }
        assert(false);    // we shouldn't get here!
        return 0;
    }();
    return {cellIndex, value};
}

sudoku::Sheet removeSuperfluousEntries(const sudoku::Sheet &sheet, std::default_random_engine &randomEngine,
                                       bool considerDiagonals)
{
    auto indicesOfEntries = [&sheet]() {
        std::vector<size_t> indices;
        indices.reserve(81);
        for (size_t i = 0; i < 81; ++i)
        {
            if (sheet[i] != 0u)
                indices.push_back(i);
        }
        return indices;
    }();

    std::shuffle(indicesOfEntries.begin(), indicesOfEntries.end(), randomEngine);
    auto removeSheet = sheet;
    for (const auto cellIndex : indicesOfEntries)
    {
        removeSheet[cellIndex] = 0;
        if (sudoku::solve(removeSheet, considerDiagonals, 2).size() > 1)
            removeSheet[cellIndex] = sheet[cellIndex];
    }
    return removeSheet;
}

}    // namespace

sudoku::Sheet sudoku::create(bool considerDiagonals, bool removeSuperfluousEntries, std::optional<unsigned> seed)
{
    const auto effectiveSeed =
      seed ? *seed : static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine randomEngine(effectiveSeed);
    sudoku::Sheet sheet;
    sudoku::deduction::Data deductionData(considerDiagonals);
    while (deductionData.valid())
    {
        const auto [cellIndex, value] = randomNewValue(deductionData, randomEngine);
        const auto numSolutions = solve(deductionData, {deduction::Entry::Set, cellIndex, value}, 2).size();
        if (numSolutions == 0)
            deductionData.update({{deduction::Entry::Forbid, cellIndex, value}});
        else
        {
            sheet[cellIndex] = value;
            if (numSolutions == 1)
                break;
            deductionData.update({{deduction::Entry::Set, cellIndex, value}});
        }
    }
    if (removeSuperfluousEntries)
        return ::removeSuperfluousEntries(sheet, randomEngine, considerDiagonals);
    else
        return sheet;
}
