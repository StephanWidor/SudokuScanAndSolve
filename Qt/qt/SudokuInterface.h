#pragma once
#include "sudoku/App.h"
#include <QAbstractTableModel>
#include <QByteArray>
#include <QHash>
#include <QVariant>

namespace qt {

/// Wrap sudoku::App class into qml interface
class SudokuInterface : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(QString solutionState READ solutionState NOTIFY stateChanged)
    Q_PROPERTY(QColor solutionStateColor READ solutionStateColor NOTIFY stateChanged)
    Q_PROPERTY(bool considerDiagonals READ diagonalsConsidered WRITE considerDiagonals NOTIFY stateChanged)
    Q_PROPERTY(bool hasHistory READ hasHistory NOTIFY stateChanged)

public:
    SudokuInterface(QObject *parent = nullptr): QAbstractTableModel(parent) {}

    enum class CellRoles
    {
        BackgroundColor = Qt::UserRole + 1,
        TextColor,
        TextBold,
        Text,
        Selected
    };
    Q_ENUM(CellRoles)

    int rowCount(const QModelIndex & = QModelIndex()) const override { return 9; }
    int columnCount(const QModelIndex & = QModelIndex()) const override { return 9; }

    virtual QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void toggleSelection(int row, int col)
    {
        m_sudokuApp.toggleSelection(row, col);
        updateUI();
    }
    void selectRow(int row)
    {
        m_sudokuApp.select(sudoku::IndexContainer::row(row));
        updateUI();
    }
    void selectCol(int col)
    {
        m_sudokuApp.select(sudoku::IndexContainer::col(col));
        updateUI();
    }
    void selectBox(int row, int col)
    {
        m_sudokuApp.select(sudoku::IndexContainer::box(row, col));
        updateUI();
    }
    void selectFirstDiagonal()
    {
        m_sudokuApp.select(sudoku::IndexContainer::diagonal(0));
        updateUI();
    }
    void selectSecondDiagonal()
    {
        m_sudokuApp.select(sudoku::IndexContainer::diagonal(1));
        updateUI();
    }

    void selectValue(int value)
    {
        m_sudokuApp.select(static_cast<sudoku::Value>(value));
        updateUI();
    }

    void clearSelected()
    {
        m_sudokuApp.clearSelected();
        updateUI();
    }

    void solveSelected()
    {
        m_sudokuApp.solveSelected();
        updateUI();
    }

    void undo()
    {
        m_sudokuApp.undo();
        updateUI();
    }

    void createPuzzle()
    {
        m_sudokuApp.createPuzzle();
        updateUI();
    }

    void clearPuzzle()
    {
        m_sudokuApp.clearPuzzle();
        updateUI();
    }

    void solvePuzzle()
    {
        m_sudokuApp.solvePuzzle();
        updateUI();
    }

    void setSheet(const sudoku::Sheet &sheet)
    {
        m_sudokuApp.setSheet(sheet);
        updateUI();
    }

    sudoku::App &app() { return m_sudokuApp; }
    const sudoku::App &app() const { return m_sudokuApp; }

signals:
    void stateChanged();

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

    QString solutionState() const;
    QColor solutionStateColor() const;

    bool diagonalsConsidered() const { return m_sudokuApp.diagonalsConsidered(); }
    void considerDiagonals(bool considerDiagonals)
    {
        if (m_sudokuApp.diagonalsConsidered() != considerDiagonals)
        {
            m_sudokuApp.considerDiagonals(considerDiagonals);
            updateUI();
        }
    }

    bool hasHistory() const { return m_sudokuApp.hasHistory(); }

    void updateUI()
    {
        emit dataChanged(index(0, 0), index(8, 8));
        emit stateChanged();
    }

    sudoku::App m_sudokuApp;
};

}    // namespace qt
