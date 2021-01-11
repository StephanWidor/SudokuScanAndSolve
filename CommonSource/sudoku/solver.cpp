#include "sudoku/solver.h"
#include <algorithm>
#include <cassert>
#include <iostream>

namespace {

std::pair<size_t, sudoku::Value> bestNewValue(const sudoku::deduction::Data &data)
{
    std::pair<std::pair<size_t, sudoku::Value>, std::pair<size_t, size_t>> best{{81, 0}, {10, 10}};
    for (size_t cellIndex = 0; cellIndex < 81; ++cellIndex)
    {
        const auto &possibleValues = data.possibleValues()[cellIndex];
        const auto cellNumPossible = possibleValues.numPossible();
        if (cellNumPossible < 2)
            continue;
        const auto relevantContainers =
          sudoku::relevantContainers(sudoku::row(cellIndex), sudoku::col(cellIndex), data.considerDiagonals());
        for (sudoku::Value value = 1; value < 10; ++value)
        {
            if (!possibleValues[value])
                continue;
            for (const auto container : relevantContainers)
            {
                const auto containerNumPossible = data.possiblePositions()[value].numPossible(*container);
                if (containerNumPossible < 2)
                    continue;
                const auto minMaxPossible = std::minmax(cellNumPossible, containerNumPossible);
                if (minMaxPossible.first < best.second.first ||
                    (minMaxPossible.first == best.second.first && minMaxPossible.second < best.second.second))
                {
                    best = {{cellIndex, value}, minMaxPossible};
                }
            }
        }
    }
    return best.first;
}

std::vector<sudoku::Sheet> solve(std::vector<sudoku::deduction::Data> &deductionStack, size_t maxNum)
{
    std::vector<sudoku::Sheet> solutions;
    solutions.reserve(maxNum);
    deductionStack.reserve(81);
    while (!deductionStack.empty() && solutions.size() < maxNum)
    {
        auto &current = deductionStack.back();
        if (!current.valid())
        {
            deductionStack.pop_back();
            continue;
        }
        if (sudoku::allEdited(current.deducedSheet()))
        {
            solutions.push_back(current.deducedSheet());
            deductionStack.pop_back();
            continue;
        };
        const auto [cellIndex, value] = bestNewValue(current);
        deductionStack.emplace_back(current).update({{sudoku::deduction::Entry::Set, cellIndex, value}});
        current.update({{sudoku::deduction::Entry::Forbid, cellIndex, value}});
    }
    return solutions;
}

}    // namespace

std::vector<sudoku::Sheet> sudoku::solve(const Sheet &sheet, bool considerDiagonals, size_t maxNum)
{
    std::vector<sudoku::deduction::Data> deductionStack;
    deductionStack.reserve(81);
    deductionStack.emplace_back(considerDiagonals).update(sudoku::deduction::entries(sheet));
    return ::solve(deductionStack, maxNum);
}

std::vector<sudoku::Sheet> sudoku::solve(const deduction::Data &data, const deduction::Entry &newEntry, size_t maxNum)
{
    std::vector<sudoku::deduction::Data> deductionStack;
    deductionStack.reserve(81);
    deductionStack.emplace_back(data).update({newEntry});
    return ::solve(deductionStack, maxNum);
}

bool sudoku::solveCell(Sheet &io_sheet, size_t cellIndex, bool considerDiagonals)
{
    auto solutions = solve(io_sheet, considerDiagonals, 1);
    if (solutions.empty())
        return false;
    io_sheet[cellIndex] = solutions.front()[cellIndex];
    return true;
}

bool sudoku::solveContainer(Sheet &io_sheet, const IndexContainer &container, bool considerDiagonals)
{
    auto solutions = solve(io_sheet, considerDiagonals, 1);
    if (solutions.empty())
        return false;
    for (const auto cellIndex : container)
        io_sheet[cellIndex] = solutions.front()[cellIndex];
    return true;
}

bool sudoku::solveValueInContainer(Sheet &io_sheet, Value value, const IndexContainer &container,
                                   bool considerDiagonals)
{
    auto solutions = solve(io_sheet, considerDiagonals, 1);
    if (solutions.empty())
        return false;
    const auto &solution = solutions.front();
    const auto cellIndex = *std::find_if(container.begin(), container.end(),
                                         [&solution, value](size_t cellIndex) { return solution[cellIndex] == value; });
    io_sheet[cellIndex] = solution[cellIndex];
    return true;
}

sudoku::SolutionState sudoku::solutionState(const sudoku::Sheet &sheet, bool considerDiagonals)
{
    if (sudoku::empty(sheet))
        return SolutionState::Empty;
    const auto solutions = sudoku::solve(sheet, considerDiagonals, 2);
    if (solutions.empty())
        return SolutionState::NoSolution;
    if (solutions.size() == 2)
        return SolutionState::NotUnique;
    if (sudoku::allEdited(sheet))
        return SolutionState::Solved;
    return SolutionState::Unique;
}
