import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: appWindow
    visible: true
    width: 640
    height: 1024
    title: qsTr("Sudoku Scan&Solve")

    SolvingView {
        id: solvingView
        anchors.fill: parent
    }

    Shortcut {
        sequence: StandardKey.Quit
        context: Qt.ApplicationShortcut
        onActivated: Qt.quit()
    }


}
