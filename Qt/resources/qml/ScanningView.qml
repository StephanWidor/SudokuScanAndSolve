import QtQuick
import QtCore
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import QtMultimedia
import "Custom" as Custom
import stephanwidor.SudokuScanAndSolve 1.1

Drawer {
    id: scanningView
    opacity: 0.9
    interactive: false
    property int downFillHeight: 8
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

    CameraHandle {
        id: cameraHandle
        videoOutput: processingFilter
    }

    FileDialog {
        id: loadDialog
        title: "Choose a Picture to Scan"
        currentFolder: StandardPaths.writableLocation(
                           StandardPaths.PicturesLocation)
        fileMode: FileDialog.OpenFile
        modality: Qt.ApplicationModal
        nameFilters: ["Image files (*.jpg *.png)"]
        onAccepted: processingFilter.processSingleImg(loadDialog.selectedFile)
        onRejected: scanningRect.state = ""
    }

    Rectangle {
        id: scanningRect
        anchors.fill: parent
        color: Style.darkGray

        property int stateChangeCount: 0
        onStateChanged: ++stateChangeCount
        property bool cameraStarted: false
        states: [
            State {
                name: ""
                StateChangeScript {
                    script: {
                        cameraHandle.stop()
                    }
                }
            },
            State {
                name: "scanning"
                property string headerLabelText: "Scanning..."
                StateChangeScript {
                    script: {
                        scanningRect.cameraStarted = cameraHandle.start()
                    }
                }
                PropertyChanges {
                    target: headerLabel
                    text: scanningRect.cameraStarted ? "Scanning..." : "Couldn't start, camera permissions granted?"
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
                StateChangeScript {
                    script: {
                        cameraHandle.stop()
                    }
                }
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
                StateChangeScript {
                    script: {
                        cameraHandle.stop()
                    }
                }
                PropertyChanges {
                    target: headerLabel
                    text: scanningView.scanSuccess ? "Found Sudoku" : "No Sudoku Found"
                    color: scanningView.scanSuccess ? Style.green : Style.red
                }
                PropertyChanges {
                    target: resultImg
                    visible: true
                    // we need to change id every time. seems otherwise we get a cached old image version
                    source: "image://scanResult/" + Number(
                                scanningRect.stateChangeCount).toString()
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
                    visible: cameraHandle.torchAvailable
                    checked: cameraHandle.torchMode === Camera.TorchOn
                    onClicked: cameraHandle.toggleTorch()
                }
                Item {
                    Layout.fillWidth: true
                    visible: cameraHandle.torchAvailable
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
                    onClicked: {
                        cameraHandle.stop()
                        scanningView.close()
                    }
                }
            }

            VideoOutput {
                id: videoOutput
                visible: scanningRect.state === "scanning"
                Layout.fillWidth: true
                Layout.fillHeight: true
                fillMode: VideoOutput.PreserveAspectFit
                Component.onCompleted: {
                    processingFilter.setOutputSink(videoOutput.videoSink)
                }
                Component.onDestruction: {
                    processingFilter.setOutputSink(null)
                }
            }

            Image {
                id: resultImg
                visible: scanningRect.state === "showing result"
                Layout.fillWidth: true
                Layout.fillHeight: true
                fillMode: Image.PreserveAspectFit
            }

            Item {
                id: fillItem
                visible: scanningRect.state === ""
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            RowLayout {
                id: buttonLayout
                Layout.alignment: Qt.AlignHCenter

                Custom.Button {
                    id: acceptButton
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
