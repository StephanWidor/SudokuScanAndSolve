import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.1

Rectangle {
    id: puzzleView
    color: Style.lightGray

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: Style.globalMargin

        RowLayout {
            id: headerLayout
            Layout.fillWidth: true
            Label {
                id: solutionStateLabel
                text: "  " + puzzle.solutionState
                color: puzzle.solutionStateColor
            }
            Item {
                Layout.fillWidth: true
            }
            Custom.MaterialFontButton {
                id: settingsButton
                text: "\ue5d2"
                round: true
                drawBorder: false
                onClicked: settingsView.open()
            }
        }

        PuzzleTable {
            id: puzzleTable
            Layout.alignment: Qt.AlignHCenter
            size: Math.min(mainLayout.width, mainLayout.height / 14 * 9)
        }

        Item {
            Layout.fillHeight: true
            Layout.preferredHeight: 3
        }

        NumberEdit {
            id: numberEdit
            buttonSize: puzzleTable.size / 10
            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.fillHeight: true
            Layout.preferredHeight: 4
        }

        RowLayout {
            id: globalButtons
            Layout.alignment: Qt.AlignHCenter
            property int buttonWidth: puzzleTable.size / 5
            Custom.Button {
                id: scanButton
                text: "Scan"
                implicitWidth: globalButtons.buttonWidth
                onClicked: scanningView.open()
            }
            Custom.Button {
                id: createButton
                text: "Create"
                implicitWidth: globalButtons.buttonWidth
                onClicked: puzzle.createPuzzle()
            }
            Custom.Button {
                id: clearAllButton
                text: "Clear All"
                implicitWidth: globalButtons.buttonWidth
                onClicked: puzzle.clearPuzzle()
            }
            Custom.Button {
                id: solveAllButton
                text: "Solve All"
                implicitWidth: globalButtons.buttonWidth
                onClicked: puzzle.solvePuzzle()
            }
        }

        Item {
            id: downFillItem
            Layout.fillHeight: true
            Layout.preferredHeight: 8
        }
    }

    SettingsView {
        id: settingsView
        edge: Qt.TopEdge
        width: parent.width
        height: parent.height
    }

    ScanningView {
        id: scanningView
        width: parent.width
        height: parent.height
        downFillHeight: downFillItem.height
    }
}
