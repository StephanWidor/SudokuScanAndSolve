import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.1

Custom.Page {
    id: instructionsView
    headerText: "Instructions"
    scrollArea: ColumnLayout {
        id: layout
        width: parent.width

        ColumnLayout {
            width: parent.width
            Custom.RichText {
                Layout.fillWidth: true
                text: "
Sudoku Scan&Solve can scan Sudoku puzzles from paper or an image and help you find the solution. \
It can also create puzzles, so that you can also use it as a simple Sudoku App that always tells you if you\'ve made a mistake, \
and helps you if you get stuck.
"
            }
            Label {
                Layout.fillWidth: true
                topPadding: 20
                color: Style.lightGray
                font.bold: true
                text: "Scanning"
            }
            Custom.RichText {
                Layout.fillWidth: true
                text: "
Tap on \"Scan\" on the left bottom corner. A view will appear that lets you choose if you want to scan from camera or from an \
image.
If you tap on \"Album\" Scan&Solve lets you choose an image from your library. If you have your puzzle available on paper, the \
recommended way of scanning is from camera though.
Choose \"Camera\", and the device will start showing you what \"it is seeing\". Try to focus on the puzzle. If the puzzle isn't
recognized, maybe move a bit forward and backward slowly to trigger the camera's autofocus.
As Scan&Solve doesn\'t have only one image of the puzzle then, it is more likely to recognize it than scanning from a single \
image.
When Scan&Solve has recognized a Sudoku, it will show you an image with the cells of the puzzle marked. If this looks okay \
(numbers should be within the marked areas), accept it, or try rescanning otherwise.
"
            }
            Label {
                Layout.fillWidth: true
                topPadding: 20
                color: Style.lightGray
                font.bold: true
                text: "Editing/Solving:"
            }
            Custom.RichText {
                Layout.fillWidth: true
                text: "
On top of the puzzle, Scan&Solve will always tell you, if there is a solution for the puzzle, and if so, if it is unique. \
You can select cells by tapping on them, 3x3 boxes by double tapping, and rows, columns and diagonals by moving your finger \
from a cell in the respective direction.
When a cell is selected, you can clear (<font face='Material Icons Outlined'>\ue256</font>), edit (1...9) or solve \
(<font face='Material Icons Outlined'>\ue8f4</font>) it.
When a row is selected, you can clear (<font face='Material Icons Outlined'>\ue256</font>) or solve it as a whole \
(<font face='Material Icons Outlined'>\ue8f4</font>), or tap on a number to fill in that number in the correct \
position for that row. Same for columns, diagonals, boxes, respectively. Undo of last edits is also possible \
(<font face='Material Icons Outlined'>\ue166</font>)
Besides, you can solve or clear the whole puzzle, or create a new one by tapping on the respective buttons.
"
            }
            Custom.RichText {
                Layout.fillWidth: true
                topPadding: 20
                text: "
If you need help, or have remarks, feel free to visit
<a href='http://sudokuscanandsolve.widor.site'>sudokuscanandsolve.widor.site</a>
or contact
<a href='mailto:sudokuscanandsolve@widor.site'>sudokuscanandsolve@widor.site</a>
"
            }
        }
    }
}
