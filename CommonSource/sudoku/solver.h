#pragma once
#include "sudoku/Sheet.h"
#include "sudoku/deduction.h"

namespace sudoku {

std::vector<Sheet> solve(const Sheet &, bool considerDiagonals, size_t maxNumSolutions = 1);

std::vector<Sheet> solve(const deduction::Data &, const deduction::Entry &, size_t maxNumSolutions = 1);

bool solveCell(Sheet &io_sheet, size_t cellIndex, bool considerDiagonals);

bool solveContainer(Sheet &io_sheet, const IndexContainer &, bool considerDiagonals);

bool solveValueInContainer(Sheet &io_sheet, Value, const IndexContainer &, bool considerDiagonals);

enum class SolutionState
{
    Empty,
    NoSolution,
    NotUnique,
    Unique,
    Solved
};

SolutionState solutionState(const sudoku::Sheet &, bool considerDiagonals);

}    // namespace sudoku
