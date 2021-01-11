import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.0

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
            size: Math.min(mainLayout.width, mainLayout.height - 5 * scanButton.height)
        }

        Item {
            Layout.fillHeight: true
            Layout.preferredHeight: 3
        }

        NumberEdit {
            id: numberEdit
            Layout.alignment: Qt.AlignHCenter
        }

        Item {
            Layout.fillHeight: true
            Layout.preferredHeight: 4
        }

        RowLayout {
            id: globalButtons
            Layout.alignment: Qt.AlignHCenter
            Custom.Button {
                id: scanButton
                text: "Scan"
                onClicked: scanningView.open()
            }
            Custom.Button {
                id: createButton
                text: "Create"
                onClicked: puzzle.createPuzzle()
            }
            Custom.Button {
                id: clearAllButton
                text: "Clear All"
                onClicked: puzzle.clearPuzzle()
            }
            Custom.Button {
                id: solveAllButton
                text: "Solve All"
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
