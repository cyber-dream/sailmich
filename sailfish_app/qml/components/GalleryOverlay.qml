import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    property string previewPath

    Image {
        id: name
        source: previewPath
        anchors.fill: parent
    }
}
