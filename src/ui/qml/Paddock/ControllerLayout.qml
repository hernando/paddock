import QtQuick 2.15

import Paddock 1.0
import Paddock.Pads 1.0

Item {
    id: root

    property var device: undefined

    Loader {
        anchors.fill: parent
        anchors.margins: Styling.sizes.spacings.min
        Component {
            id: korgPadKontrol
            KorgPadKontrol {
                device: root.device
            }
        }
        Component {
            id: noDevice
            Item {
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
