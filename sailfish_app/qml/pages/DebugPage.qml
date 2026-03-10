import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Secrets 1.0


Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height

        Column {
            id: col

            width: parent.width
            spacing: Theme.paddingSmall
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: Theme.horizontalPageMargin
                rightMargin: Theme.horizontalPageMargin
            }

            PageHeader {
                title: "Debug Page"
            }

            SectionHeader {
                text: "Secrets"
            }

            Button {
                text: "Clear"
                onClicked: {
                    Secrets.q_deleteAuthToken()
                }
            }

            SectionHeader {
                text: "Logger"
            }

            DetailItem {
                label: "Path"
                value: Logger.filePath
            }

            SectionHeader {
                text: "Qt varaibles"
            }

            DetailItem {
                label: "Applacation Name"
                value: Qt.application.name
            }
        }
    }
}
