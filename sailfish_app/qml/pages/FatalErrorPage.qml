import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: fatalErrorPage
    property alias text: errorLabel.text

    InfoLabel {
        id: errorLabel
        property Item _page

        readonly property bool _portrait: !_page || _page.isPortrait

        y: _page
           || !parent ? Math.round(
                            _portrait ? Screen.height / 4 : Screen.width
                                        / 4) : parent.height / 4
        anchors.horizontalCenter: parent && parent.horizontalCenter
                                  || undefined

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

    Component.onCompleted: {
    console.log("AAA")}
}
