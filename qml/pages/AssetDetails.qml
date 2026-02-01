import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    property var assetData
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height

        Column {
            id: col
            width: parent.width
            spacing: Theme.paddingSmall

            PageHeader {
                //% "About"
                //: Title of asset details page
                title: qsTrId("assetDetailsPage.titleAbout")
            }

            DetailItem {
                //% "Asset ID"
                //: Uuid of Immich asset
                label: qsTrId("common.assetId")
                value: assetData.assetId.toString().replace(/{|}/g,"").toLowerCase()
            }

            DetailItem {
                //% "Created at"
                //: Date of asset creation
                label: qsTrId("common.createdAt")
                value: assetData.fileCreatedAt
            }

            DetailItem {
                //% "Owner Id"
                //: Uuid of owner account
                label: qsTrId("common.ownerId")
                value: assetData.ownerId.toString().replace(/{|}/g,"").toLowerCase()
            }

            DetailItem {
                //% "City"
                //: City where photo created
                label: qsTrId("common.city")
                value: assetData.city
            }
            DetailItem {
                //% "Country"
                //: Country where photo created
                label: qsTrId("common.country")
                value: assetData.country
            }
            DetailItem {
                //% "Live Id"
                //: Uuid of live photo/video asset
                label: qsTrId("common.livePhotoVideoId")
                value: assetData.livePhotoVideoId
            }
            DetailItem {
                //% "Projection Type"
                //: Type of asset projection (plain or 360)
                label: qsTrId("common.projectionType")
                value: assetData.projectionType
            }
            DetailItem {
                //% "Thumbhash"
                //: Bytecoded preview of asset
                label: qsTrId("common.thumbhash")
                value: assetData.thumbhash
            }
            DetailItem {
                //% "Stack"
                //: Information about assets stack (union of similar photos)
                label: qsTrId("common.stack")
                value: assetData.stack
            }
            DetailItem {
                //% "Duration"
                //: Duration of asset
                label: qsTrId("common.duration")
                value: assetData.duration
            }
            DetailItem {
                //% "Is Favourite"
                //: Favourite or not bool flag
                label: qsTrId("common.isFavorite")
                value: assetData.isFavorite
            }
//            DetailItem {
                //TODO type enum in app src
//                    text: "isImage"
//                value: assetData.isImage
//            }
            DetailItem {
                //% "Is Trashed"
                //: Is this asset trashed bool flag
                label: qsTrId("common.isTrashed")
                value: assetData.isTrashed
            }
//            DetailItem { //TODO WTF???
//                label: "ratio"
//                value: assetData.ratio
//            }
            DetailItem {
                //% "Local offset"
                //: Time Zone Offset from UTC+0 in hours
                label: qsTrId("common.localOffsetHours")
                value: assetData.localOffsetHours
            }
            DetailItem {
                //% "Latitude"
                //: -
                label: qsTrId("common.latitude")
                value: assetData.latitude
            }
            DetailItem {
                //% "Longitude"
                //: -
                label: qsTrId("common.longitude")
                value: assetData.longitude
            }
            DetailItem {
                //% "Visibility"
                //: Visibilty of asset (archive, timeline, hidden, locked)
                label: qsTrId("common.assetVisibility")
                value: assetData.visibility
            }
        }
    }
}
