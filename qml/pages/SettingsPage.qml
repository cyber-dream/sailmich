import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                //% "Logout"
                //: Logout pulley menu item
                text:  qsTrId("settingsPage.pulleyLogout")
                onClicked: {
                    Immich.auth.q_logout()
                    //                    pageStack.clear()
                    //                    pageStack.replace(Qt.resolvedUrl("AuthPage.qml"), Qt.resolvedUrl("AuthDialog.qml"))
                    window.openAuthDialog()
                }
            }
        }

        Column {
            id: column
            width: parent.width

            PageHeader {
                //% "Settings"
                //: Settings page title
                title: qsTrId("settingsPage.titleSettings")
            }

            //            IconButton {
            //                icon.source: "../icons/icon-m-bug.png"
            //                             + (pressed ? Theme.highlightColor : Theme.primaryColor)
            //                onClicked: pageStack.push(Qt.resolvedUrl("DebugPage.qml"))
            //            }
            InfoLabel {
                text: "Use pulley menu to logout"
            }
        }

        VerticalScrollDecorator {}
    }
}
