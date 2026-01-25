import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0

Column {
    id: root
//    width: parent.width

    property int timebucketSize: 1
    property alias dateStr: sectionHeader.text
    property real loadingAnimOpacity: 0.1

    signal openMediaViewer(var timeBucketDate, int offset)

    property var timeBucketDate

    SectionHeader {
        id: sectionHeader
        text: root.dateStr
    }

    SilicaGridView {
        id: gridView
        width: parent.width
        interactive: false

        cellWidth: width / 5
        cellHeight: cellWidth
        model: timebucketSize

        height: Math.ceil(count / 5) * cellHeight

        delegate: Rectangle {
            width: gridView.cellWidth
            height: gridView.cellHeight
            color: Theme.secondaryHighlightColor
            opacity: root.loadingAnimOpacity

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    openMediaViewer(timeBucketDate, index)
                }
            }
        }
    }
}
