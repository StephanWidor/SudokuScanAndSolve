#pragma once
#include "sudoku/Sheet.h"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <opencv2/core.hpp>
#include <sstream>
#include <string>

namespace test::utils {

const inline std::filesystem::path testDataDir = TOSTRING(TEST_DATA_DIR);
const inline std::filesystem::path testOutputDir = TOSTRING(TEST_OUTPUT_DIR);

inline std::string currentTimeString()
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

size_t diff(const ::sudoku::Sheet &sheet0, const ::sudoku::Sheet &sheet1);

class Timer
{
public:
    Timer(): m_startTime(std::chrono::high_resolution_clock::now()) {}

    double secondsAsDouble() const
    {
        const size_t millisecs =
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_startTime)
            .count();
        return static_cast<double>(millisecs) / 1000.0;
    }

private:
    std::chrono::high_resolution_clock::time_point m_startTime;
};

void createRandomSheetFiles(const std::filesystem::path &folder, size_t numSheets, bool considerDiagonals);

}    // namespace test::utils
