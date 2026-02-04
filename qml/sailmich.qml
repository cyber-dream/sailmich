import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0
import "pages"

import "./components/mediaviewer/"

ApplicationWindow {

    //    initialPage: Component {
    //        Page {
    //            PageBusyIndicator {
    //                id: busyIndicator
    //                running: true
    //            }

    //            InfoLabel {
    //                id: errorLabel
    //                anchors.top: busyIndicator.top
    //                anchors.topMargin: Theme.paddingLarge * 10
    //                visible: text.length === 0 ? false : true
    //                layer.enabled: true
    //                layer.effect: ShaderEffect {
    //                    property color color: Theme.errorColor
    //                    fragmentShader: "
    //varying highp vec2 qt_TexCoord0;
    //uniform sampler2D source;
    //uniform lowp vec4 color;
    //uniform lowp float qt_Opacity;
    //void main() {
    //gl_FragColor = texture2D(source, qt_TexCoord0).a * color * qt_Opacity;
    //}"
    //                }
    //            }

    //        }
    //    }
    Component.onCompleted: {
        var callback = function () {
            Immich.isInitFinishedChanged.disconnect(callback)
            workWithInitResult(Immich.initStatus)
        }

        if (Immich.isInitFinished)
            workWithInitResult(Immich.initStatus)
        else {
            Immich.isInitFinishedChanged.connect(callback)
            console.log("waiting for Immich init finish")
        }
    }

    Page {
        id: fatalErrorPage
        property alias text: errorLabel.text
        InfoLabel {
            id: errorLabel
            anchors.top: busyIndicator.top
            anchors.topMargin: Theme.paddingLarge * 10
            visible: text.length === 0 ? false : true
            layer.enabled: true
            layer.effect: ShaderEffect {
                property color color: Theme.errorColor
                fragmentShader: "
varying highp vec2 qt_TexCoord0;
uniform sampler2D source;
uniform lowp vec4 color;
uniform lowp float qt_Opacity;
void main() {
gl_FragColor = texture2D(source, qt_TexCoord0).a * color * qt_Opacity;
}"
            }
        }
    }

    function workWithInitResult(status) {
        console.log("Immich init status:", status)
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
            console.error("unknown Immich init status:", status)
            pageStack.replaceAbove(null, fatalErrorPage)

            fatalErrorPage.text = "unknown Immich init status: " + status
            // TODO
        }
    }

    function openMainPage() {
        if (pageStack.busy) {
            _waitingToOpenMain = true
            return
        }

        // Используем replaceAbove(null, ...) для полной очистки
        pageStack.replaceAbove(null, Qt.resolvedUrl("pages/MainPage.qml"))
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
            anchors.bottomMargin: Theme.paddingLarge * 6
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
