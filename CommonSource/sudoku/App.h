#pragma once
#include "sudoku/History.h"
#include "sudoku/IndexContainer.h"
#include "sudoku/Sheet.h"
#include "sudoku/solver.h"
#include <filesystem>
#include <optional>
#include <variant>

namespace sudoku {

class App
{
public:
    using CellSelection = std::variant<std::monostate, size_t, sudoku::IndexContainer>;
    using ValueSelection = std::optional<sudoku::Value>;

    const Sheet &sheet() const { return m_sheet; }

    void setSheet(const sudoku::Sheet &sheet);

    bool cellSelected(size_t cellIndex) const;

    bool valueSelected(size_t cellIndex) const;

    void toggleSelection(size_t row, size_t col);

    void select(const IndexContainer &container) { m_cellSelection = container; }

    /// no cell/container selected  -> select/highlight value
    /// cell selected               -> set cell value
    /// container selected          -> set/solve value in container
    void select(sudoku::Value value);

    void solveSelected();

    void clearSelected();

    bool hasHistory() const { return m_history.hasHistory(); }

    void undo();

    void clearHistory() { m_history.clear(); }

    bool diagonalsConsidered() const { return m_diagonalsConsidered; }

    void considerDiagonals(bool considerDiagonals);

    void clearPuzzle();

    void createPuzzle();

    void solvePuzzle();

    SolutionState solutionState() const { return m_solutionState; }

private:
    bool m_diagonalsConsidered{false};
    SolutionState m_solutionState{SolutionState::Empty};
    sudoku::Sheet m_sheet{};
    CellSelection m_cellSelection{std::monostate{}};
    ValueSelection m_valueSelection;
    sudoku::History m_history{m_sheet};
};

bool saveState(const std::filesystem::path &path, const sudoku::App &sudokuApp, bool outputProcessingImg);

bool loadState(const std::filesystem::path &path, sudoku::App &o_sudokuApp,
               std::function<void(bool)> outputProcessingImgSetter);

}    // namespace sudoku
