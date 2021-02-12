import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

import Paddock 1.0
import Paddock.Pads 1.0

ApplicationWindow {
    id: root

    minimumHeight: height
    minimumWidth: width
    maximumHeight: height
    maximumWidth: width

    property Session session: globalSession

    menuBar: MenuBar {
        session: root.session

        onQuitRequest: root.close()
    }

    ControllerLayout {
        id: layout

        device: session.controller

        onReloaded: {
            root.width = Math.max(400, layout.implicitWidth)
            root.height = Math.max(400, layout.implicitHeight) + menuBar.height
            root.minimumHeight = root.height
            root.minimumWidth = root.width
            root.maximumHeight = root.height
            root.maximumWidth = root.width
        }
    }

    background: Rectangle {
        color: Styling.colors.layers.background
    }

    Connections {
        target: session
        function onProgramChanged() {
            console.log("Program changed")
        }
    }
}
