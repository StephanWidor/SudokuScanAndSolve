import QtQuick
import QtMultimedia
import stephanwidor.SudokuScanAndSolve 1.1

Rectangle {
    id: cameraHandle
    property alias active: camera.active
    property alias videoOutput: captureSession.videoOutput
    property alias cameraCount: camera.deviceCount
    property alias availableCameraFormats: camera.cameraDevice.videoFormats
    property alias cameraFormatIndex: camera.formatIndex
    property bool torchAvailable: camera.isTorchModeSupported(Camera.TorchOn)
    property alias torchMode: camera.torchMode

    function toggleTorch() {
        if (torchMode === Camera.TorchOff)
            torchMode = Camera.TorchOn
        else
            torchMode = Camera.TorchOff
    }

    function start() {
        if (processingFilter.startCameraThread()) {
            camera.start()
            return true
        }
        return false
    }

    function stop() {
        processingFilter.stopCameraThread()
        camera.stop()
    }

    function switchToNextCameraDevice() {
        camera.switchToNextDevice()
    }

    MediaDevices {
        id: mediaDevices
    }

    CaptureSession {
        id: captureSession
        camera: Camera {
            id: camera
            property alias devices: mediaDevices.videoInputs
            property int deviceCount: devices.length
            property int deviceIndex: 0
            property int formatIndex: 0
            focusMode: Camera.FocusModeAuto
            Component.onCompleted: {
                if (deviceCount == 0)
                    return

                for (var i = 0; i < deviceCount; i++) {
                    // we prefer back facing camera (1 is Camera.BackFace)
                    if (devices[i].position === 1)
                        camera.deviceIndex = i
                }

                if (camera.deviceIndex === 0) {
                    camera.formatIndex = findBestFormatIndex()
                }
            }

            onActiveChanged: {
                if (camera.active) {
                    camera.cameraFormat = camera.cameraDevice.videoFormats[camera.formatIndex]
                    cameraHandle.availableCameraFormatsChanged()
                }
            }

            onDeviceIndexChanged: {
                camera.cameraDevice = camera.devices[camera.deviceIndex]
            }
            onCameraDeviceChanged: {
                camera.formatIndex = findBestFormatIndex()
            }
            onFormatIndexChanged: {
                camera.cameraFormat = camera.cameraDevice.videoFormats[camera.formatIndex]
            }

            function switchToNextDevice() {
                camera.deviceIndex = (deviceIndex + 1) % deviceCount
            }

            function findBestFormatIndex() {
                var bestSmallerSize = 720
                var minRatio = Number.MAX_VALUE
                var minSizeDiff = Number.MAX_VALUE
                var bestIndex = 0
                var formats = cameraHandle.availableCameraFormats
                for (var i = 0; i < formats.length; i++) {
                    var formatResolution = formats[i].resolution
                    var smallerSize = Math.min(formatResolution.width,
                                               formatResolution.height)
                    var sizeDiff = Math.abs(smallerSize - bestSmallerSize)
                    var largerSize = Math.max(formatResolution.width,
                                              formatResolution.height)
                    var ratio = largerSize / smallerSize
                    if (sizeDiff < minSizeDiff || (sizeDiff == minSizeDiff
                                                   && ratio < minRatio)) {
                        minSizeDiff = sizeDiff
                        minRatio = ratio
                        bestIndex = i
                    }
                }
                return bestIndex
            }
        }
    }
}
