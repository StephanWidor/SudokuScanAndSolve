#include "test/cv/recognition/digit.h"
#include "test/cv/recognition/sudoku.h"
#include "test/sudoku/solving.h"
#include "test/utils.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    std::filesystem::create_directory(test::utils::testOutputDir);

    test::cv::recognition::sudoku::run(test::utils::testDataDir / "Recognition" / "Sheets" / "Picture",
                                       test::utils::testDataDir / "Recognition" / "Sheets" / "Video",
                                       test::utils::testOutputDir / "SudokuRecognition", false,
                                       ::cv::recognition::digit::Classifier::KNN);

    test::cv::recognition::digit::runAll(test::utils::testDataDir / "Recognition" / "Numbers",
                                         cv::recognition::digit::Classifier::KNN);

    test::sudoku::runAll(test::utils::testDataDir / "Solving");

    test::sudoku::crossValidate();

    return 0;
}
