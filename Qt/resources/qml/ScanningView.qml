import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import QtMultimedia 5.15
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.0

Drawer {
    id: scanningView
    opacity: 0.9
    interactive: false
    property int downFillHeight: 10
    onClosed: {
        scanningRect.state = ""
    }
    property bool scanSuccess: false
    Connections {
        target: processingFilter
        function onResultReady(success) {
            scanSuccess = success
            scanningRect.state = "showing result"
        }
    }

    FileDialog {
        id: loadDialog
        title: "Choose a Picture to Scan"
        folder: shortcuts.pictures
        selectMultiple: false
        nameFilters: ["Image files (*.jpg *.png)"]
        onAccepted: processingFilter.processImg(fileUrl)
        onRejected: scanningRect.state = ""
    }

    Rectangle {
        id: scanningRect
        anchors.fill: parent
        color: Style.darkGray

        property int stateChangeCount: 0
        onStateChanged: ++stateChangeCount
        states: [
            State {
                name: "scanning"
                PropertyChanges {
                    target: headerLabel
                    text: "Scanning..."
                }
                PropertyChanges {
                    target: cameraView
                    doScan: true
                }
                PropertyChanges {
                    target: cancelButton
                    visible: true
                }
                PropertyChanges {
                    target: albumButton
                    visible: false
                }
                PropertyChanges {
                    target: cameraButton
                    visible: false
                }
            },
            State {
                name: "open file"
                PropertyChanges {
                    target: headerLabel
                    text: "Open File..."
                }
                PropertyChanges {
                    target: loadDialog
                    visible: true
                }
            },
            State {
                name: "showing result"
                PropertyChanges {
                    target: headerLabel
                    text: scanningView.scanSuccess? "Found Sudoku" : "No Sudoku Found"
                    color: scanningView.scanSuccess? Style.green : Style.red
                }
                PropertyChanges {
                    target: resultImg
                    visible: true
                    // we need to change id every time. seems otherwise we get a cached old image version
                    source: "image://scanResult/" + Number(scanningRect.stateChangeCount).toString()
                }
                PropertyChanges {
                    target: acceptButton
                    visible: scanningView.scanSuccess
                }
            }
        ]

        ColumnLayout {
            id: mainLayout
            anchors.fill: parent
            anchors.margins: Style.globalMargin

            RowLayout {
                id: headerLayout
                Layout.fillWidth: true

                Custom.CheckBox {
                    id: torchCheck
                    text: "Torch"
                    penColor: Style.lightGray
                    visible: cameraView.flashAvailable
                    checked: cameraView.torchOn
                    onClicked: cameraView.switchTorch(checked)
                }
                Item {
                    Layout.fillWidth: true
                    visible: cameraView.flashAvailable
                }
                Label {
                    id: headerLabel
                    text: "Scan Sudoku"
                    color: Style.lightGray
                }
                Item {
                    Layout.fillWidth: true
                }
                Custom.CloseButton {
                    id: closeButton
                    onClicked: scanningView.close()
                }
            }

            CameraView {
                id: cameraView
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Image {
                id: resultImg
                visible: false// scanningRect.state == "showing result"
                Layout.fillWidth: true
                Layout.fillHeight: true
                fillMode: Image.PreserveAspectFit
            }

            RowLayout {
                id: buttonLayout
                Layout.alignment: Qt.AlignHCenter

                Custom.Button {
                    id:acceptButton
                    text: "Accept"
                    visible: false
                    penColor: Style.lightGray
                    onClicked: {
                        processingFilter.acceptScanResult()
                        scanningView.close()
                    }
                }
                Custom.Button {
                    id: cancelButton
                    text: "Cancel"
                    visible: false
                    penColor: Style.lightGray
                    onClicked: scanningRect.state = ""
                }
                Custom.Button {
                    id: albumButton
                    text: "Album"
                    penColor: Style.lightGray
                    onClicked: scanningRect.state = "open file"
                }
                Custom.Button {
                    id: cameraButton
                    text: "Camera"
                    penColor: Style.lightGray
                    onClicked: scanningRect.state = "scanning"
                }
            }

            Item {
                Layout.preferredHeight: scanningView.downFillHeight
            }
        }
    }
}
