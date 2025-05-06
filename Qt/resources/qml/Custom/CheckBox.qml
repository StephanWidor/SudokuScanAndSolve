import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Controls.impl
import stephanwidor.SudokuScanAndSolve 1.1

Controls.CheckBox {
    id: control
    property var penColor: Style.darkGray

    indicator: Rectangle {
        property int size: 1.1 * Math.max(checkedLabel.width,
                                          checkedLabel.height)
        implicitWidth: size
        implicitHeight: size
        radius: control.height / 10

        x: control.text ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding) : control.leftPadding
                          + (control.availableWidth - width) / 2
        y: control.topPadding + (control.availableHeight - height) / 2

        color: control.down ? Style.orange : Style.transparent
        border.width: control.visualFocus ? 2 : 1
        border.color: penColor

        Controls.Label {
            id: checkedLabel
            anchors.centerIn: parent
            visible: control.checkState === Qt.Checked
            font.family: "Material Icons Outlined"
            font.pointSize: 1.3 * Qt.application.font.pointSize
            text: "\ue876"
            font.bold: true
            color: control.down ? Style.lightGray : penColor
        }
    }

    contentItem: CheckLabel {
        id: textItem
        leftPadding: control.indicator
                     && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: control.indicator
                      && control.mirrored ? control.indicator.width + control.spacing : 0
        text: control.text
        font: control.font
        color: penColor
    }
}
