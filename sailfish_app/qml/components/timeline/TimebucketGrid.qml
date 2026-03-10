import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Timeline 1.0
import "../"

//import "thumbhash.js" as ThumbHash
Column {
    property real loadingAnimOpacity: 0.1
    property var model
    property real cellSize: width / 5 //TODO

    //    property type name: value
    signal openMediaViewer(int assetIndex)
    width: parent.width
    readonly property int dayRowIndex: index
    SectionHeader {
        id: sectionHeader
        text: Qt.formatDateTime(date, "ddd, MMM dd, yyyy")
    }

    SilicaGridView {
        id: gridView
        width: parent.width
        interactive: false

        model: bucketModel

        cellWidth: cellSize
        cellHeight: cellWidth

        height: Math.ceil(count / 5) * cellHeight //TODO to property

        delegate: BackgroundItem {
            id: backItem

            _showPress: false

            width: gridView.cellWidth
            height: gridView.cellHeight

            onClicked: {
                openMediaViewer(index)
            }

            ImmichThumbnail {
                anchors.fill: parent
                imageSource: thumbnailSource
                //                    loadingAnimOpacity: loadingAnim.globalLoadingAnimOpacity TODO
            }

            Rectangle {
                id: overlay

                property color highlightedColor: Theme.rgba(
                                                     palette.highlightBackgroundColor,
                                                     Theme.highlightBackgroundOpacity)

                anchors.fill: parent
                color: backItem.highlighted ? highlightedColor : "transparent"

                //Behavior on opacity { NumberAnimation { duration: 100 } }
            }
        }
    }
}
