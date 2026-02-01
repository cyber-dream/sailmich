import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0
import "../components/timeline"
import "../components"
import "."

Page {
    id: mainPage

    TimelineAlbumModel {
        id: tModel
    }

    ThumbnailPulsator {
        id: loadingAnim
    }

    Component.onCompleted: {
        const resAlbums = Albums.q_updateAlbumsListModel(GlobalAlbumsList)
        resAlbums.finished.connect(function () {
            if (!resAlbums.isSucceeded()) {
                console.error(resAlbums.error())
            } else {
                console.log("albums model updated", GlobalAlbumsList.rowCount())
            }
        })
        const r = Albums.q_updateAlbumModel(tModel)
        r.finished.connect(function () {
            if (!resAlbums.isSucceeded()) {
                console.error(r.error())
            } else {
                console.log("updateAlbumModels no errors")
            }
        })
    }

    Timeline {
                model: tModel
        PullDownMenu {
            MenuItem {
                //% "Setting"
                //: Settings page pulley menu item
                text: qsTrId("mainPage.settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
        }

        header: Column {
            width: parent.width

            MainPageButton {
                //% "Albums"
                //: Albums page button
                text: qsTrId("mainPage.Albums")
                onClicked: pageStack.push(Qt.resolvedUrl("AlbumsPage.qml"))
            }

            AlbumsFeaturing{}
        }
    }
}
