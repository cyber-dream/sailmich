import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Secrets 1.0
import Immich.Init 1.0

Page {
    id: offlinePage

    Column {
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: Theme.paddingLarge
        InfoLabel {
            //% "Unable to connect to the Immich server"
            text: qsTrId("offlinePage.connectivityProblemError")
        }



        InfoLabel {
            //% "Please resolve the connection issue and open the app again or login in to different instance"
            //: Info label about problem with connectivity to the server
            text: qsTrId("offlinePage.connectivityProblemHint")
        }
    }

    Column {
        spacing: Theme.paddingSmall
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: Theme.paddingLarge * 6
        }

        RemorsePopup {
            id: remorseLogout
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            //% "retry"
            //: try to ping server button
            text: qsTrId("offlinePage.retryButton")
            onClicked: {
                Init.start()
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            //% "logout"
            //: logout button
            text: qsTrId("offlinePage.logoutButton")
            onClicked: {
                remorseLogout.execute(//% "Logging out"
                                      qsTrId(
                                          "settingsPage.logoutRemorseBannerTitle"),
                                      function () {
                                          Secrets.q_deleteAuthToken()
                                          openAuthDialog()
                                      })
            }
        }
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
