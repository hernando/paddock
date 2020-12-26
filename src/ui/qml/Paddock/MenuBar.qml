import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC

import Paddock 1.0

QQC.MenuBar {
    id: root

    property Session session

    signal quitRequest

    QQC.Menu {
        title: qsTr("&File")
        MenuItem {
            text: qsTr("New...")
            shortcut: "Ctrl+N"
        }
        MenuItem {
            enabled: !session.isNsmSession
            text: qsTr("Open...")
            shortcut: "Ctrl+O"
        }
        MenuItem {
            enabled: session.isNsmSession
            text: qsTr("Import into...")
            shortcut: "Ctrl+I"
        }
        MenuItem {
            text: qsTr("Save")
            shortcut: "Ctrl+S"
        }
        MenuItem {
            enabled: !session.isNsmSession
            text: qsTr("Save As...")
            shortcut: "Ctrl+Shift+S"
        }
        QQC.MenuSeparator { }
        MenuItem { text: qsTr("Close") }
        MenuItem {
            text: qsTr("Quit")
            onTriggered: root.quitRequest()
        }
    }

    QQC.Menu {
        title: qsTr("&Controller")
        MenuItem {
            enabled: session.controller !== undefined &&
                     session.controller.isNative
            text: qsTr("Set normal mode")
            onTriggered: session.controller.setNormalMode()
            shortcut: "Ctrl+M"
        }
        MenuItem {
            enabled: session.controller !== undefined &&
                     !session.controller.isNative
            text: qsTr("Set native mode")
            onTriggered: session.controller.setNativeMode()
            shortcut: "Ctrl+M"
        }
        MenuItem {
            text: qsTr("Load device scene")
            enabled: session.controller !== undefined
            onTriggered: session.controller.loadDeviceSceneIntoProgram()
            shortcut: "Ctrl+R"
        }
        MenuItem {
            text: qsTr("Upload program")
            enabled: session.controller !== undefined
            onTriggered: session.controller.uploadProgramToDevice()
            shortcut: "Ctrl+U"
        }
    }

    QQC.Menu {
        title: qsTr("Help")
        QQC.Action { text: qsTr("About") }
    }
}
