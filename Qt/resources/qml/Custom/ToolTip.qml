import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import stephanwidor.SudokuScanAndSolve 1.0

Controls.ToolTip {
    property var control
    property bool dark: true

    id: toolTip
    visible: control.pressed
    delay: 500
    timeout: 5000

    contentItem: Text {
        text: toolTip.text
        font: toolTip.font
        color: dark? Style.lightGray : Style.darkGray
    }

    background: Rectangle {
        color: dark? Style.darkGray : Style.lightGray
        radius: toolTip.height / 10
    }
}
