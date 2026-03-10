import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Album 1.0
import Immich.Timeline 1.0
import "../../components/dream-tabs"
import "../../components"

DreamTab {
    //% "Albums"
    //: Tab title
    title: qsTrId("albumsPage.titleAlbums")

    menu: Component {
        PullDownMenu {
            MenuItem {
                //% "Setting"
                //: Settings page pulley menu item
                text: qsTrId("mainPage.settings")
                onClicked: pageStack.push(Qt.resolvedUrl("../SettingsPage.qml"))
            }
        }
    }

    SilicaListView {
        anchors.fill: parent

        FeAlbumsList {
            params: FeAlbumsListParams {
                ownershipType: FeAlbumsListParams.OwnershipTypeAll
            }
            model: albumsListModel
        }

        model: ModAlbumsList {
            id: albumsListModel
        }

        delegate: BackgroundItem {
            id: delegateItem
            width: ListView.view.width
            height: Theme.itemSizeLarge

            FeTimelineParams {
                id: params
                album_id: albumId
            }

            onClicked: {
                pageStack.push(Qt.resolvedUrl("../TimelinePage.qml"), {
                                   "pageTitle": albumName !== "" ? albumName : "Album",
                                   "pageDescription": albumDescription,
                                   "timelineFetcherParams": params
                               })
            }
            readonly property real leftPadding: Theme.horizontalPageMargin * 4

            Item {
                anchors.fill: parent
                opacity: albumSize < 1 ? 0.5 : 1
                clip: true

                ImmichThumbnail {
                    id: icon
                    x: delegateItem.leftPadding
                    width: Theme.itemSizeLarge
                    height: Theme.itemSizeLarge
                    anchors.verticalCenter: parent.verticalCenter
                    imageSource: albumThumbnailPath

                    //                    loadingAnimOpacity: loadingAnim.globalLoadingAnimOpacity TODO fix pulsator
                    //                    isImageIconOnLoading: true TODO fix icon
                    Label {
                        text: albumSize >= 10000 ? (albumSize / 1000) + 'K' : albumSize
                        anchors.right: parent.left
                        anchors.rightMargin: Theme.paddingMedium
                        anchors.verticalCenter: parent.verticalCenter
                        color: Theme.secondaryColor
                        // font.pixelSize: Theme.fontSizeLarge
                        horizontalAlignment: Text.AlignRight
                    }

                    Rectangle {
                        id: overlay

                        property color highlightedColor: Theme.rgba(
                                                             palette.highlightBackgroundColor,
                                                             Theme.highlightBackgroundOpacity)

                        anchors.fill: parent
                        color: delegateItem.highlighted ? highlightedColor : "transparent"
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

                        //% "Owned"
                        property string owned: qsTrId("common.ownershipShared")

                        //% "Shared"
                        property string shared: qsTrId("common.ownershipOwned")

                        text: albumIsShared ? owned : shared

                        color: delegateItem.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
