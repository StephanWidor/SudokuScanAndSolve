import QtQuick
import QtQuick.Controls
import stephanwidor.SudokuScanAndSolve 1.1

ItemDelegate {
    id: delegate
    property bool onFirstDiagonal: model.row === model.column
    property bool onSecondDiagonal: model.row === 8 - model.column
    property alias rectAnchors: rect.anchors

    Rectangle {
        id: rect
        color: model.backgroundColor
        border.color: Style.darkGray
        border.width: 1
        anchors.fill: parent

        Label {
            color: model.textColor
            font.bold: model.textBold
            anchors.centerIn: parent
            text: model.text
            font.pixelSize: rect.width / 2
        }
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: puzzle.toggleSelection(model.row, model.column)
            onDoubleClicked: puzzle.selectBox(model.row, model.column)
            function select(x, y) {
                var xStep = Math.floor(x / width)
                var yStep = Math.floor(y / height)
                if (xStep === 0 && yStep !== 0)
                    puzzle.selectCol(model.column)
                else if (xStep !== 0 && yStep === 0)
                    puzzle.selectRow(model.row)
                else if (yStep !== 0 && xStep / yStep >= 0.5
                         && xStep / yStep <= 2 && delegate.onFirstDiagonal)
                    puzzle.selectFirstDiagonal()
                else if (yStep !== 0 && xStep / yStep <= -0.5
                         && xStep / yStep >= -2 && delegate.onSecondDiagonal)
                    puzzle.selectSecondDiagonal()
            }
            onReleased: select(mouse.x, mouse.y)
            scrollGestureEnabled: true
            onWheel: select(wheel.angleDelta.x, wheel.angleDelta.y)
        }
    }
}
