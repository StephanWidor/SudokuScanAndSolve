#include "test/utils.h"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <sudoku/Sheet.h>
#include <sudoku/creator.h>

size_t test::utils::diff(const sudoku::Sheet &sheet0, const sudoku::Sheet &sheet1)
{
    size_t diff = 0;
    for (size_t i = 0; i < 81; ++i)
    {
        if (sheet0[i] != sheet1[i])
            ++diff;
    }
    return diff;
}

void test::utils::createRandomSheetFiles(const std::filesystem::path &folder, size_t numSheets, bool considerDiagonals)
{
    for (size_t i = 0; i < numSheets; ++i)
    {
        const auto file =
          considerDiagonals ? folder / (std::to_string(i) + "_diagonals.sp") : folder / (std::to_string(i) + ".sp");
        std::cout << "creating " << file << std::endl;
        sudoku::saveSheet(sudoku::create(considerDiagonals, true), file);
    }
}
