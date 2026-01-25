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
                title: "About"
            }

            DetailItem {
                label: "Asset Id"
                value: assetData.assetId.toString().replace(/{|}/g,"").toLowerCase()
            }

            DetailItem {
                label: "Created"
                value: assetData.fileCreatedAt
            }

            DetailItem {
                label: "ownerId"
                value: assetData.ownerId.toString().replace(/{|}/g,"").toLowerCase()
            }

            DetailItem {
                label: "city"
                value: assetData.city
            }
            DetailItem {
                label: "country"
                value: assetData.country
            }
            DetailItem {
                label: "livePhotoVideoId"
                value: assetData.livePhotoVideoId
            }
            DetailItem {
                label: "projectionType"
                value: assetData.projectionType
            }
            DetailItem {
                label: "thumbhash"
                value: assetData.thumbhash
            }
            DetailItem {
                label: "stack"
                value: assetData.stack
            }
            DetailItem {
                label: "duration"
                value: assetData.duration
            }
            DetailItem {
                label: "isFavorite"
                value: assetData.isFavorite
            }
            DetailItem {
                label: "isImage"
                value: assetData.isImage
            }
            DetailItem {
                label: "isTrashed"
                value: assetData.isTrashed
            }
            DetailItem {
                label: "ratio"
                value: assetData.ratio
            }
            DetailItem {
                label: "localOffsetHours"
                value: assetData.localOffsetHours
            }
            DetailItem {
                label: "latitude"
                value: assetData.latitude
            }
            DetailItem {
                label: "longitude"
                value: assetData.longitude
            }
            DetailItem {
                label: "visibility"
                value: assetData.visibility
            }
        }
    }
}
