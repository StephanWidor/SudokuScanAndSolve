import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import stephanwidor.SudokuScanAndSolve 1.0

Item {
    id: view
    property bool doScan: false // actually this is rather a command than a property
    property bool flashAvailable: false
    property bool torchOn: false

    onDoScanChanged: {
        if(doScan)
        {
            if(loader.item === null)
                loader.sourceComponent = loadComponent
        }
        else
        {
            if(loader.item !== null)
                loader.sourceComponent = null
        }
    }

    function switchTorch(on)
    {
        if(loader.item !== null)
            loader.item.switchTorch(on)
    }

    Loader {
        id: loader
        anchors.fill: parent
        sourceComponent: undefined
        onItemChanged: {
            if(item === null)
            {
                view.flashAvailable = false
                view.torchOn = false
            }
            else
            {
                view.flashAvailable = Qt.binding(function() { return item.flashAvailable })
                view.torchOn = Qt.binding(function() { return item.torchOn })
            }
        }
    }
    Component {
        id: loadComponent

        Item {
            property bool flashAvailable: camera.flash.supportedModes.includes(Camera.FlashVideoLight)
            property bool torchOn: camera.flash.mode === Camera.FlashVideoLight
            function switchTorch(on)
            {
                if(on)
                    camera.flash.mode = Camera.FlashVideoLight
                else
                    camera.flash.mode = Camera.FlashOff
            }
            function findBestResolution()
            {
                // we want a ratio close to 4:3, and resolution as high as possible
                var available = camera.supportedViewfinderResolutions()
                var maxNumPixels = 0
                var bestRatio = 4/3
                var minRatioDiff = Number.MAX_VALUE
                var minIndex = -1
                for(var i = 0; i < available.length; ++i)
                {
                    var width = available[i].width
                    var height = available[i].height
                    var ratioDiff = Math.abs(Math.max(width, height)/Math.min(width, height) - bestRatio)
                    var numPixels = width * height
                    if(ratioDiff < minRatioDiff || (ratioDiff === minRatioDiff && numPixels > maxNumPixels))
                    {
                        minRatioDiff = ratioDiff
                        maxNumPixels = numPixels
                        minIndex = i
                    }
                }
                return Qt.size(available[minIndex].width, available[minIndex].height)
            }
            Camera {
                id: camera
                position: Camera.BackFace
                captureMode: Camera.CaptureViewfinder
                exposure.exposureMode: Camera.ExposureAuto
                focus.focusMode: Camera.FocusContinuous
                onError: console.log(errorString)
                onCameraStatusChanged: {
                    if(cameraStatus === Camera.LoadedStatus)
                        camera.viewfinder.resolution = findBestResolution()
                }
            }
            VideoOutput {
                id: videoOutput
                anchors.fill: parent
                source: camera
                autoOrientation: true
                focus: visible
                filters: [processingFilter]
                Binding {
                    target: processingFilter
                    property: "cameraRotation"
                    value: videoOutput.orientation
                }
            }
        }
    }
}
