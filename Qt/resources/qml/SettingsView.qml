import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.0

Custom.Page {
    id: settingsView
    headerText: "Settings"
    scrollArea: ColumnLayout {
        id: layout
        width: parent.width

        Label {
            text: "Solving/Creating:"
            font.bold: true
            color: Style.lightGray
        }
        RowLayout {
            Label {
                text: "Consider Diagonals (Every\nNumber shall occur only once\nin the Diagonals of the Puzzle)"
                color: Style.lightGray
            }
            Item {
                Layout.fillWidth: true
            }
            Custom.CheckBox {
                penColor: Style.lightGray
                checked: puzzle.considerDiagonals
                onClicked: puzzle.considerDiagonals = checked
            }
        }
        Item {
            Layout.preferredHeight: 20
        }
        Rectangle {
            Layout.fillWidth: true
            height: 2
            color: Style.lightGray
        }
        Item {
            Layout.preferredHeight: 20
        }
        Label {
            text: "Scanning:"
            font.bold: true
            color: Style.lightGray
        }
        RowLayout {
            Label {
                text: "Show unprocessed Image\nwhile Scanning from Camera"
                color: Style.lightGray
            }
            Item {
                Layout.fillWidth: true
            }
            Custom.CheckBox {
                penColor: Style.lightGray
                checked: !processingFilter.outputProcessingImg
                onClicked: processingFilter.outputProcessingImg = !checked
            }
        }
        Item {
            Layout.preferredHeight: 20
        }
        Rectangle {
            Layout.fillWidth: true
            height: 2
            color: Style.lightGray
        }
        Item {
            Layout.preferredHeight: 20
        }
        RowLayout {
            Label {
                text: "Instructions:"
                font.bold: true
                color: Style.lightGray
            }
            Item {
                Layout.fillWidth: true
            }
            Custom.Button {
                text: "Show"
                penColor: Style.lightGray
                onClicked: instructionsView.open()
            }
        }
        Item {
            Layout.preferredHeight: 20
        }
        Rectangle {
            Layout.fillWidth: true
            height: 2
            color: Style.lightGray
        }
        Item {
            Layout.preferredHeight: 20
        }
        RowLayout {
            Label {
                text: "Open Source/Licenses:"
                font.bold: true
                color: Style.lightGray
            }
            Item {
                Layout.fillWidth: true
            }
            Custom.Button {
                text: "Show"
                penColor: Style.lightGray
                onClicked: licensesView.open()
            }
        }
    }

    InstructionsView {
        id: instructionsView
        edge: Qt.RightEdge
        width: parent.width
        height: parent.height
    }
    LicensesView {
        id: licensesView
        edge: Qt.RightEdge
        width: parent.width
        height: parent.height
    }
}
