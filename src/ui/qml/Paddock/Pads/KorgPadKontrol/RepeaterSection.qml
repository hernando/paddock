import QtQuick 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0
import Paddock.Pads.KorgPadKontrol 1.0
import Paddock.Controls 1.0

ColumnLayout {
    id: root

    property Program program

    spacing: Styling.sizes.spacings.min

    RepeaterModel {
        id: repeaterModel
        program: root.program
    }

    RepeaterController {
        id: controller
        program: root.program
    }

    Repeater {
        model: repeaterModel
        delegate: TriggerRepeater {
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight

            name: model.name
            minSpeed: model.minSpeed
            maxSpeed: model.maxSpeed
            minVolume: model.minVolume
            maxVolume: model.maxVolume

            onIncrementParameter: controller.incrementParameter(index, parameter)
            onDecrementParameter: controller.decrementParameter(index, parameter)
        }
    }

    Item {
        Layout.fillHeight: true
    }
}
