import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Timeline 1.0
import Immich.Album 1.0
import "../../components/dream-tabs"
import "../../components/timeline"

//TODO stacked
DreamTab {
    //% "Timeline"
    //: Tab title
    title: qsTrId("timelinePage.settings")

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

    Timeline {
        ModTimeline {
            id: mod
        }

        FeTimeline {
            id: fetcher
            model: mod
            params: FeTimelineParams {
                visibility: FeTimelineParams.AssetVisibilityTimeline
                with_partners: true
            }
        }

        Component.onCompleted: {
            fetcher.fetch()
        }

        model: fetcher.model

        header: Column {
            width: parent.width
        }
    }
}
