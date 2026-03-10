import QtQuick 2.0
import Sailfish.Silica 1.0
import "."

Item {
    id: root
    property list<DreamTab> tabs
    property int currentIndex: 0

    property PullDownMenu sharedMenu

    SlideshowView {
        id: slideshow
        anchors.fill: parent

        clip: true

        model: tabs
        currentIndex: root.currentIndex
        onCurrentIndexChanged: root.currentIndex = currentIndex

        delegate: SilicaFlickable {
            id: delegateFlickable
            width: slideshow.width
            height: slideshow.height

            Component.onCompleted: menu.createObject(delegateFlickable)

            Loader {
                id: bodyLoader
                anchors.fill: parent
                anchors.topMargin: tabRow.height + indicator.height + Theme.paddingLarge
                sourceComponent: body
                clip: true
            }
        }
    }

    Row {
        //TODO more space
        id: tabRow
        anchors.horizontalCenter: parent.horizontalCenter
        height: Theme.itemSizeSmall * .9

        spacing: Theme.paddingLarge * 2

        Repeater {
            id: tabRepeater
            model: tabs

            delegate: BackgroundItem {
                id: tabItem
                _showPress: false
                width: tabLabel.width

                onClicked: {
                    currentIndex = index
                }

                Label {
                    id: tabLabel
                    anchors.centerIn: parent
                    text: modelData.title
                    font.pixelSize: Theme.fontSizeLarge
                    color: currentIndex === index ? Theme.highlightColor : Theme.primaryColor
                }
            }
        }

        y: {
            var currentFlick = slideshow.currentItem
            return (currentFlick
                    && currentFlick.contentY < 0) ? -currentFlick.contentY : 0
        }
    }

    Rectangle {
        id: indicator
        height: 4
        radius: 2
        color: Theme.highlightColor

        anchors.top: tabRow.bottom
        property bool ready: false

        function updatePos() {
            var item = tabRepeater.itemAt(currentIndex)
            if (!item)
                return

            x = tabRow.x + item.x
            indicator.width = item.width
        }

        Behavior on width {
            enabled: indicator.ready
            NumberAnimation {
                duration: 200
            }
        }
        Behavior on x {
            enabled: indicator.ready
            NumberAnimation {
                duration: 200
            }
        }

        Timer {
            id: animTimer
            interval: 50
            onTriggered: indicator.ready = true
        }

        Connections {
            target: tabRepeater
            Component.onCompleted: {
                indicator.updatePos()
                animTimer.start()
            }
        }

        Connections {
            target: root
            onCurrentIndexChanged: {
                indicator.updatePos()
            }
        }
    }
}
