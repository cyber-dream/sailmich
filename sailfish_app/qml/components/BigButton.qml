import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    property alias text: label.text
    readonly property color _color: enabled ? highlighted ? Theme.highlightColor : Theme.primaryColor : Theme.secondaryColor

    height: Theme.itemSizeMedium

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: Theme.rgba(Theme.highlightBackgroundColor, 0.15)
            }
            GradientStop {
                position: 1.0
                color: "transparent"
            }
        }
    }

    Icon {
        source: "image://icon-m-ambience"
    }

    Row {
        spacing: Theme.paddingMedium
        Icon {
            source: "image://theme/icon-m-like"
            color: _color
        }
        Label {
            id: label
                        anchors {
                            verticalCenter: parent.verticalCenter

                        }
                        font.pixelSize: Theme.fontSizeLarge
            //            horizontalAlignment: Text.AlignRight
            //            truncationMode: TruncationMode.Fade
                        color: _color
        }
    }

    //    Image {
    //        id: image
    //        visible: parent.enabled
    //        anchors {
    //            right: parent.right
    //            verticalCenter: parent.verticalCenter
    //            rightMargin: Theme.horizontalPageMargin
    //        }
    //        source: "image://theme/icon-m-right?" + _color
    //    }
}
