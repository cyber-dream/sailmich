import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: "Logout"
                onClicked: {
                    Immich.auth.q_logout()
                    pageStack.replace(Qt.resolvedUrl("InitPage.qml"))
                }
            }
        }

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: "Settings"
            }

//            IconButton {
//                icon.source: "../icons/icon-m-bug.png"
//                             + (pressed ? Theme.highlightColor : Theme.primaryColor)
//                onClicked: pageStack.push(Qt.resolvedUrl("DebugPage.qml"))
//            }

            InfoLabel{text: "Use pulley menu to logout"}
        }

        VerticalScrollDecorator {}
    }
}
