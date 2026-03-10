import QtQuick 2.0
import Sailfish.Silica 1.0

//TODO fix fadeAnim when photo appears
Item {
    id: loader
    property string imageSource
    property real cellSize: Theme.itemSizeMedium
    property real loadingAnimOpacity: 0.1
    property bool isImageIconOnLoading: false

    property string _imageSource: ""

    width: cellSize
    height: cellSize


    Image {
        id: imageThumbnail
        anchors.fill: parent

        source: imageSource
        asynchronous: true

        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectCrop

        visible: imageSource != ""

        opacity: imageSource ? 1.0 : 0.0
    }

    Rectangle {
        id: loadingStubRect

        anchors.fill: parent

        color: Theme.secondaryHighlightColor
        opacity: loadingAnimOpacity

        visible: imageThumbnail.status === Image.Ready

        Image {
            anchors.centerIn: parent
            source: "image://theme/icon-m-image"
            visible: isImageIconOnLoading
        }
    }
}
