import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0
//import Nemo.Thumbnailer 1.0
import "../"

//import "thumbhash.js" as ThumbHash
SilicaListView {
    id: rootView
    interactive: false

    width: parent.width
    height: contentHeight

    property real cellSize: width / 5 //TODO
    property real loadingAnimOpacity: 0.1
    signal openMediaViewer(var timeBucketDate, int dayRowIndex, int localAssetIndex)

    readonly property var timeBucketDate: date

    delegate: Column {
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

            model: dayRowModel

            cellWidth: cellSize
            cellHeight: cellWidth

            height: Math.ceil(count / 5) * cellHeight //TODO to property

            delegate: BackgroundItem {
                id: backItem
                width: gridView.cellWidth
                height: gridView.cellHeight

                onClicked: {
                    openMediaViewer(timeBucketDate, dayRowIndex, index)
                }

                Thumbnail {
                    anchors.fill: parent
                    imageSource: thumbnailPath
                    loadingAnimOpacity: loadingAnim.globalLoadingAnimOpacity
                }

                Rectangle {
                    id: overlay
                    anchors.fill: parent
                    color: Theme.highlightBackgroundColor
                    opacity: backItem.pressed ? 0.5 : 0

                    //Behavior on opacity { NumberAnimation { duration: 100 } }
                }
            }
        }
    }
}
