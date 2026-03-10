import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Timeline 1.0
import "../components"
import "../components/timeline"

Page {
    property var albumInfo: null

    FeTimeline {
        params: FeTimelineParams {
            album_id: "8263b966-dbd7-4077-87c4-be48533884dd" //albumInfo.id
        }
        model: tModel
    }

    ModTimeline {
        id: tModel
    }

    ThumbnailPulsator {
        id: loadingAnim
    }

    Timeline {
        model: tModel
        header: Column {
            width: parent.width
            //            spacing: Theme.paddingSmall
            PageHeader {
                id: pageHeader
                title: albumInfo.albumName
            }

            Row {
                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin
                spacing: Theme.paddingMedium
                Label {
                    text: dateRange()
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall

                    function dateRange(start, end) {
                        //                        const dateRange = tModel.getDateRange()
                        const dateRange = [new Date(2025, 10,
                                                    14), new Date(2025, 11, 19)]

                        var locale = Qt.locale()
                        var format = "MMM d, yyyy"

                        switch (dateRange.length) {
                        case 0:
                            return ""
                        case 1:
                            return dateRange[0].toLocaleDateString(locale,
                                                                   format)
                        case 2:
                            const startDate = dateRange[0]
                            const endDate = dateRange[1]

                            var str = endDate.toLocaleDateString(locale, format)

                            if (startDate.getFullYear(
                                        ) !== endDate.getFullYear())
                                return startDate.toLocaleDateString(
                                            locale, "MMM d, yyyy") + " - " + str

                            if (startDate.getMonth() !== endDate.getMonth() || startDate.getDate() !== endDate.getDate()) {
                                return startDate.toLocaleDateString(
                                            locale, "MMM d") + " - " + str
                            }

                            return str
                        }
                    }
                }

                Label {
                    // anchors.right: parent.right
                    // anchors.rightMargin: Theme.horizontalPageMargin
                    text: "•"
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                }

                Label {
                    //                    anchors.right: parent.right
                    //                    anchors.rightMargin: Theme.horizontalPageMargin
                    text: tModel.assetsCount() + " items" //TODO implement and translate
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                }
            }
            Item {
                height: Theme.paddingMedium
                width: parent.width
            }
            Label {
                anchors.right: parent.right
                //                     visible: false

                //                     anchors.bottom: parent.bottom
                //                     anchors.bottomMargin: -Theme.paddingMedium


                anchors.rightMargin: Theme.horizontalPageMargin

                width: parent.width - 2 * Theme.horizontalPageMargin
                text: albumInfo.description
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall


                horizontalAlignment: Text.AlignRight
                wrapMode: Text.WordWrap
            }
        }
    }
}
