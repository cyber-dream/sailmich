import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0

ListView {
    id: albumsFeaturing

    width: parent.width
    height: albumsFeaturing.width / 5 // TODO to property
    model: GlobalAlbumsList
    orientation: Qt.Horizontal
    interactive: false

    delegate: Loader {
        active: index < 5

        sourceComponent: BackgroundItem {
            id: backItem
            width: albumsFeaturing.width / 5
            height: albumsFeaturing.width / 5
            anchors.verticalCenter: parent.verticalCenter

            onClicked: pageStack.push(Qt.resolvedUrl("../pages/AlbumTimelinePage.qml"), {
                                          "albumInfo": albumInfo
                                      })

            Thumbnail {
                anchors.fill: parent
                imageSource: albumThumbnailPath
                loadingAnimOpacity: loadingAnim.globalLoadingAnimOpacity
                isImageIconOnLoading: true
            }

            Rectangle {
                id: overlay
                anchors.fill: parent
                color: Theme.highlightBackgroundColor
                opacity: backItem.pressed ? 0.5 : 0

                //                            Behavior on opacity { NumberAnimation { duration: 100 } }
            }

            Rectangle {
                id: shadowRect
                width: albumsFeaturing.width / 5
                height: albumsFeaturing.width / 5
                anchors.verticalCenter: parent.verticalCenter
                color: Theme.overlayBackgroundColor

                Label {
                    id: albumNameLabel

                    width: parent.width
                    anchors.bottom: parent.bottom
                    padding: Theme.paddingSmall
                    horizontalAlignment: Text.AlignLeft
                    color: backItem.pressed ? Theme.highlightColor : Theme.primaryColor

                    text: albumName
                }
                OpacityRampEffect {
                    sourceItem: albumNameLabel
                    direction: OpacityRamp.LeftToRight
                    slope: 2
                    offset: .5
                }
            }

            OpacityRampEffect {
                sourceItem: shadowRect
                direction: OpacityRamp.BottomToTop
                offset: .1
            }
        }
    }
}
