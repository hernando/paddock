import QtQuick 2.15

import Paddock 1.0
import Paddock.Pads 1.0

Item {
    id: root

    property var device: undefined
    implicitWidth: loader.width
    implicitHeight: loader.height

    signal reloaded

    Loader {
        id: loader
        anchors.centerIn: parent

        onLoaded: root.reloaded()

        Component {
            id: korgPadKontrol

            KorgPadKontrol {
                device: root.device
            }
        }

        Component {
            id: noDevice

            Item {
                width: 400
                height: 400

                Text {
                    anchors.centerIn: parent
                    text: "No supported device connected"
                }
            }
        }

        sourceComponent: {
            if (root.device === undefined) {
                return noDevice
            }
            switch (device.model) {
            case ControllerModel.KorgPadKontrol: return korgPadKontrol
            }
            return noDevice
        }
    }
}
