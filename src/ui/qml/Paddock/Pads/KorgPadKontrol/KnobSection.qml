import QtQuick 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0
import Paddock.Pads.KorgPadKontrol 1.0
import Paddock.Controls 1.0

RowLayout {
    id: root

    spacing: Styling.sizes.spacings.min
    property Program program

    KnobModel {
        id: knobModel
        program: root.program
    }

    KnobController {
        id: controller
        program: root.program
    }

    Repeater {
        model: knobModel
        delegate: Item {
            Layout.preferredWidth: 80
            Layout.fillHeight: true

            //Knob {
            //}
        }
    }
}
