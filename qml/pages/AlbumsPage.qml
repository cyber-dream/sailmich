import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0
import "../components"

Page {
    ThumbnailPulsator {
        id: loadingAnim
    }

    SilicaListView {
        anchors.fill: parent
        model: GlobalAlbumsList

        header: Column {
            width: parent.width
            PageHeader {
                //% "Albums"
                //: Album in the plural
                title: qsTrId("albumsPage.titleAlbums")
            }

            //            SearchField {
            //                width: parent.width
            //                placeholderText: "Search"

            //                onTextChanged: {
            //                    listModel.update()
            //                }
            //            }

            //            Row {
            //                width: parent.width
            //                ComboBox {
            //                    width: parent.width / 2

            //                    label: "Filter"

            //                    menu: ContextMenu {
            //                        MenuItem {
            //                            text: "All"
            //                        }
            //                        MenuItem {
            //                            text: "Shared with me"
            //                        }
            //                        MenuItem {
            //                            text: "My albums"
            //                        }
            //                    }
            //                }

            //                ComboBox {
            //                    width: parent.width / 2
            //                    label: "Sort"

            //                    menu: ContextMenu {
            //                        MenuItem {
            //                            text: "Album title"
            //                        }
            //                        MenuItem {
            //                            text: "Number of asset"
            //                        }
            //                        MenuItem {
            //                            text: "Last modified"
            //                        }
            //                        MenuItem {
            //                            text: "Created date"
            //                        }
            //                        MenuItem {
            //                            text: "Newest photo"
            //                        }
            //                        MenuItem {
            //                            text: "Oldest photo"
            //                        }
            //                    }
            //                }
            //            }
        }

        delegate: BackgroundItem {
            id: delegateItem
            width: ListView.view.width
            height: Theme.itemSizeLarge

            onClicked: {
//                console.log(albumInfo.id)
                pageStack.push(Qt.resolvedUrl("AlbumTimelinePage.qml"), {"albumInfo": albumInfo})
            }
            readonly property real leftPadding: Theme.horizontalPageMargin * 4

            Item {
                anchors.fill: parent
                opacity: albumSize < 1 ? 0.5 : 1
                clip: true

                Thumbnail {
                    id: icon
                    x: delegateItem.leftPadding
                    width: Theme.itemSizeLarge
                    height: Theme.itemSizeLarge
                    anchors.verticalCenter: parent.verticalCenter
                    imageSource: albumThumbnailPath
                    loadingAnimOpacity: loadingAnim.globalLoadingAnimOpacity
                    isImageIconOnLoading: true

                    Label {
                        text: albumSize >= 10000 ? (albumSize / 1000) + 'K' : albumSize
                        anchors.right: parent.left
                        anchors.rightMargin: Theme.paddingMedium
                        anchors.verticalCenter: parent.verticalCenter
                        color: Theme.secondaryColor
                        //                        font.pixelSize: Theme.fontSizeLarge
                        horizontalAlignment: Text.AlignRight
                    }
                }

                Column {
                    anchors.left: icon.right
                    anchors.leftMargin: Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    Item {
                        width: parent.width
                        height: albumNameLabel.height
                        Label {
                            id: albumNameLabel
                            width: parent.width

                            text: albumName
                            color: delegateItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                            horizontalAlignment: Text.AlignLeft
                        }
                        OpacityRampEffect {
                            sourceItem: albumNameLabel
                            direction: OpacityRamp.LeftToRight
                        }
                    }

                    Label {
                        text: "owned"
                        color: delegateItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
