#include "sudoku/Sheet.h"
#include <fstream>

/// \throw std::ios_base::failure
sudoku::Sheet sudoku::loadSheet(const std::filesystem::path &filePath)
{
    Sheet sheet;
    std::ifstream in;
    in.open(filePath);
    if (in.is_open() && !in.eof())
    {
        size_t row = 0u, col = 0u, value = 0u;
        in >> row >> col >> value;
        if (row >= 9 || col >= 9 || value > 9)
            throw std::ios_base::failure("bad values in file");
        while (true)
        {
            if (in.eof())
                break;
            sheet[sudoku::cellIndex(row, col)] = static_cast<sudoku::Value>(value);
            in >> row >> col >> value;
        }
        in.close();
    }
    else
        throw std::ios_base::failure("couldn't open file");
    return sheet;
}

bool sudoku::saveSheet(const Sheet &sheet, const std::filesystem::path &filePath)
{
    std::ofstream out;
    out.open(filePath);
    if (out.is_open())
    {
        for (size_t i = 0; i < 81; ++i)
        {
            if (sheet[i] != 0)
                out << row(i) << "\t" << col(i) << "\t" << static_cast<size_t>(sheet[i]) << std::endl;
        }
        out.close();
        return true;
    }
    return false;
}

std::ostream &operator<<(std::ostream &out, const sudoku::Sheet &sheet)
{
    out << "-------------------------" << std::endl;
    for (size_t row = 0; row < 9; ++row)
    {
        out << "|  ";
        for (size_t col = 0; col < 9; ++col)
        {
            out << static_cast<int>(sheet[sudoku::cellIndex(row, col)]);
            if (col == 8)
                out << "  |\n";
            else if (col % 3 == 2)
                out << "  |  ";
        }
        if (row % 3 == 2)
            out << "-------------------------" << std::endl;
    }
    return out;
}
