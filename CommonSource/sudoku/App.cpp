#include "sudoku/App.h"
#include "base/utils.h"
#include "sudoku/creator.h"
#include "sudoku/solver.h"
#include <opencv2/core.hpp>

void sudoku::App::setSheet(const sudoku::Sheet &sheet)
{
    m_sheet = sheet;
    m_solutionState = sudoku::solutionState(m_sheet, m_diagonalsConsidered);
    m_history.push(m_sheet);
    m_cellSelection = std::monostate{};
    m_valueSelection = std::nullopt;
}

bool sudoku::App::cellSelected(size_t cellIndex) const
{
    return std::visit(utils::match{[&](size_t selectedIndex) { return selectedIndex == cellIndex; },
                                   [&](const sudoku::IndexContainer &container) {
                                       return std::find(container.begin(), container.end(), cellIndex) !=
                                              container.end();
                                   },
                                   [](const auto &) { return false; }},
                      m_cellSelection);
}

bool sudoku::App::valueSelected(size_t cellIndex) const
{
    return m_valueSelection != std::nullopt && m_sheet[cellIndex] == *m_valueSelection;
}

void sudoku::App::toggleSelection(size_t row, size_t col)
{
    const auto cellIndex = sudoku::cellIndex(row, col);
    if (cellSelected(cellIndex))
    {
        m_cellSelection = std::monostate{};
        m_valueSelection = std::nullopt;
    }
    else
    {
        m_cellSelection = cellIndex;
        if (m_sheet[cellIndex] != 0)
            m_valueSelection = m_sheet[cellIndex];
    }
}

void sudoku::App::select(sudoku::Value value)
{
    std::visit(utils::match{[&](size_t selectedIndex) {
                                m_valueSelection = std::nullopt;
                                m_sheet[selectedIndex] = value;
                                m_solutionState = sudoku::solutionState(m_sheet, m_diagonalsConsidered);
                            },
                            [&](const IndexContainer &container) {
                                m_valueSelection = std::nullopt;
                                sudoku::solveValueInContainer(m_sheet, value, container, m_diagonalsConsidered);
                            },
                            [&](const auto &) {
                                if (m_valueSelection.has_value() && value == *m_valueSelection)
                                    m_valueSelection = std::nullopt;
                                else
                                    m_valueSelection = value;
                            }},
               m_cellSelection);
    m_history.push(m_sheet);
    m_cellSelection = std::monostate{};
}

void sudoku::App::solveSelected()
{
    std::visit(
      utils::match{
        [&](size_t selectedIndex) { sudoku::solveCell(m_sheet, selectedIndex, m_diagonalsConsidered); },
        [&](const IndexContainer &container) { sudoku::solveContainer(m_sheet, container, m_diagonalsConsidered); },
        [&](const auto &) {}},
      m_cellSelection);
    m_solutionState = sudoku::solutionState(m_sheet, m_diagonalsConsidered);
    m_history.push(m_sheet);
    m_cellSelection = std::monostate{};
    m_valueSelection = std::nullopt;
}

void sudoku::App::clearSelected()
{
    std::visit(utils::match{[&](size_t selectedIndex) { m_sheet[selectedIndex] = 0; },
                            [&](const IndexContainer &container) { sudoku::clear(m_sheet, container); },
                            [&](const auto &) {}},
               m_cellSelection);
    m_solutionState = sudoku::solutionState(m_sheet, m_diagonalsConsidered);
    m_history.push(m_sheet);
    m_cellSelection = std::monostate{};
    m_valueSelection = std::nullopt;
}

void sudoku::App::undo()
{
    if (m_history.pop())
    {
        m_sheet = m_history.back();
        m_solutionState = sudoku::solutionState(m_sheet, m_diagonalsConsidered);
    }
    m_cellSelection = std::monostate{};
    m_valueSelection = std::nullopt;
}

void sudoku::App::clearPuzzle()
{
    m_sheet = sudoku::Sheet{};
    m_solutionState = SolutionState::Empty;
    m_history.push(m_sheet);
    m_cellSelection = std::monostate{};
    m_valueSelection = std::nullopt;
}

void sudoku::App::createPuzzle()
{
    m_sheet = sudoku::create(m_diagonalsConsidered, m_createDifficult);
    m_solutionState = SolutionState::Unique;
    m_history.push(m_sheet);
    m_cellSelection = std::monostate{};
    m_valueSelection = std::nullopt;
}

void sudoku::App::solvePuzzle()
{
    const auto solutions = sudoku::solve(m_sheet, diagonalsConsidered());
    if (!solutions.empty())
    {
        m_sheet = solutions.back();
        m_solutionState = SolutionState::Solved;
        m_history.push(m_sheet);
        m_cellSelection = std::monostate{};
        m_valueSelection = std::nullopt;
    }
}

void sudoku::App::considerDiagonals(bool considerDiagonals)
{
    m_diagonalsConsidered = considerDiagonals;
    m_solutionState = sudoku::solutionState(m_sheet, m_diagonalsConsidered);
}

void sudoku::App::createDifficult(bool createDifficult)
{
    m_createDifficult = createDifficult;
}

bool sudoku::saveState(const std::filesystem::path &path, const sudoku::App &sudokuApp, bool outputProcessingImg)
{
    try
    {
        cv::FileStorage file(path.string(), cv::FileStorage::WRITE);
        if (file.isOpened())
        {
            file << "sudokuSheet" << std::vector<sudoku::Value>(sudokuApp.sheet().begin(), sudokuApp.sheet().end());
            file << "considerDiagonals" << sudokuApp.diagonalsConsidered();
            file << "createDifficult" << sudokuApp.createDifficult();
            file << "outputProcessingImg" << outputProcessingImg;
            file.release();
            return true;
        }
    }
    catch (...)
    {}
    return false;
}

bool sudoku::loadState(const std::filesystem::path &path, sudoku::App &o_sudokuApp,
                       std::function<void(bool)> outputProcessingImgSetter)
{
    try
    {
        cv::FileStorage file(path.string(), cv::FileStorage::READ);
        if (file.isOpened())
        {
            std::vector<sudoku::Value> sheet;
            file["sudokuSheet"] >> sheet;
            o_sudokuApp.setSheet({utils::makeArray<81>([sheet](size_t i) { return sheet[i]; })});
            o_sudokuApp.clearHistory();
            bool considerDiagonals = false, createDifficult = true, outputProcessingImg = true;
            file["considerDiagonals"] >> considerDiagonals;
            o_sudokuApp.considerDiagonals(considerDiagonals);
            file["createDifficult"] >> createDifficult;
            o_sudokuApp.createDifficult(createDifficult);
            file["outputProcessingImg"] >> outputProcessingImg;
            outputProcessingImgSetter(outputProcessingImg);
            file.release();
            return true;
        }
    }
    catch (...)
    {}
    return false;
}
