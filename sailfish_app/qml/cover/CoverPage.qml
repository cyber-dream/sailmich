import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

    Icon {
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: -width / 5
        anchors.verticalCenterOffset: height / 3
        width: parent.width * 1.3
        height: parent.width * 1.3
        source: "../icons/icon-immich-grey.svg"
        sourceSize.width: width
        sourceSize.height: height
        opacity: 0.65
        cache: true
        color: Theme.primaryColor
    }

    Label {
        id: appName
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: Theme.paddingMedium
        anchors.rightMargin: Theme.paddingLarge
        text: "Sailmich" //TODO translate
        font.bold: true
        opacity: 0.6
    }
}
