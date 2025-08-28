import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import stephanwidor.SudokuScanAndSolve 1.1

Rectangle {
    id: puzzleTable
    color: Style.darkGray

    property int size: 100
    implicitWidth: tableView.size + 2 * tableView.borderExtra
    implicitHeight: tableView.size + 2 * tableView.borderExtra
    radius: tableView.borderExtra

    TableView {
        id: tableView
        anchors.centerIn: parent
        //interactive: false
        readonly property int borderExtra: 1
        property int cellSize: puzzleTable.size / 9
        property int size: 9 * cellSize
        onSizeChanged: layoutCount = 0
        property int layoutCount: 0
        onLayoutCountChanged: {
            if (layoutCount == 81)
                forceLayout()
        }
        width: size
        height: size
        contentWidth: size
        contentHeight: size

        model: puzzle
        delegate: PuzzleRect {
            id: cell
            rectAnchors {
                topMargin: model.row % 3 == 0 ? tableView.borderExtra : 0
                leftMargin: model.column % 3 == 0 ? tableView.borderExtra : 0
                bottomMargin: model.row % 3 == 2 ? tableView.borderExtra : 0
                rightMargin: model.column % 3 == 2 ? tableView.borderExtra : 0
            }
            implicitWidth: tableView.cellSize
            implicitHeight: tableView.cellSize
            onImplicitWidthChanged: ++tableView.layoutCount
        }
    }
}
