import QtQuick 2.0
import Sailfish.Silica 1.0

InfoLabel {
    property string text
    id: errorLabel
    visible: errorMsg.length === 0 ? false : true
    //% "Error"
    //: "Error" prefix for prepend error message (Error: *error text*)
    text: qsTrId("common.Error") + ": " + text
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
