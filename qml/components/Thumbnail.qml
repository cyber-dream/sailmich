import QtQuick 2.0
import Sailfish.Silica 1.0

Loader {
    id: loader
    property var imageSource
    property real cellSize: Theme.itemSizeMedium
    property real loadingAnimOpacity: 0.1
    property bool isImageIconOnLoading: false

    sourceComponent: imageSource ? imageThumbnail : loadRect

    Component {
        id: imageThumbnail
        Image {
            id: thumb
            width: cellSize
            height: cellSize

            source: imageSource
            sourceSize.width: cellSize
            sourceSize.height: cellSize
            fillMode: Image.PreserveAspectCrop
        }
    }

    Component {
        id: loadRect
        Rectangle {
            width: cellSize
            height: cellSize
            color: Theme.secondaryHighlightColor
            opacity: loadingAnimOpacity

            Image {
                anchors.centerIn: parent
                source: "image://theme/icon-m-image"
                visible: isImageIconOnLoading
            }
        }
    }
}
