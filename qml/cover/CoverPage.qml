import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

    //    Label {
    //        id: label
    //        anchors.centerIn: parent
    //        text: qsTr("Aboba")
    //    }
    Image {
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: -width / 5
        anchors.verticalCenterOffset: height / 3
        width: parent.width * 1.3
        height: parent.width * 1.3
        source: "../icons/icon-immich-grey.svg"
        sourceSize.width: width
        sourceSize.height: height
        opacity: 0.4
    }

    Label {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: Theme.paddingLarge
        text: "Sailmich"
    }

    //    CoverActionList {
    //        id: coverAction

    //        CoverAction {
    //            iconSource: "image://theme/icon-cover-next"
    //        }

    //        CoverAction {
    //            iconSource: "image://theme/icon-cover-pause"
    //        }
    //    }
}
