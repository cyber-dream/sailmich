import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich.Init 1.0

Page {
    Component.onCompleted: {
        if (Init.initStatus === Init.StatusNotStarted)
            Init.initStatusChanged.connect(function () {
                workWithInitResult(Init.initStatus)
            })
        else
            workWithInitResult(Init.initStatus)
    }

    function workWithInitResult(status) {
        switch (status) {
        case Init.StatusEndpointUrlNotFound:
            console.log("Immich init status:", "UrlNotFound")
            openAuthDialog()
            break
        case Init.StatusAuthTokenNotFound:
            console.log("Immich init status:", "AuthTokenNotFound")
            openAuthDialog()
            break
        case Init.StatusApiPingFailed:
            console.log("Immich init status:", "ApiPingFailed")
            openOfflinePage()
            break
        case Init.StatusTokenInvalid:
            console.log("Immich init status:", "TokenInvalid")
            openAuthDialog()
            break
        case Init.StatusComplete:
            console.log("Immich init status:", "Complete")
            openMainPage()
            break
        default:
            console.error("unknown Immich init status:", status)
            pageStack.replaceAbove(null, fatalErrorPage)

            fatalErrorPage.text = "unknown Immich init status: " + status // TODO translate
            // TODO
        }
    }
}
