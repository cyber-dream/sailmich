import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0
import Immich 1.0

FullscreenContentPage {
    property alias model: slideShow.model

    SlideshowView {
        id: slideShow
        //        model: 10
        currentIndex: model.currentGlobalIndex
        anchors.fill: parent

        delegate: MouseArea {
            width: PathView.view.width
            height: PathView.view.height

            readonly property var assetDataObject: model.assetDataObject

            onClicked: overlay.active = !overlay.active

            Loader {
                anchors.fill: parent
                sourceComponent: previewPath ? img : busyInd
                Component {
                    id: img
                    Image {
                        anchors.fill: parent
                        source: previewPath
                        fillMode: Image.PreserveAspectFit
//                        Label {
//                            anchors.bottom: parent.bottom
//                            anchors.horizontalCenter: parent.horizontalCenter
//                            anchors.bottomMargin: Theme.paddingLarge
//                            text: index
//                        }
                    }
                }
                Component {
                    id: busyInd
                    Item {
                        anchors.fill: parent
                        BusyIndicator {
                            size: BusyIndicatorSize.Medium
                            anchors.centerIn: parent
                            running: true
                        }
                    }
                }
            }
        }
    }

    Item {
        id: overlay

        property bool active: true

        enabled: active
        anchors.fill: parent
        opacity: active ? 1.0 : 0.0
        Behavior on opacity {
            FadeAnimator {}
        }

        IconButton {
            y: Theme.paddingLarge
            anchors {
                right: parent.right
                rightMargin: Theme.horizontalPageMargin
            }
            icon.source: "image://theme/icon-m-dismiss"
            onClicked: pageStack.pop()
        }

        IconButton {
            y: Theme.paddingLarge
            anchors {
                left: parent.left
                leftMargin: Theme.horizontalPageMargin
            }
            icon.source: "image://theme/icon-m-about"
            onClicked: pageStack.push(Qt.resolvedUrl(
                                          "../pages/AssetDetails.qml"), {
                                          "assetData": slideShow.currentItem.assetDataObject
                                      })
        }
    }
}
