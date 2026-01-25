import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0
import "pages"

ApplicationWindow {
    initialPage: Component {
        Page {
            //% "Welcome to Sailmich"
            //: Greeting label on the main screen
            //Label{text: qsTrId("helloworld")}
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: Immich.initStatus
            }
        }
    }

    Component.onCompleted: {
        switch (Immich.initStatus) {
        case Immich.InitStatusNotStarted:
            //FIXME
            break
        case Immich.InitStatusStarted:
            Immich.initResult.finished.connect(function () {
                workWithInitResult(Immich.initStatus)
            })
            break
        case Immich.InitStatusComplete:
            workWithInitResult(Immich.initStatus)
            break
        default:
            //FIXME
            break
        }
    }

    function workWithInitResult(status) {
        status = 2
        switch (status) {
        case Immich.InitStatusBaseUrlNotFound:
            openAuthDialog()
            break
        case Immich.InitStatusAuthTokenNotFound:
            openAuthDialog()
            break
        case Immich.InitStatusApiPingFailed://FIXME
        case Immich.InitStatusTokenInvalid:
            openAuthDialog()
            break
//        case Immich.InitStatusComplete:
//            pageStack.replace(Qt.resolvedUrl("pages/MainPage.qml"), {})
//            break
        default:

            // FIXME
        }
    }
    function openAuthDialog() {
        var dialog = pageStack.replace(Qt.resolvedUrl("pages/AuthDialog.qml"), {
                                        "name": "aboa"
                                    }, PageStackAction.Immediate)
        dialog.accepted.connect(function () {
            console.log(dialog.name)
        })
    }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
