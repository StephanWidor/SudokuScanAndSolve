#include "test/sudoku/solving.h"
#include "sudoku/creator.h"
#include "sudoku/solver.h"
#include "test/utils.h"
#include <iostream>

void test::sudoku::runAll(const std::filesystem::path &sheetsFolder)
{
    std::cout << "test::sudoku::runAll(" << sheetsFolder.string() << ")" << std::endl;

    double solveTime = 0.0;
    auto run = [&solveTime](const std::filesystem::path &folder, size_t expectedNumSolutions) {
        std::cout << "\tFolder " << folder << ", expecting " << expectedNumSolutions << " solutions:" << std::endl;
        for (const auto &p : std::filesystem::directory_iterator(folder))
        {
            if (p.is_directory() || !(p.path().extension() == ".sp"))
                continue;
            const auto fileName = p.path().filename();
            std::cout << "\t\t" << fileName << std::endl;
            bool considerDiagonals = fileName.string().find("diagonals") != std::string::npos;
            const auto sheet = ::sudoku::loadSheet(p.path());
            utils::Timer timer;
            const auto solutions = ::sudoku::solve(sheet, considerDiagonals, 2);
            solveTime += timer.secondsAsDouble();
            if (solutions.size() != expectedNumSolutions)
                std::cout << "\t\t\tsolver has " << solutions.size() << " solutions" << std::endl;
        }
    };

    run(sheetsFolder / "invalid", 0);
    run(sheetsFolder / "unique", 1);
    run(sheetsFolder / "not_unique", 2);

    std::cout << "solver time total: " << solveTime << std::endl << std::endl;
}

void test::sudoku::crossValidate()
{
    std::cout << "test::sudoku::crossValidate()" << std::endl;

    const size_t numChecks = 500;
    double createTime = 0.0, solveTime = 0.0;
    const auto minToPrint = 0.5;

    auto solutionIsSupSet = [](const ::sudoku::Sheet &sheet, const ::sudoku::Sheet &solution) {
        for (size_t i = 0; i < 81; ++i)
        {
            if (sheet[i] != 0 && sheet[i] != solution[i])
                return false;
        }
        return true;
    };

    for (unsigned i = 0; i < numChecks; ++i)
    {
        const auto considerDiagonals = (i < numChecks / 2);
        const auto removeSuperfluous = (i % 2 == 0);
        std::cout << "\tcreating puzzle " << i << std::endl;
        utils::Timer newCreateTimer;
        const auto sheet = ::sudoku::create(considerDiagonals, removeSuperfluous, i);
        const auto _createTime = newCreateTimer.secondsAsDouble();
        createTime += _createTime;
        if (_createTime > minToPrint)
            std::cout << "\t\tcreate time: " << _createTime << std::endl;

        utils::Timer solveTimer;
        const auto solutions = ::sudoku::solve(sheet, considerDiagonals, 2);
        const auto _solveTime = solveTimer.secondsAsDouble();
        if (_solveTime > minToPrint)
            std::cout << "\t\tsolve time: " << _solveTime << std::endl;
        solveTime += _solveTime;
        if (solutions.size() != 1)
            std::cout << "\t\tsolving has " << solutions.size() << " solutions" << std::endl;
        if (!solutions.empty() && !solutionIsSupSet(sheet, solutions.front()))
            std::cout << "\t\tsolution is no supset of sheet" << std::endl;
    }
    std::cout << "create time: " << createTime << std::endl;
    std::cout << "solve time: " << solveTime << std::endl << std::endl;
}
