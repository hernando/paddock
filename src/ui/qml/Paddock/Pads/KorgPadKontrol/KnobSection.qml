import QtQuick 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0
import Paddock.Pads.KorgPadKontrol 1.0
import Paddock.Controls 1.0

ColumnLayout {
    id: root

    property Program program

    spacing: Styling.sizes.spacings.min

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
        delegate: Knob {
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight

            on: model.enabled
            type: model.type
            parameter: model.parameter
            reversed: model.reversePolarity

            onToggleEnabled: controller.toggleEnabled(index)
            onTogglePolarity: controller.togglePolarity(index)
            onIncrementParameter: controller.incrementParameter(index)
            onDecrementParameter: controller.decrementParameter(index)
        }
    }

    Item {
        Layout.fillHeight: true
    }
}
