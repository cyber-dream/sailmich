import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Timeline 1.0
import Immich.MediaView 1.0
import "."
import ".."

SilicaListView {
    id: listView
    width: parent.width
    height: parent.height

    property real globalLoadingAnimOpacity: 0.1

    ModMediaView {
        id: mediaViewModel
        timeline: listView.model
    }

    SequentialAnimation on globalLoadingAnimOpacity {
        loops: Animation.Infinite
        running: true

        NumberAnimation {
            from: 0.1
            to: 0.3
            duration: 800
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            from: 0.3
            to: 0.1
            duration: 800
            easing.type: Easing.InOutQuad
        }
    }

    delegate: Loader {
        //TODO background item with reaction to tap
        id: gridLoader
        width: parent.width

        sourceComponent: revealStatus === ModTimebucket.RevealStatusRevealed ? dayRowGrid : stubGrid

        Component {
            id: dayRowGrid

            TimebucketGrid {
                id: kek
                //                implicitHeight: Theme.itemSizeLarge
                model: bucketModel
                loadingAnimOpacity: globalLoadingAnimOpacity
                onOpenMediaViewer: {
                    mediaViewModel.setCurrentIndexByLocalIndex(index,
                                                               assetIndex)
                    pageStack.push(Qt.resolvedUrl(
                                       "../mediaviewer/MediaViewer.qml"), {
                                       "model": mediaViewModel
                                   })
                }
            }
        }

        Component {
            id: stubGrid

            TimebucketStubGrid {
                loadingAnimOpacity: globalLoadingAnimOpacity
                timebucketSize: bucketModel.rowCount()
                dateStr: Qt.formatDateTime(date, "ddd, MMM dd, yyyy")

                timeBucketDate: date
                Component.onCompleted: {
                    if (bucketModel.isRevealed //TODO may be delete this check
                            && !bucketModel.isFetching) {
                        console.log("skip " + date + ", already revealed")
                        return
                    }
                    bucketModel.needReveal()
                }
            }
        }
    }

    InfoLabel {
        id: label
        property int rows: model.rowCount()

        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        Connections {
            target: model
            onRowsInserted: label.rows = model.rowCount()
            onRowsRemoved: label.rows = model.rowCount()
            onModelReset: label.rows = model.rowCount()
        }

        text: "Empty"
        visible: model.isInitialized && rows === 0
    }

    BusyIndicator { //TODO show it after n seconds/mseconds
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: !model.isInitialized
    }

    VerticalScrollDecorator {}
}
