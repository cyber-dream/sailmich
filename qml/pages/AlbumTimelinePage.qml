import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0
import "../components"
import "../components/timeline"

Page {
    property var albumInfo: null

    TimelineAlbumModel {
        id: tModel
        albumId: albumInfo.id
    }

    ThumbnailPulsator {
        id: loadingAnim
    }

    Component.onCompleted: {
        if (albumInfo === null) {
            console.error("albumInfo is null")
            return
        }

        console.log(albumInfo.id, albumInfo.albumName)

        const r = Albums.q_updateAlbumModel(tModel, albumInfo.id)
        r.finished.connect(function () {
            if (!r.code().isSucceeded()) {
                console.error(r.error)
            } else {
                console.log("updateAlbumModels no errors")
            }
        })
    }

    Timeline {
        model: tModel
        //        PullDownMenu {
        //            MenuItem {
        //                text: "Settings"
        //                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
        //            }
        //        }
        header: Column {
            width: parent.width
            PageHeader {
                title: albumInfo.albumName
            }
        }
    }
}
