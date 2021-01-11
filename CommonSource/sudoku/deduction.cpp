#include "sudoku/deduction.h"
#include "sudoku/IndexContainer.h"
#include <cassert>

sudoku::deduction::Entries sudoku::deduction::entries(const sudoku::Sheet &sheet)
{
    std::vector<Entry> entries;
    entries.reserve(81);
    for (size_t cellIndex = 0; cellIndex < 81; ++cellIndex)
    {
        if (sheet[cellIndex] != 0u)
            entries.push_back({Entry::Set, cellIndex, sheet[cellIndex]});
    }
    return entries;
}

namespace {

void update(sudoku::deduction::SheetPossibleValues &io_possibleValues, bool considerDiagonals,
            const sudoku::deduction::Entries &newEntries)
{
    for (const auto &entry : newEntries)
    {
        const auto cellIndex = entry.cellIndex;
        const auto value = entry.value;
        auto &cellPossibleValues = io_possibleValues[cellIndex];
        if (entry.action == sudoku::deduction::Entry::Set)
        {
            for (sudoku::Value v = 1; v < 10; ++v)
                cellPossibleValues[v] &= (v == value);
            for (auto container :
                 sudoku::relevantContainers(sudoku::row(cellIndex), sudoku::col(cellIndex), considerDiagonals))
            {
                for (auto otherCellIndex : *container)
                    io_possibleValues[otherCellIndex][value] &= (otherCellIndex == cellIndex);
            }
        }
        else
            cellPossibleValues[value] = false;
    }
}

void update(const sudoku::deduction::SheetPossibleValues &possibleValues,
            sudoku::deduction::SheetPossiblePositions &io_possiblePositions, bool considerDiagonals)
{
    sudoku::forAllContainers(
      [&](const auto &container) {
          for (sudoku::Value value = 1; value < 10; ++value)
          {
              auto &valuePossiblePositions = io_possiblePositions[value];
              for (const auto cellIndex : container)
                  valuePossiblePositions[cellIndex] &= possibleValues[cellIndex][value];
          }
          return true;
      },
      considerDiagonals);
}

bool appendSetAction(const sudoku::Sheet &currentSheet, sudoku::deduction::Entries &entries, size_t cellIndex,
                     sudoku::Value value)
{
    if (currentSheet[cellIndex] != 0)
        return (currentSheet[cellIndex] == value);
    bool found = false;
    for (auto &entry : entries)
    {
        if (entry.cellIndex == cellIndex)
        {
            if (entry.action == sudoku::deduction::Entry::Set)
            {
                if (entry.value == value)
                    found = true;
                else
                    return false;
            }
            else
            {
                if (entry.value == value)
                    return false;
                else
                {
                    entry.action = sudoku::deduction::Entry::Set;
                    entry.value = value;
                    found = true;
                }
            }
        }
    }
    if (!found)
        entries.push_back({sudoku::deduction::Entry::Set, cellIndex, value});
    return true;
}

bool appendNewEntries(const sudoku::Sheet &currentSheet,
                      const sudoku::deduction::SheetPossiblePositions &possiblePositions, bool considerDiagonals,
                      sudoku::deduction::Entries &o_entries)
{
    return sudoku::forAllContainers(
      [&](const auto &container) {
          for (int value = 1; value < 10; ++value)
          {
              const auto &valPossiblePositions = possiblePositions[value];
              const auto numPossible = valPossiblePositions.numPossible(container);
              switch (numPossible)
              {
                  case 0:
                      return false;
                  case 1:
                  {
                      if (!appendSetAction(currentSheet, o_entries, valPossiblePositions.firstPossible(container),
                                           value))
                          return false;
                  }
                  default:
                      break;
              }
          }
          return true;
      },
      considerDiagonals);
}

bool appendNewEntries(const sudoku::Sheet &currentSheet, const sudoku::deduction::SheetPossibleValues &possibleValues,
                      sudoku::deduction::Entries &o_entries)
{
    for (size_t cellIndex = 0; cellIndex < 81; ++cellIndex)
    {
        switch (possibleValues[cellIndex].numPossible())
        {
            case 0:
                return false;
            case 1:
            {
                if (!appendSetAction(currentSheet, o_entries, cellIndex, possibleValues[cellIndex].firstPossible()))
                    return false;
                break;
            }
            default:
                break;
        }
    }
    return true;
}

}    // namespace

bool sudoku::deduction::Data::update(Entries newEntries)
{
    while (!newEntries.empty())
    {
        for (const auto &entry : newEntries)
        {
            if (entry.action == sudoku::deduction::Entry::Set)
                m_sheet[entry.cellIndex] = entry.value;
        }
        ::update(m_possibleValues, m_considerDiagonals, newEntries);
        ::update(m_possibleValues, m_possiblePositions, m_considerDiagonals);
        newEntries.clear();
        if (!appendNewEntries(m_sheet, m_possiblePositions, m_considerDiagonals, newEntries) ||
            !appendNewEntries(m_sheet, m_possibleValues, newEntries))
        {
            return m_valid = false;
        }
    }
    return m_valid = true;
}
