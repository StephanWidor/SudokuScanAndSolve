import QtQuick 2.15
import stephanwidor.SudokuScanAndSolve 1.0

Text {
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
    color: Style.lightGray
    wrapMode: Text.WordWrap
    textFormat: Text.RichText
    onLinkActivated: Qt.openUrlExternally(link)
}
