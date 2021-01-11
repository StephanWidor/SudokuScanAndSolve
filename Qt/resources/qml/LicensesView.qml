import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.0

Custom.Page {
    id: licensesView
    headerText: "Open Source/Licenses"
    scrollArea: ColumnLayout {
        id: layout
        width: parent.width
        ColumnLayout {
            width: parent.width
            Custom.RichText {
                Layout.fillWidth: true
                text: "
Sudoku Scan&Solve is written in C++. It uses Qt and QtQuick for UI and to bring it on Android. For Sudoku recognition, the \
awesome OpenCV framework is used. Icons are taken from Google's Material Design.
You can find the source code of Sudoku Scan&Solve on \
<a href='https://github.com/StephanWidor/SudokuScanAndSolve'>GitHub</a>.
Third party software that is used is mentioned on this page.
"
            }
            Label {
                Layout.fillWidth: true
                topPadding: 20
                color: Style.lightGray
                font.bold: true
                text: "Qt 5.15.2"
            }
            Custom.RichText {
                Layout.fillWidth: true
                color: Style.lightGray
                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                text: "
There is different licenses for Qt. As this app is open source, we can use the \
<a href='https://www.gnu.org/licenses/gpl-3.0.en.html'>GPLv3</a> license here.
"
            }
            Label {
                Layout.fillWidth: true
                topPadding: 20
                color: Style.lightGray
                font.bold: true
                text: "OpenCV 4.5.1"
            }
            Custom.RichText {
                Layout.fillWidth: true
                color: Style.lightGray
                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                text: "
OpenCV is licensed under <a href='https://github.com/opencv/opencv/blob/4.5.1/LICENSE'>Apache License 2.0</a>.
"
            }
            Label {
                Layout.fillWidth: true
                topPadding: 20
                color: Style.lightGray
                font.bold: true
                text: "Material Design Icons 4.0.0"
            }
            Custom.RichText {
                Layout.fillWidth: true
                color: Style.lightGray
                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                text: "
Material Design Icons are also licensed under \
<a href='https://github.com/google/material-design-icons/blob/4.0.0/LICENSE'>Apache License 2.0</a>.
"
            }
        }
    }
}
