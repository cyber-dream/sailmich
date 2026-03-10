import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Auth 1.0

Dialog {
    id: dialog

    property SOAuthFlow authFlow
    property string errorMsg: ""

    canAccept: {
        switch (authFlow.authMethod) {
        case SOAuthFlow.AuthMethodLoginPassword:
            return emailField.acceptableInput && passwordField.acceptableInput
        case SOAuthFlow.AuthMethodOAuth2:
            return true
        case SOAuthFlow.AuthMetodApiToken:
            return apiTokenField.acceptableInput
        }
    }

    onRejected: {
        pageStack.previousPage(this).isBackToEnpointPage = true
    }

    Column {
        width: parent.width
        spacing: Theme.paddingLarge
        DialogHeader {
            //% "Immich Account"
            //: Label about account type
            title: qsTrId("authFlow.immichAccount")
            //% "Login"
            //: DialogPage header start login button
            acceptText: qsTrId("authFlow.Login")
            //% "Change enpoint"
            //: DialogPage header start login button
            cancelText: qsTrId("authFlow.ChangeEndpoint")
        }

        ComboBox {
            id: authComboBox

            //% "disabled on server"
            //: Hint shows when auth method disabled in server config
            readonly property string hintDisabledOnServer: " (" + qsTrId(
                                                               "authFlow.DisabledOnServerHint")
                                                           + ")"

            width: parent.width
            //% "Auth Metod"
            label: qsTrId("authFlow.AuthMetod")

            menu: ContextMenu {

                MenuItem {
                    //% "Password Login"
                    //: Name of auth method in comboBox
                    text: qsTrId("authFlow.PasswordLoginAuthMethod")
                          + (enabled ? "" : authComboBox.hintDisabledOnServer)
                    enabled: authFlow.features.passwordLogin
                }
                MenuItem {
                    //% "OAuth2"
                    //: Name of auth method in comboBox
                    text: qsTrId("authFlow.OAuth2AuthMetod")
                          + (enabled ? "" : authComboBox.hintDisabledOnServer)
                    enabled: authFlow.features.oauth
                }
                MenuItem {
                    //% "API key"
                    //: Name of auth method in comboBox
                    text: qsTrId("authFlow.APIKeyAuthMetod")
                }
            }

            currentIndex: authFlow.authMethod

            onCurrentIndexChanged: {
                authFlow.authMethod = currentIndex
            }

            Component.onCompleted: {
                if (currentIndex === 0 && !authFlow.features.passwordLogin) {
                    currentIndex = authFlow.features.oauth ? 1 : 2
                } else if (currentIndex === 1 && !authFlow.features.oauth) {
                    currentIndex = 2
                }
            }
        }

        Separator {
            width: parent.width
            horizontalAlignment: Qt.AlignHCenter
            color: Theme.secondaryColor
        }

        TextField {
            id: emailField
            width: parent.width

            //% "Email"
            //: Form email field name
            placeholderText: qsTrId("authFlow.emailFieldName")
            validator: RegExpValidator {
                regExp: /^[^@\s]+@[^@\s]+$/
            }

            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
            errorHighlight: !acceptableInput && text.length > 0

            label: {
                if (text.length > 0 && !acceptableInput) {
                    //% "Email of Immich account"
                    //: Email format hint in auth form (showing when regexp validator failed)
                    return qsTrId("authFlow.emailHint")
                }
                //% "Email"
                //: Form email field name
                return qsTrId("authFlow.emailFieldName")
            }
            labelVisible: text.length > 0 || (!acceptableInput
                                              && text.length > 0)

            onTextChanged: {
                if (!authFlow)
                    return
                authFlow.email = text
            }

            visible: authFlow.authMethod === SOAuthFlow.AuthMethodLoginPassword
        }

        PasswordField {
            id: passwordField

            //% "Password"
            //: Form password field name
            placeholderText: qsTrId("authFlow.passwordFieldName")
            inputMethodHints: Qt.ImhNoPredictiveText
            acceptableInput: text.length > 0
            errorHighlight: !acceptableInput && text.length > 0

            //% "Password"
            //: Form password field name
            label: qsTrId("authFlow.passwordFieldName")

            onTextChanged: {
                if (!authFlow)
                    return
                authFlow.password = text
            }

            width: parent.width

            visible: authFlow.authMethod === SOAuthFlow.AuthMethodLoginPassword
        }

        InfoLabel {
            //% "Swipe left to open the browser and log in via OAuth2"
            //: Hint to continue OAuth2 auth in credintial form
            text: qsTrId("authFlow.oAuth2ContinueHint")
            visible: authFlow.authMethod === SOAuthFlow.AuthMethodOAuth2
        }

        PasswordField {
            id: apiTokenField

            //% "Api Token"
            //: Form api token field name
            placeholderText: qsTrId("authFlow.apiTokenFieldName")
            inputMethodHints: Qt.ImhNoPredictiveText
            acceptableInput: text.length > 0
            errorHighlight: !acceptableInput && text.length > 0

            //% "Api Token"
            //: Form api token field name
            label: qsTrId("authFlow.apiTokenFieldName")

            text: authFlow.apiToken

            onTextChanged: {
                if (!authFlow)
                    return
                authFlow.apiToken = text
            }
            Component.onCompleted: authFlow.apiToken = text

            width: parent.width

            visible: authFlow.authMethod === SOAuthFlow.AuthMetodApiToken
        }

        //TODO: translatable errors
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
