import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC

import Paddock 1.0

QQC.MenuBar {
    id: root

    property Session session

    QQC.Menu {
        title: qsTr("&File")
        QQC.Action { text: qsTr("&New...") }
        QQC.Action {
            enabled: !session.isNsmSession
            text: qsTr("&Open...")
        }
        QQC.Action {
            enabled: session.isNsmSession
            text: qsTr("&Import into...")
        }
        QQC.Action { text: qsTr("&Save") }
        QQC.Action {
            enabled: !session.isNsmSession
            text: qsTr("Save &As...")
        }
        QQC.MenuSeparator { }
        QQC.Action { text: qsTr("&Close") }
        QQC.Action { text: qsTr("&Quit") }
    }

    QQC.Menu {
        title: qsTr("&Controller")
        QQC.Action {
            enabled: session.controller !== undefined && session.controller.isNative
            text: qsTr("&Set normal mode")
            onTriggered: session.controller.setNormalMode()
        }
        QQC.Action {
            enabled: session.controller !== undefined && !session.controller.isNative
            text: qsTr("&Set native mode")
            onTriggered: session.controller.setNativeMode()
        }
    }

    QQC.Menu {
        title: qsTr("&Help")
        QQC.Action { text: qsTr("&About") }
    }
}
