import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

ApplicationWindow {
    id: appWindow
    visible: true
    width: 450
    height: 800
    title: "Sudoku Scan&Solve"

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
