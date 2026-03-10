import QtQuick 2.0
import Sailfish.Silica 1.0

import "../../components/dream-tabs"
import "../../components"
import Immich.Timeline 1.0

DreamTab {
    id: root

    //% "Library"
    //: Tab title (favourites, trash, peoples list, map and etc.)
    title: qsTrId("libraryPage.titleLibrary")

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

    SilicaFlickable {
        width: parent.width;
        height: parent.height
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            MainPageButton {
                //% "Favorites"
                //: Button to see a list of favorites photos
                text: qsTrId("libraryPage.buttonFavorites")

                property FeTimelineParams _favouritesParams: FeTimelineParams {
                    is_favorite: true
                }

                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../TimelinePage.qml"), {
                                       "pageTitle"//% "Favorites"
                                       //: Button to see a list of favorites photos
                                       : qsTrId("libraryPage.buttonFavorites"),
                                       "timelineFetcherParams": _favouritesParams
                                   })
                }
            }

            MainPageButton {
                //% "Archived"
                //: Button to see a list of archived photos
                text: qsTrId("libraryPage.buttonArchived")

                property FeTimelineParams _archivedParams: FeTimelineParams {
                    visibility: FeTimelineParams.AssetVisibilityArchive
                }

                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../TimelinePage.qml"), {
                                       "pageTitle"//% "Archived"
                                       //: Button to see a list of archived photos
                                       : qsTrId("libraryPage.buttonArchived"),
                                       "timelineFetcherParams": _archivedParams
                                   })
                }
            }

            MainPageButton {
                //% "Trashed"
                //: Button to see a list of trashed photos
                text: qsTrId("libraryPage.buttonTrashed")

                property FeTimelineParams _trashedParams: FeTimelineParams {
                    //                    visibility: FeTimelineParams.Asset
                    is_trashed: true
                }

                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../TimelinePage.qml"), {
                                       "pageTitle"//% "Trashed"
                                       //: Button to see a list of trashed photos
                                       : qsTrId("libraryPage.buttonTrashed"),
                                       "pageDescription"//% "Trashed items will be permanently deleted after 30 days"
                                       //: Static description to album of trashed photos
                                       : qsTrId(
                                             "trashedPhotosAlbumPage.description"),
                                       "timelineFetcherParams": _trashedParams
                                   })
                }
            }

            //            MainPageButton {
            //                text: "Shared links"
            //            }

            //            MainPageButton {
            //                text: "Peoples"
            //            }
        }

        VerticalScrollDecorator {}
    }
}
