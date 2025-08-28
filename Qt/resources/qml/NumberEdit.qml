import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.1

ColumnLayout {
    id: numberEdit
    property int buttonSize: 40
    RowLayout {
        id: numberEdit0
        Layout.alignment: Qt.AlignHCenter
        Repeater {
            model: ["1", "2", "3", "4", "5", "6"]
            NumberEditButton {
                id: numberButton0
                text: modelData
                rectSize: numberEdit.buttonSize
                onClicked: puzzle.selectValue(index + 1)
                Shortcut {
                    sequence: modelData
                    onActivated: numberButton0.clicked()
                }
            }
        }
    }
    RowLayout {
        id: numberEdit1
        Layout.alignment: Qt.AlignHCenter
        Repeater {
            model: ["7", "8", "9"]
            NumberEditButton {
                id: numberButton1
                text: modelData
                rectSize: numberEdit.buttonSize
                onClicked: puzzle.selectValue(index + 7)
                Shortcut {
                    sequence: modelData
                    onActivated: numberButton1.clicked()
                }
            }
        }
        NumberEditButton {
            id: clearSelectedButton
            font.family: "Material Icons Outlined"
            text: "\ue256"
            rectSize: numberEdit.buttonSize
            onClicked: puzzle.clearSelected()
            Custom.ToolTip {
                text: "Clear selected Cell(s)"
                control: clearSelectedButton
            }
        }
        NumberEditButton {
            id: solveSelectedButton
            font.family: "Material Icons Outlined"
            text: "\ue8f4"
            rectSize: numberEdit.buttonSize
            onClicked: puzzle.solveSelected()
            Custom.ToolTip {
                text: "Solve selected Cell(s)"
                control: solveSelectedButton
            }
        }
        NumberEditButton {
            id: undoButton
            font.family: "Material Icons Outlined"
            text: "\ue166"
            rectSize: numberEdit.buttonSize
            visible: puzzle.hasHistory
            onClicked: puzzle.undo()
            Custom.ToolTip {
                text: "Undo last Edit"
                control: undoButton
            }
        }
    }
}
