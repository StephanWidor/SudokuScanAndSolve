import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.0

ColumnLayout {
    id: numberEdit
    property int buttonSize: 40
    RowLayout {
        id: numberEdit0
        Layout.alignment: Qt.AlignHCenter
        Repeater {
            model: ["1", "2", "3", "4", "5", "6"]
            Custom.Button {
                id: numberButton0
                text: modelData
                implicitWidth: numberEdit.buttonSize
                implicitHeight: numberEdit.buttonSize
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
            Custom.Button {
                id: numberButton1
                text: modelData
                implicitWidth: numberEdit.buttonSize
                implicitHeight: numberEdit.buttonSize
                onClicked: puzzle.selectValue(index + 7)
                Shortcut {
                    sequence: modelData
                    onActivated: numberButton1.clicked()
                }
            }
        }
        Custom.MaterialFontButton {
            id: clearSelectedButton
            text: "\ue256"
            implicitWidth: numberEdit.buttonSize
            implicitHeight: numberEdit.buttonSize
            onClicked: puzzle.clearSelected()
            Custom.ToolTip {
                text: "Clear selected Cell(s)"
                control: clearSelectedButton
            }
        }
        Custom.MaterialFontButton {
            id: solveSelectedButton
            text: "\ue8f4"
            implicitWidth: numberEdit.buttonSize
            implicitHeight: numberEdit.buttonSize
            onClicked: puzzle.solveSelected()
            Custom.ToolTip {
                text: "Solve selected Cell(s)"
                control: solveSelectedButton
            }
        }
        Custom.MaterialFontButton {
            id: undoButton
            text: "\ue166"
            implicitWidth: numberEdit.buttonSize
            implicitHeight: numberEdit.buttonSize
            visible: puzzle.hasHistory
            onClicked: puzzle.undo()
            Custom.ToolTip {
                text: "Undo last Edit"
                control: undoButton
            }
        }
    }
}
