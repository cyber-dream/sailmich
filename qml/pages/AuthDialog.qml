import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    backNavigation: false
//    property string authErrorMessage

    //    property alias address: addressField.text
    //    property alias email: emailField.text
    //    property alias password: passwordField.text
    property string errorMsg: ""

    canAccept: addressField.acceptableInput && emailField.acceptableInput
               && passwordField.acceptableInput

    onAccepted: {

        pageStack.previousPage(this).address = addressField.text
        pageStack.previousPage(this).email = emailField.text
        pageStack.previousPage(this).password = passwordField.text
        //        pageStack.push(loadingPage, {
        //                           "address": addressField.text,
        //                           "email": emailField.text,
        //                           "password": passwordField.text
        //                       })
    }

    Column {
        width: parent.width

        DialogHeader {
            //% "Immich Account"
            //: Label about account type
            title: qsTrId("authPage.immichAccount")
            //% "Login"
            //: DialogPage header start login button
            acceptText: qsTrId("authPage.Login")
            cancelText: ""
        }

        TextField {
            id: addressField
            validator: RegExpValidator {
                regExp: /^https?:\/\/[\w\.-]+(?:\.[\w-]{2,32})?(?::\d+)?\/?$/
            }
            //% "Address"
            //: Form address field name
            placeholderText: qsTrId("authPage.addressFieldName")
            inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
            errorHighlight: !acceptableInput && text.length > 0


            label: {
                if (text.length > 0 && !acceptableInput) {
                    //% "Immich address with http/https protocol"
                    //: Address format hint in auth form (showing when regexp validator failed)
                    return qsTrId("authPage.addressHint")
                }
                //% "Address"
                //: Form address field name
                return qsTrId("authPage.addressFieldName")
            }

            labelVisible: text.length > 0 || (!acceptableInput
                                              && text.length > 0)
        }

        TextField {
            id: emailField
            //% "Email"
            //: Form email field name
            placeholderText: qsTrId("authPage.emailFieldName")
            validator: RegExpValidator {
                regExp: /^[\w-\.]+@[\w-]+(\.[\w-]{2,4})?$/
            }

            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoPredictiveText
            errorHighlight: !acceptableInput && text.length > 0

            label: {
                if (text.length > 0 && !acceptableInput) {
                    //% "Email of Immich account"
                    //: Email format hint in auth form (showing when regexp validator failed)
                    return qsTrId("authPage.emailHint")
                }
                //% "Email"
                //: Form email field name
                return qsTrId("authPage.emailFieldName")
            }

            labelVisible: text.length > 0 || (!acceptableInput
                                              && text.length > 0)


        }

        PasswordField {
            id: passwordField
            //% "Password"
            //: Form password field name
            placeholderText: qsTrId("authPage.passwordFieldName")
            inputMethodHints: Qt.ImhNoPredictiveText
            acceptableInput: text.length > 0
            errorHighlight: !acceptableInput && text.length > 0



            //% "Email"
            //: Form password field name
            label: qsTrId("authPage.passwordFieldName")
        }

        InfoLabel {
            id: errorLabel
            visible: errorMsg.length === 0 ? false : true
            //% "Error: *error text*"
            //: Error message on the auth page
            text: "Error: " + errorMsg
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
