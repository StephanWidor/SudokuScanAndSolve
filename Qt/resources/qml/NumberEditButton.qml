import "Custom" as Custom

Custom.Button {
    id: button
    property int rectSize: 40
    implicitWidth: rectSize
    implicitHeight: rectSize
    font.pixelSize: rectSize / 2
}
