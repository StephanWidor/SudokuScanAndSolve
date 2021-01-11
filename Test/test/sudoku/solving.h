#pragma once
#include <filesystem>

namespace test::sudoku {

void runAll(const std::filesystem::path &sheetsFolder);

void crossValidate();

}    // namespace test::sudoku
