import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

import Paddock 1.0
import Paddock.Pads 1.0

ApplicationWindow {
    id: root

    width: 1250
    height: 950

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action { text: qsTr("&New...") }
            Action { text: qsTr("&Open...") }
            Action { text: qsTr("&Save") }
            Action { text: qsTr("Save &As...") }
            MenuSeparator { }
            Action { text: qsTr("&Close") }
            Action { text: qsTr("&Quit") }
        }
        Menu {
            title: qsTr("&Help")
            Action { text: qsTr("&About") }
        }
    }

    ControllerLayout {
        model: ControllerLayout.Model.KorgPadKontrol
        anchors.fill: parent
    }

    background: Rectangle {
        color: Styling.colors.layers.background
    }
}
