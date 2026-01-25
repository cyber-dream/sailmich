import QtQuick 2.0
import Sailfish.Silica 1.0
//import Immich 1.0
//import LogWriter 1.0

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height
        Column {
            id: col
            PageHeader {
                title: "Debug Page"
            }
            width: parent.width
            spacing: Theme.paddingSmall

            SectionHeader {
                text: "Logs"
            }

            DetailItem {
                label: "File"
                value: LogWriterObject.filePath
            }

            Button{text: "View logs"
                onClicked: {LogWriterObject.readLogFile()}}

            //            SectionHeader {
            //                text: "Standart Pathes"
            //            }

            //            DetailItem {
            //                label: "DesktopLocation"
            //                value: Immich.qStandartPath(Immich.DesktopLocation)
            //            }
            //            DetailItem {
            //                label: "DocumentsLocation"
            //                value: Immich.qStandartPath(Immich.DocumentsLocation)
            //            }
            //            DetailItem {
            //                label: "FontsLocation"
            //                value: Immich.qStandartPath(Immich.FontsLocation)
            //            }
            //            DetailItem {
            //                label: "ApplicationsLocation"
            //                value: Immich.qStandartPath(Immich.ApplicationsLocation)
            //            }
            //            DetailItem {
            //                label: "MusicLocation"
            //                value: Immich.qStandartPath(Immich.MusicLocation)
            //            }
            //            DetailItem {
            //                label: "MoviesLocation"
            //                value: Immich.qStandartPath(Immich.MoviesLocation)
            //            }
            //            DetailItem {
            //                label: "PicturesLocation"
            //                value: Immich.qStandartPath(Immich.PicturesLocation)
            //            }
            //            DetailItem {
            //                label: "TempLocation"
            //                value: Immich.qStandartPath(Immich.TempLocation)
            //            }
            //            DetailItem {
            //                label: "HomeLocation"
            //                value: Immich.qStandartPath(Immich.HomeLocation)
            //            }
            //            DetailItem {
            //                label: "DataLocation"
            //                value: Immich.qStandartPath(Immich.DataLocation)
            //            }
            //            DetailItem {
            //                label: "CacheLocation"
            //                value: Immich.qStandartPath(Immich.CacheLocation)
            //            }
            //            DetailItem {
            //                label: "GenericDataLocation"
            //                value: Immich.qStandartPath(Immich.GenericDataLocation)
            //            }
            //            DetailItem {
            //                label: "RuntimeLocation"
            //                value: Immich.qStandartPath(Immich.RuntimeLocation)
            //            }
            //            DetailItem {
            //                label: "ConfigLocation"
            //                value: Immich.qStandartPath(Immich.ConfigLocation)
            //            }
            //            DetailItem {
            //                label: "DownloadLocation"
            //                value: Immich.qStandartPath(Immich.DownloadLocation)
            //            }
            //            DetailItem {
            //                label: "GenericCacheLocation"
            //                value: Immich.qStandartPath(Immich.GenericCacheLocation)
            //            }
            //            DetailItem {
            //                label: "GenericConfigLocation"
            //                value: Immich.qStandartPath(Immich.GenericConfigLocation)
            //            }
            //            DetailItem {
            //                label: "AppDataLocation"
            //                value: Immich.qStandartPath(Immich.AppDataLocation)
            //            }
            //            DetailItem {
            //                label: "AppConfigLocation"
            //                value: Immich.qStandartPath(Immich.AppConfigLocation)
            //            }
            //            DetailItem {
            //                label: "AppLocalDataLocation"
            //                value: Immich.qStandartPath(Immich.AppLocalDataLocation)
            //            }
        }
    }
}
