import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "." as Custom
import stephanwidor.SudokuScanAndSolve 1.0

Drawer {
    id: page
    opacity: 0.9
    interactive: false
    property alias headerText: headerLabel.text
    property alias scrollArea: scrollAreaLoader.sourceComponent

    Rectangle {
        id: pageRect
        color: Style.darkGray
        anchors.fill: parent

        RowLayout {
            id: headerLayout
            anchors.top: parent.top
            anchors.margins: Style.globalMargin
            width: parent.width - 2 * Style.globalMargin
            anchors.horizontalCenter: parent.horizontalCenter
            Label {
                id: headerLabel
                color: Style.lightGray
                font.bold: true
                font.underline: true
                text: "Page Header"
            }
            Item {
                Layout.fillWidth: true
            }
            Custom.CloseButton {
                id: closeButton
                onClicked: page.close()
            }
        }

        ScrollView {
            id: scrollView
            anchors.top: headerLayout.bottom
            anchors.margins: Style.globalMargin
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 2 * Style.globalMargin
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded
            contentWidth: scrollView.width
            Loader {
                id: scrollAreaLoader
                anchors.fill: parent
            }
        }
    }
}
