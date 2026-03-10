import QtQuick 2.0
import Sailfish.Silica 1.0
import "."
import "../../components/dream-tabs"

Page {
    TabTimeline {
        id: tabTimeline
    }

    TabAlbums {
        id: tabAlbums
    }

    TabLibrary{
        id: tabLibrary
    }

    DreamTabView {
        width: parent.width
        height: parent.height

//        sharedMenu:  PullDownMenu{
//            MenuItem{
//                text: "Aboba"
//            }
//        }

        tabs: [
            tabTimeline, //b
            tabAlbums, //a
            tabLibrary,
        ]
    }
}
