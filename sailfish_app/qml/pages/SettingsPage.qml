import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Secrets 1.0

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        RemorsePopup {
            id: remorseLogout
        }

        PullDownMenu {
            MenuItem {
                //% "Logout"
                //: Logout pulley menu item
                text: qsTrId("settingsPage.pulleyLogout")
                onClicked: {
                    remorseLogout.execute(//% "Logging out"
                                          qsTrId(
                                              "settingsPage.logoutRemorseBannerTitle"),
                                          function () {
                                              Secrets.q_deleteAuthToken()
                                              window.openAuthDialog()
                                          })
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

    IconButton {
        icon.source: "../icons/icon-m-bug.png"
                     + (pressed ? Theme.highlightColor : Theme.primaryColor)
        onClicked: pageStack.push(Qt.resolvedUrl("DebugPage.qml"))

        anchors {
            bottom: parent.bottom
            left: parent.left
            leftMargin: Theme.paddingSmall
            bottomMargin: Theme.paddingSmall
        }
    }
}
