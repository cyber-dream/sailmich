import QtQuick 2.0
import Sailfish.Silica 1.0
import Immich 1.0

Page {
    backNavigation: false

    property string address: ""
    property string email: ""
    property string password: ""
    property string error: ""

    //    Component.onCompleted: {
    //          if (pageStack.busy.)
    //      }
    onStatusChanged: {
        if (status === PageStatus.Active && (address.length > 0
                                             && email.length > 0
                                             && password.length > 0)) {
            startLogin()
        }
    }

    function startLogin() {
        const loginResult = Immich.auth.q_login(address, email, password)

        var callback = function () {
            loginResult.finished.disconnect(callback)
            onLoginResult(loginResult)
        }

        if (loginResult.isFinished()) {
            console.log("finished")
            onLoginResult(loginResult)
        } else {
            loginResult.finished.connect(callback)
        }

        address = ""
        email = ""
        password = ""
    }

    function onLoginResult(loginResult) {
        if (!loginResult.isSucceeded()) {
            error = loginResult.error().message
            pageStack.push(Qt.resolvedUrl("AuthDialog.qml"), {
                               "errorMsg": loginResult.error().message
                           })
            return
        }

        openMainPage()
    }

    PageBusyIndicator {
        running: true
    }
}
