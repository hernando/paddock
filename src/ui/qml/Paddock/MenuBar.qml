import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC

import Paddock 1.0

QQC.MenuBar {
    id: root

    property Session session

    QQC.Menu {
        title: qsTr("&File")
        QQC.Action {
            text: qsTr("&New...")
            shortcut: "Ctrl+N"
        }
        QQC.Action {
            enabled: !session.isNsmSession
            text: qsTr("&Open...")
            shortcut: "Ctrl+O"
        }
        QQC.Action {
            enabled: session.isNsmSession
            text: qsTr("&Import into...")
            shortcut: "Ctrl+I"
        }
        QQC.Action {
            text: qsTr("&Save")
            shortcut: "Ctrl+S"
        }
        QQC.Action {
            enabled: !session.isNsmSession
            text: qsTr("Save &As...")
            shortcut: "Ctrl+Shift+S"
        }
        QQC.MenuSeparator { }
        QQC.Action { text: qsTr("&Close") }
        QQC.Action { text: qsTr("&Quit") }
    }

    QQC.Menu {
        title: qsTr("&Controller")
        MenuItem {
            enabled: session.controller !== undefined &&
                     session.controller.isNative
            text: qsTr("Set &normal mode")
            onTriggered: session.controller.setNormalMode()
            shortcut: "Ctrl+M"
        }
        MenuItem {
            enabled: session.controller !== undefined &&
                     !session.controller.isNative
            text: qsTr("Set &native mode")
            onTriggered: session.controller.setNativeMode()
            shortcut: "Ctrl+M"
        }
        MenuItem {
            text: qsTr("&Dump scene")
            enabled: session.controller !== undefined
            onTriggered: session.controller.dumpCurrentScene()
            shortcut: "Ctrl+D"
        }
    }

    QQC.Menu {
        title: qsTr("&Help")
        QQC.Action { text: qsTr("&About") }
    }
}
