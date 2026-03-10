import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Auth 1.0
import ".."

Dialog {
    property SOAuthFlow authFlow

    property string errorMsg

    backNavigation: false
    canAccept: addressField.acceptableInput

    IconButton {
        icon.source: "../../icons/icon-m-bug.png"
                     + (pressed ? Theme.highlightColor : Theme.primaryColor)
        onClicked: pageStack.push(Qt.resolvedUrl("../DebugPage.qml"))

        anchors {
            bottom: parent.bottom
            left: parent.left
            leftMargin: Theme.paddingSmall
            bottomMargin: Theme.paddingSmall
        }
    }

    Column {
        width: parent.width
        spacing: Theme.paddingLarge
        DialogHeader {
            //% "Server Endpoint URL"
            //: Label about account type
            title: qsTrId("authFlow.serverEndpointURL")
            //% "Continue"
            //: Dialog header accept option
            acceptText: qsTrId("authFlow.Continue")
            cancelText: ""
        }

        TextField {
            id: addressField

            validator: RegExpValidator {
                regExp: /^https?:\/\/[\w\.-]+(?:\.[\w-]{2,32})?(?::\d+)?\/?$/
            }

            //% "Your server address"
            //: Form address field name
            placeholderText: qsTrId("authFlow.serverEndpointURL")
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
            errorHighlight: !acceptableInput && text.length > 0

            label: {
                if (text.length > 0 && !acceptableInput)
                    //% "Endpoint address with http(s) and domain or ip:port"
                    //: Address format hint in auth form (showing when regexp validator failed)
                    return qsTrId("authFlow.addressHint")

                //% "Address"
                //: Form address field name
                return qsTrId("authFlow.addressFieldName")
            }

            labelVisible: text.length > 0 || (!acceptableInput
                                              && text.length > 0)
            onTextChanged: {
                if (!authFlow)
                    return
                authFlow.endpointUrl = text
            }
            Component.onCompleted: {
                text = authFlow.endpointUrl
            }
        }

        TextSwitch {
            id: ignoreSSLErrorsSwitch
            //% "Ignore SSL errors"
            //: Endpoint connection form option
            text: qsTrId("authFlow.IgnoreSSLErrors")

            checked: authFlow.isIgnoreSSLErrors

            onCheckedChanged: {
                if (!authFlow)
                    return
                authFlow.isIgnoreSSLErrors = ignoreSSLErrorsSwitch.checked
            }
        }

        InfoLabel {
            id: errorLabel
            visible: errorMsg.length === 0 ? false : true
            //% "Error"
            //: "Error" prefix for prepend error message (Error: *error text*)
            text: qsTrId("common.Error") + ": " + errorMsg
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
}
