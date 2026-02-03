import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0
import "pages"

import "./components/mediaviewer/"

ApplicationWindow {
    initialPage: Component {
        Page {
            PageBusyIndicator {
                running: true
            }
        }
    }

    Component.onCompleted: {
        var callback = function () {
            Immich.isInitFinishedChanged.disconnect(callback)
            workWithInitResult(Immich.initStatus)
        }

        if (Immich.isInitFinished)
            workWithInitResult(Immich.initStatus)
        else
            Immich.isInitFinishedChanged.connect(callback)
    }

    function workWithInitResult(status) {
        switch (status) {
        case Immich.InitStatusBaseUrlNotFound:
            openAuthDialog()
            break
        case Immich.InitStatusAuthTokenNotFound:
            openAuthDialog()
            break
        case Immich.InitStatusApiPingFailed:
            openOfflinePage()
            break
        case Immich.InitStatusTokenInvalid:
            openAuthDialog()
            break
        case Immich.InitStatusComplete:
            openMainPage()
            break
        default:

            // TODO
        }
    }

    function openMainPage() {
        if (pageStack.busy) {
            openMainPageTimer.start()
            return
        }
        pageStack.clear()
        pageStack.push(Qt.resolvedUrl("pages/MainPage.qml"), {})
    }

    Timer {
        id: openMainPageTimer
        interval: 100
        repeat: false

        onTriggered: openMainPage()
    }

    function openAuthDialog(error) {
        //        pageStack.clear()
        pageStack.push([loadingPageComponent, authDialogComponent], {})
    }

    function openOfflinePage() {
        pageStack.clear()
        pageStack.push([offlinePage], {})
    }

    Component {
        id: authDialogComponent
        AuthDialog {}
    }

    Component {
        id: loadingPageComponent
        AuthPage {}
    }

    Page {
        id: offlinePage

        InfoLabel {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Unable to connect to the Immich server \n \nPlease resolve the connection issue and open the app again or login in to different instance"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.paddingLarge*6
            text: "logout"
            onClicked: {
                Immich.auth.q_logout()
                openAuthDialog()
            }
        }
    }

    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
