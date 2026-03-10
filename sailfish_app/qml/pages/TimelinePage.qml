import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Timeline 1.0
import "../components"
import "../components/timeline"

Page {
    property FeTimelineParams timelineFetcherParams
    property string pageTitle
    property string pageDescription

    property Component header

    FeTimeline {
        id: fetcher
        params: timelineFetcherParams
        model: ModTimeline {}
        Component.onCompleted: {
            fetch()
        }
    }

    ThumbnailPulsator {
        id: loadingAnim
    }

    Timeline {
        model: fetcher.model
        header: Column {
            width: parent.width

            PageHeader {
                id: pageHeader
                title: pageTitle
            }

            Label {
                anchors.right: parent.right

                anchors.rightMargin: Theme.horizontalPageMargin

                width: parent.width - 2 * Theme.horizontalPageMargin
                text: pageDescription
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall

                horizontalAlignment: Text.AlignRight
                wrapMode: Text.WordWrap
            }
        }
    }
}
