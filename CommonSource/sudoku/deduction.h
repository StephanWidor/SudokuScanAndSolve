#pragma once
#include "base/utils.h"
#include "sudoku/IndexContainer.h"
#include "sudoku/Sheet.h"

namespace sudoku::deduction {

struct PossibleValues
{
    bool operator[](sudoku::Value value) const { return possibleValues[value - 1]; }
    bool &operator[](sudoku::Value value) { return possibleValues[value - 1]; }

    sudoku::Value firstPossible() const
    {
        return static_cast<sudoku::Value>(
          std::distance(possibleValues.begin(), std::find(possibleValues.begin(), possibleValues.end(), true)) + 1);
    }

    size_t numPossible() const { return std::count(possibleValues.begin(), possibleValues.end(), true); }

private:
    std::array<bool, 9> possibleValues = utils::makeFilledArray<9>(true);
};

using SheetPossibleValues = std::array<PossibleValues, 81>;

struct PossiblePositions
{
    bool operator[](size_t cellIndex) const { return possiblePositions[cellIndex]; }
    bool &operator[](size_t cellIndex) { return possiblePositions[cellIndex]; }

    size_t numPossible(const sudoku::IndexContainer &container) const
    {
        return std::count_if(container.begin(), container.end(),
                             [this](size_t cellIndex) { return possiblePositions[cellIndex]; });
    }

    /// Only call when you know there is a possible position!
    size_t firstPossible(const sudoku::IndexContainer &container) const
    {
        return *std::find_if(container.begin(), container.end(),
                             [this](size_t cellIndex) { return possiblePositions[cellIndex]; });
    }

private:
    std::array<bool, 81> possiblePositions = utils::makeFilledArray<81>(true);
};

struct SheetPossiblePositions
{
    constexpr PossiblePositions &operator[](sudoku::Value value) { return possiblePositions[value - 1]; }
    constexpr const PossiblePositions &operator[](sudoku::Value value) const { return possiblePositions[value - 1]; }

private:
    std::array<PossiblePositions, 9> possiblePositions;
};

struct Entry
{
    enum
    {
        Set,
        Forbid
    } action;
    size_t cellIndex{0};
    sudoku::Value value{0};
};
using Entries = std::vector<Entry>;

Entries entries(const sudoku::Sheet &sheet);

class Data
{
public:
    Data(bool considerDiagonals): m_considerDiagonals(considerDiagonals) {}

    bool update(Entries newEntries);

    bool valid() const { return m_valid; }

    bool considerDiagonals() const { return m_considerDiagonals; }

    const sudoku::Sheet &deducedSheet() const { return m_sheet; }

    const SheetPossibleValues &possibleValues() const { return m_possibleValues; }

    const SheetPossiblePositions &possiblePositions() const { return m_possiblePositions; }

private:
    bool m_valid{true};
    bool m_considerDiagonals{false};
    sudoku::Sheet m_sheet;
    SheetPossibleValues m_possibleValues;
    SheetPossiblePositions m_possiblePositions;
};

}    // namespace sudoku::deduction
