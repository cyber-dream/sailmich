import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Timeline 1.0

Page {
    property ModTimebucketAsset asset

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
                value: asset.id.toString().replace(/{|}/g, "").toLowerCase()
            }

            DetailItem {
                //% "Created at"
                //: Date of asset creation
                label: qsTrId("common.createdAt")
                value: asset.file_created_at
            }

            DetailItem {
                //% "Owner Id"
                //: Uuid of owner account
                label: qsTrId("common.ownerId")
                value: asset.owner_id.toString().replace(/{|}/g,
                                                         "").toLowerCase()
            }

            DetailItem {
                //% "City"
                //: City where photo created
                label: qsTrId("common.city")
                value: asset.city
            }
            DetailItem {
                //% "Country"
                //: Country where photo created
                label: qsTrId("common.country")
                value: asset.country
            }
            DetailItem {
                //% "Live Id"
                //: Uuid of live photo/video asset
                label: qsTrId("common.livePhotoVideoId")
                value: asset.livePhotoVideoId.replace(/{|}/g, "").toLowerCase()
            }
            //            DetailItem {
            //                //% "Projection Type"
            //                //: Type of asset projection (plain or 360)
            //                label: qsTrId("common.projectionType")
            //                value: asset.projectionType
            //            }
            //            DetailItem {
            //                //% "Thumbhash"
            //                //: Bytecoded preview of asset
            //                label: qsTrId("common.thumbhash")
            //                value: asset.thumbhash
            //            }
            //            DetailItem {
            //                //% "Stack"
            //                //: Information about assets stack (union of similar photos)
            //                label: qsTrId("common.stack")
            //                value: asset.stack
            //            }
            DetailItem {
                //% "Duration"
                //: Duration of asset
                label: qsTrId("common.duration")
                value: asset.duration
                visible: !asset.is_image
            }
            DetailItem {
                //% "Is Favourite"
                //: Favourite or not bool flag
                label: qsTrId("common.isFavorite")
                value: "" + asset.is_favorite
            }
            //            DetailItem {
            //TODO type enum in app src
            //                    text: "isImage"
            //                value: asset.isImage
            //            }
            DetailItem {
                //% "Is Trashed"
                //: Is this asset trashed bool flag
                label: qsTrId("common.isTrashed")
                value: "" + asset.is_trashed
            }
            //            DetailItem { //TODO WTF???
            //                label: "ratio"
            //                value: asset.ratio
            //            }
            DetailItem {
                //% "Local offset"
                //: Time Zone Offset from UTC+0 in hours
                label: qsTrId("common.localOffsetHours")
                value: asset.local_offset_hours
            }
            DetailItem {
                //% "Latitude"
                //: -
                label: qsTrId("common.latitude")
                value: asset.latitude
            }
            DetailItem {
                //% "Longitude"
                //: -
                label: qsTrId("common.longitude")
                value: asset.longitude
            }
            //            DetailItem {
            //                //% "Visibility"
            //                //: Visibilty of asset (archive, timeline, hidden, locked)
            //                label: qsTrId("common.assetVisibility")
            //                value: asset.visibility
            //            }
        }
    }
}
