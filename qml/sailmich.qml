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
        pageStack.clear()
        pageStack.push(Qt.resolvedUrl("pages/MainPage.qml"), {})
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

        Column {
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Theme.paddingLarge
            InfoLabel {
                text: "Can't connect to Immich server. Fix connectivity or choose another instance"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "logout"
                onClicked: {
                    Immich.auth.q_logout()
                    openAuthDialog()
                }
            }
        }
    }

    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
