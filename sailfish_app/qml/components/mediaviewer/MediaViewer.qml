import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0

FullscreenContentPage {
    property alias model: slideShow.model

    SlideshowView {
        id: slideShow

        currentIndex: model.currentGlobalIndex
        anchors.fill: parent

        delegate: Item {
            readonly property var assetObject: model.assetObject

            width: PathView.view.width
            height: PathView.view.height

            Loader {
                anchors.fill: parent
                sourceComponent: thumbnailPath ? img : busyInd
                Component {
                    id: img
                    ZoomableFlickable {
                        id: zoomFlick
                        maximumZoom: 10 // TODO calculate

                        onZoomedChanged: {
                            overlay.active = !zoomed
                        }

                        MouseArea {
                            anchors.fill: parent
                            propagateComposedEvents: true

                            onClicked: {
                                if (zoomFlick.zoomed)
                                    zoomFlick.zoomOut()

                                overlay.active = !overlay.active

                                mouse.accepted = false // pass click to zoomFlick
                            }
                        }

                        Image {
                            anchors.fill: parent
                            source: thumbnailPath
                            fillMode: Image.PreserveAspectFit
                        }
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
            onClicked: {
                const currentData = slideShow.currentItem ? slideShow.currentItem.assetObject : null
                pageStack.push(Qt.resolvedUrl("../../pages/AssetDetails.qml"), {
                                   "asset": currentData
                               })
            }
        }
    }
}
