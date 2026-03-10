import QtQuick 2.0

Item {
    property real globalLoadingAnimOpacity
    SequentialAnimation on globalLoadingAnimOpacity {
        loops: Animation.Infinite
        running: true

        NumberAnimation {
            from: 0.1
            to: 0.3
            duration: 800
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            from: 0.3
            to: 0.1
            duration: 800
            easing.type: Easing.InOutQuad
        }
    }
}
