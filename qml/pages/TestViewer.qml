import QtQuick 2.0
import Sailfish.Silica 1.0
//import Sailfish.Silica.private 1.0
import Sailfish.Gallery.private 1.0
import "../components"

Page {
//    Button {
//        anchors.verticalCenter: parent.verticalCenter
//        anchors.horizontalCenter:  parent.horizontalCenter
//        text: "Invoke"
//        onClicked: {pv.visible = true}
//    }

    ListModel {
        id: galleryModel
        ListElement {
            url: "image://theme/icon-m-share-gallery"
            isImage: true
        }
//        ListElement {
//            url: "file:///home/nemo/Videos/b.mp4"
//            isImage: false
//        }
        ListElement {
            url: "image://theme/icon-m-share-bluetooth"
            isImage: true
        }
    }

    PathView {
        id: view
        anchors.fill: parent
        model: galleryModel
        interactive: true
        highlightRangeMode: PathView.StrictlyEnforceRange
        preferredHighlightBegin: width / 2
        preferredHighlightEnd: width / 2
        snapMode: PathView.SnapOneItem
        focus: true

        path: Path {
            startX: -width / 2; startY: height / 2
            PathLine { x: width * (galleryModel.count - 0.5); y: height / 2 }
        }

        delegate: Item {
            width: view.width
            height: view.height

            GalleryOverlay {
                anchors.fill: parent
                previewPath: model.url
            }
        }
    }
}
