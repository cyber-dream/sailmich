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

            delegate: Loader {
                sourceComponent: thumbnailPath ? imageThumbnail : loadRect

                Component {
                    id: imageThumbnail
                    Thumbnail {
                        id: thumb
                        width: gridView.cellWidth
                        height: gridView.cellHeight

                        imageSource: thumbnailPath
                        loadingAnimOpacity: loadingAnim.globalLoadingAnimOpacity

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                openMediaViewer(timeBucketDate,
                                                dayRowIndex, index)
                            }
                        }
                    }
                }

                Component {
                    id: loadRect
                    Rectangle {
                        width: gridView.cellWidth
                        height: gridView.cellHeight
                        color: Theme.secondaryHighlightColor
                        opacity: loadingAnimOpacity

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                //                                console.log(dayRowIndex)
                                openMediaViewer(timeBucketDate,
                                                dayRowIndex, index)
                            }
                        }
                    }
                }
            }
        }
    }
}
