import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Auth 1.0
import Immich.Init 1.0

Page {
    property SOAuthFlow authFlow
    property bool isBackToEnpointPage: false
    property bool _showContent: false

    Timer {
        id: appearanceTimer
        interval: 100
        onTriggered: {

            //            busyIndicator.opacity = 1
            //            label.opacity = 1
            //            oAuth2Info.opacity = 1
            _showContent = true
        }
    }

    PageBusyIndicator {
        id: busyIndicator

        running: !isBackToEnpointPage

        opacity: _showContent ? 1 : 0
        visible: !isBackToEnpointPage

        Behavior on opacity {
            FadeAnimation {}
        }
    }

    InfoLabel {
        id: label
        anchors.top: busyIndicator.bottom
        anchors.topMargin: Theme.paddingLarge * 5

        visible: !isBackToEnpointPage
                 && authFlow.authMethod !== SOAuthFlow.AuthMethodOAuth2
        //% "Authenticating"
        //: Info label on auth page
        text: qsTrId("authFlow.Authenticating")
        opacity: _showContent ? 1 : 0

        Behavior on opacity {
            FadeAnimation {}
        }
    }

    Column {
        id: oAuth2Info
        width: parent.width
        anchors {
            top: busyIndicator.bottom
            topMargin: Theme.paddingLarge * 5
            left: parent.left
            right: parent.right
            leftMargin: Theme.paddingLarge
            rightMargin: Theme.paddingLarge
        }

        spacing: Theme.paddingLarge

        visible: !isBackToEnpointPage
                 && authFlow.authMethod === SOAuthFlow.AuthMethodOAuth2

        opacity: 0

        Behavior on opacity {
            FadeAnimator {}
        }

        InfoLabel {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTrId("authFlow.WaitingForAuthorization")
        }

        InfoLabel {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTrId("authFlow.CopyLinkManuallyHint")

            font.pixelSize: Theme.fontSizeSmall
        }

        Item {
            width: parent.width
            height: Theme.paddingLarge * 3
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            Row {
                spacing: Theme.paddingSmall
                anchors.horizontalCenter: parent.horizontalCenter
                Icon {
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image://theme/icon-m-clipboard"
                    color: parent.parent.highlighted ? Theme.highlightColor : Theme.primaryColor
                }
                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTrId("authFlow.OAuth2LinkButton")
                    color: parent.parent.highlighted ? Theme.highlightColor : Theme.primaryColor
                }
            }

            onClicked: {
                Clipboard.text = authFlow.oAuth2Link
                // hint.show(qsTrId("authFlow.Copied")) // TODO
            }
        }
    }

    onStatusChanged: {
        switch (status) {
        case PageStatus.Activating:
            if (isBackToEnpointPage)
                return
            appearanceTimer.start()
            return
        case PageStatus.Active:
            break
        case PageStatus.Deactivating:
            if (isBackToEnpointPage)
                return
            busyIndicator.opacity = 0
            label.opacity = 0
            oAuth2Info.opacity = 0
            return
        default:
            return
        }

        if (isBackToEnpointPage) {
            CallLater.callLater(function () {
                pageStack.push([{
                                    "page": Qt.resolvedUrl(
                                                "EndpointVerifyPage.qml"),
                                    "properties": {
                                        "authFlow": authFlow
                                    }
                                }, {
                                    "page": Qt.resolvedUrl(
                                                "EndpointDialog.qml"),
                                    "properties": {
                                        "authFlow": authFlow
                                    }
                                }])
            })
            return
        }

        authFlow.startAuth()
    }

    Connections {
        target: authFlow

        onLoginCompleted: {
            Init.start()
        }

        onLoginFailed: {
            console.error(error)
            pageStack.push(Qt.resolvedUrl("AuthDialog.qml"), {
                               "authFlow": authFlow,
                               "errorMsg": error
                           }, {})
            return
        }

        onOAuth2LinkChanged: {
            if (!Qt.openUrlExternally(authFlow.oAuth2Link)) {
                console.log("Can't open browser for OAuth2")
            }

            oAuth2Info.opacity = 1
        }
    }
}
