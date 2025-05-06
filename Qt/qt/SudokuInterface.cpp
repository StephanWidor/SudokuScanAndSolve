#include "qt/SudokuInterface.h"
#include "qt/Style.h"

namespace {}

QVariant qt::SudokuInterface::data(const QModelIndex &index, int role) const
{
    const auto cellIndex = sudoku::cellIndex(index.row(), index.column());
    switch (static_cast<CellRoles>(role))
    {
        case CellRoles::BackgroundColor:
            return m_sudokuApp.cellSelected(cellIndex) ? Style::orange() : Style::white();
        case CellRoles::TextColor:
            return m_sudokuApp.cellSelected(cellIndex)  ? Style::lightGray() :
                   m_sudokuApp.valueSelected(cellIndex) ? Style::orange() :
                                                          Style::darkGray();
        case CellRoles::TextBold:
            return m_sudokuApp.valueSelected(cellIndex);
        case CellRoles::Text:
        {
            const auto value = m_sudokuApp.sheet()[cellIndex];
            if (value != 0)
                return QString::number(value);
            return "";
        }
        case CellRoles::Selected:
            return m_sudokuApp.cellSelected(cellIndex);
        default:
            break;
    }
    return {};
}

QHash<int, QByteArray> qt::SudokuInterface::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[static_cast<int>(CellRoles::BackgroundColor)] = "backgroundColor";
    roles[static_cast<int>(CellRoles::TextColor)] = "textColor";
    roles[static_cast<int>(CellRoles::TextBold)] = "textBold";
    roles[static_cast<int>(CellRoles::Text)] = "text";
    roles[static_cast<int>(CellRoles::Selected)] = "cellSelected";
    return roles;
}

QString qt::SudokuInterface::solutionState() const
{
    switch (m_sudokuApp.solutionState())
    {
        case sudoku::SolutionState::Empty:
            return "Puzzle is empty";
        case sudoku::SolutionState::NoSolution:
            return "Puzzle has no Solution";
        case sudoku::SolutionState::NotUnique:
            return "Puzzle's Solution is not unique";
        case sudoku::SolutionState::Unique:
            return "Puzzle has a unique Solution";
        case sudoku::SolutionState::Solved:
            return "Solved";
        default:
            return "don't know";
    }
}

QColor qt::SudokuInterface::solutionStateColor() const
{
    switch (m_sudokuApp.solutionState())
    {
        case sudoku::SolutionState::Empty:
            return Style::darkGray();
        case sudoku::SolutionState::NoSolution:
            return Style::red();
        case sudoku::SolutionState::NotUnique:
            return Style::darkGray();
        case sudoku::SolutionState::Unique:
            return Style::darkGray();
        case sudoku::SolutionState::Solved:
            return Style::green();
        default:
            return Style::white();
    }
}
