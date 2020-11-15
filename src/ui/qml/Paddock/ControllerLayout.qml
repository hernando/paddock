import QtQuick 2.15

import Paddock 1.0
import Paddock.Pads 1.0

Item {
    id: root

    enum Model {
        KorgPadKontrol
    }

    property int model: -1

    Loader {
        anchors.fill: parent
        anchors.margins: Styling.sizes.spacings.min
        Component {
            id: korgPadKontrol
            KorgPadKontrol {
            }
        }

        sourceComponent: {
            switch (root.model) {
            case ControllerLayout.Model.KorgPadKontrol: return korgPadKontrol
            }
            return undefined
        }
    }
}
