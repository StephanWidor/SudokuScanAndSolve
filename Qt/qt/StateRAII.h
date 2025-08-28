#pragma once
#include "qt/FileSystem.h"
#include "qt/ProcessingFilter.h"
#include <sudoku/App.h>

namespace qt {

class StateRAII
{
public:
    StateRAII(sudoku::App &app, qt::ProcessingFilter &processingFilter)
        : m_app(app), m_processingFilter(processingFilter)
    {
        sudoku::loadState(qt::stateFilePath(), m_app, [this](bool outputProcessingImg) {
            m_processingFilter.setShowProcessingImg(outputProcessingImg);
        });
    }

    void saveState() { sudoku::saveState(qt::stateFilePath(), m_app, m_processingFilter.showProcessingImg()); }

    ~StateRAII() { saveState(); }

private:
    sudoku::App &m_app;
    qt::ProcessingFilter &m_processingFilter;
};

}    // namespace qt
