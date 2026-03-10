import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    InfoLabel {
        id: errorLabel

        property Item _page
        readonly property bool _portrait: !_page || _page.isPortrait

        Component.onCompleted: _page = Util.findPage(root)

        y: _page
           || !parent ? Math.round(
                            _portrait ? Screen.height / 4 : Screen.width / 4) : parent.height / 4
        anchors.horizontalCenter: parent && parent.horizontalCenter || undefined

        visible: errorMsg.length === 0 ? false : true
        //% "Login has been disabled"
        //: "Error" prefix for prepend error message (Error: *error text*)
        text: qsTrId("authFlow.LoginDisabled")
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
