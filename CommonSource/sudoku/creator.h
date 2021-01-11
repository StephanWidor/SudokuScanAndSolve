#pragma once
#include "sudoku/Sheet.h"
#include <optional>

namespace sudoku {

Sheet create(bool considerDiagonals, bool removeSuperfluousEntries, std::optional<unsigned> seed = std::nullopt);

}    // namespace sudoku
