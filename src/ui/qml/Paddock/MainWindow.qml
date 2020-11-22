import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

import Paddock 1.0
import Paddock.Pads 1.0

ApplicationWindow {
    id: root

    width: 1250
    height: 950

    property Session session: globalSession

    menuBar: MenuBar {
        session: root.session
    }

    ControllerLayout {
        model: ControllerLayout.Model.KorgPadKontrol
        anchors.fill: parent
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
