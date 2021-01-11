import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import stephanwidor.SudokuScanAndSolve 1.0

Controls.Button {
    id: control
    property var penColor: Style.darkGray
    property bool drawBorder: true
    property bool round: false

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.down? Style.lightGray : penColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        opacity: enabled ? 1 : 0.3
        color: control.down? Style.orange : Style.transparent
        border.color: control.penColor
        border.width: drawBorder? 2 : 0
        radius: round? control.height / 2 : control.height / 10
    }
}
