import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Auth 1.0
import Immich.Init 1.0
import Immich.Secrets 1.0

ApplicationWindow {
    id: appWindow

    initialPage: initPage

    Component {
        id: initPage

        Page {
            property bool showBusyIndication: false

            PageBusyIndicator {
                id: busyIndicator
                running: parent.showBusyIndication

                Behavior on running {
                    FadeAnimator {}
                }
            }

            InfoLabel {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: busyIndicator.bottom
                    topMargin: 4 * Theme.paddingLarge
                }
                //% "Initialization"
                text: qsTrId("initPage.intitalizationLabel")
                opacity: parent.showBusyIndication ? 1 : 0

                Behavior on opacity {
                    FadeAnimator {}
                }
            }

            Timer {
                id: delayTimer
                interval: 1500
                repeat: false
                onTriggered: parent.showBusyIndication = true
            }

            Component.onCompleted: {
                delayTimer.start()
                Init.start()
            }

            IconButton {
                icon.source: "../icons/icon-m-bug.png"
                             + (pressed ? Theme.highlightColor : Theme.primaryColor)
                onClicked: pageStack.push(Qt.resolvedUrl(
                                              "./pages/DebugPage.qml"))

                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: Theme.paddingSmall
                    bottomMargin: Theme.paddingSmall
                }
            }
        }
    }
    Connections {
        target: Init
        onFinished: {
            switch (Init.initStatus) {
            case Init.StatusEndpointUrlNotFound:
                console.log("Immich init status:", "UrlNotFound")
                openAuthDialog()
                break
            case Init.StatusAuthTokenNotFound:
                console.log("Immich init status:", "AuthTokenNotFound")
                openAuthDialog()
                break
            case Init.StatusApiPingFailed:
                console.log("Immich init status:", "ApiPingFailed")
                openOfflinePage()
                break
            case Init.StatusTokenInvalid:
                console.log("Immich init status:", "TokenInvalid")
                openAuthDialog()
                break
            case Init.StatusComplete:
                console.log("Immich init status:", "Complete")
                openMainPage()
                break
            default:
                console.error("unknown Immich init status:", status)
                pageStack.clear()
                pageStack.push(Qt.resolvedUrl("pages/FatalErrorPage.qml"), {
                                   "text": "Unknown Immich init status: " + status
                               }) // TODO translate
            }
        }
    }

    function openMainPage() {
        CallLater.callLater(function () {
            pageStack.replaceAbove(null,
                                   Qt.resolvedUrl("pages/main/MainPage.qml"))
        })
    }

    function openAuthDialog(error) {
        const authFlow = Qt.createQmlObject(
                           'import Immich.Auth 1.0; SOAuthFlow{}', appWindow)

        CallLater.callLater(function () {
            pageStack.push([{
                                "page": Qt.resolvedUrl(
                                            "pages/auth/EndpointVerifyPage.qml"),
                                "properties": {
                                    "authFlow": authFlow
                                }
                            }, {
                                "page": Qt.resolvedUrl(
                                            "pages/auth/EndpointDialog.qml"),
                                "properties": {
                                    "authFlow": authFlow
                                }
                            }])
        })
    }

    function openOfflinePage() {
        pageStack.clear()
        pageStack.push(Qt.resolvedUrl("pages/OfflinePage.qml"))
    }

    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
