#pragma once
#include "sudoku/Sheet.h"
#include <deque>

namespace sudoku {

class History
{
public:
    History(const sudoku::Sheet &initSheet) { m_history.push_back(initSheet); }

    void push(const sudoku::Sheet &sheet)
    {
        if (m_history.empty() || sheet != m_history.back())
        {
            m_history.push_back(sheet);
            if (m_history.size() > g_maxNum)
                m_history.pop_front();
        }
    }

    const sudoku::Sheet &back() const { return m_history.back(); }

    bool pop()
    {
        if (hasHistory())
        {
            m_history.pop_back();
            return true;
        }
        return false;
    }

    bool hasHistory() const { return m_history.size() > 1; }

    void clear() { m_history.erase(m_history.begin(), m_history.end() - 1); }

private:
    std::deque<sudoku::Sheet> m_history;
    static constexpr size_t g_maxNum = 20;
};

}    // namespace sudoku
