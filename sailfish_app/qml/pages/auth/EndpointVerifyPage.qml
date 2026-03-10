import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Auth 1.0

Page {
    property SOAuthFlow authFlow

    Timer {
        id: appearanceTimer
        interval: 100
        onTriggered: {
            busy.opacity = 1
            label.opacity = 1
        }
    }

    PageBusyIndicator {
        id: busy
        running: true
        opacity: 0

        Behavior on opacity {
            FadeAnimation {}
        }
    }

    InfoLabel {
        id: label
        anchors.top: busy.bottom
        anchors.topMargin: Theme.paddingLarge * 5
        //% "Verifiyng enpoint"
        //: Info label for page while veryfing immich address
        text: qsTrId("authFlow.VerifyingEndpoint")
        opacity: 0

        Behavior on opacity {
            FadeAnimation {}
        }
    }

    onStatusChanged: {
        switch (status) {
        case PageStatus.Activating:
            appearanceTimer.start()
            return
        case PageStatus.Deactivating:
            busy.opacity = 0
            label.opacity = 0
            return
        case PageStatus.Active:
            break
        default:
            return
        }

        authFlow.verifyEndpointUrl()
    }

    Connections {
        target: authFlow
        onEndpointVerificationCompleted: {
            CallLater.callLater(function () {
                pageStack.push([{
                                    "page": Qt.resolvedUrl("LoginPage.qml"),
                                    "properties": {
                                        "authFlow": authFlow
                                    }
                                }, {
                                    "page": Qt.resolvedUrl("AuthDialog.qml"),
                                    "properties": {
                                        "authFlow": authFlow
                                    }
                                }])
            })
        }
        onEndpointVerificationFailed: {
            console.error(error)
            CallLater.callLater(function () {
                pageStack.push(Qt.resolvedUrl("EndpointDialog.qml"), {
                                   "authFlow": authFlow,
                                   "errorMsg": error
                               })
            })
        }
    }
}
