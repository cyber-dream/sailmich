import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0
import "."
import ".."

SilicaListView {
    id: listView
    width: parent.width
    height: parent.height

    property real globalLoadingAnimOpacity: 0.1
    property PathViewModel slideShowModel: PathViewModel {
        timelineModel: model
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
        sourceComponent: timebucketModel.revealStatus
                         === TimelineTimeBucketModel.Complete ? dayRowGrid : stubGrid
        width: parent.width

        Component {
            id: dayRowGrid

            TimebucketGrid {
                id: kek
                implicitHeight: Theme.itemSizeLarge
                model: timebucketModel
                loadingAnimOpacity: globalLoadingAnimOpacity
                onOpenMediaViewer: {
                    slideShowModel.setCurrentIndex(timeBucketDate, dayRowIndex,
                                                   localAssetIndex)
                    //                    console.log(slideShowModel.currentGlobalIndex)
                    pageStack.push(Qt.resolvedUrl("../mediaviewer/MediaViewer.qml"), {
                                       "model": slideShowModel
                                   })
                }
            }
        }

        Component {
            id: stubGrid

            TimebucketStubGrid {
                loadingAnimOpacity: globalLoadingAnimOpacity
                timebucketSize: timebucketModel.bucketSize
                dateStr: {
                    const qDate = new Date(date.year, date.month, 1)
                    return Qt.formatDateTime(qDate, "ddd, MMM dd, yyyy")
                }

                timeBucketDate: date
                Component.onCompleted: {
                    if (timebucketModel.isRevealed //TODO may be delete this check
                            && !timebucketModel.isLoading) {
                        console.log("skip " + date + ", already revealed")
                        return
                    }
                    timebucketModel.needReveal()
                }

                onOpenMediaViewer: {
                    slideShowModel.setCurrentIndex(timeBucketDate, offset)
                    console.log(slideShowModel.currentGlobalIndex)
                    pageStack.push(Qt.resolvedUrl("../MediaViewer.qml"), {
                                       "model": slideShowModel
                                   })
                }
            }
        }
    }

    VerticalScrollDecorator {}
}
