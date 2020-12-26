import QtQuick 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0
import Paddock.Pads.KorgPadKontrol 1.0
import Paddock.Controls 1.0

GridLayout {
    id: root
    columns: 4
    rows: 4
    columnSpacing: Styling.sizes.spacings.min
    rowSpacing: Styling.sizes.spacings.min
    property Program program

    TriggerModel {
        id: triggerModel
        program: root.program
    }

    TriggerController {
        id: controller
        program: root.program
    }

    Repeater {
        model: triggerModel
        delegate: Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Pad {
                on: model.enabled
                midiChannel: model.midiChannel
                port: model.port == TriggerModel.PortA ? "A" : "B"
                isMomentary: model.switchType == TriggerModel.MomentarySwitch
                hasFlamRoll: model.hasFlamRoll

                isNote: model.actionType === TriggerModel.NoteAction
                noteName: model.actionType === TriggerModel.NoteAction
                          ? model.note : ""
                velocity: model.actionType === TriggerModel.NoteAction
                          ? model.velocity : 1
                anchors.fill: parent

                parameter: model.actionType === TriggerModel.ControlAction
                           ? model.parameter : 0
                value: model.actionType === TriggerModel.ControlAction
                       ? model.value : 0
                releaseValue: model.actionType === TriggerModel.ControlAction
                              ? model.releaseValue : 0

                onToggleEnabled: controller.toggleEnabled(index)
                onTogglePort: controller.togglePort(index)
                onToggleSwitchType: controller.toggleSwitchType(index)
                onToggleFlamRoll: controller.toggleFlamRoll(index)
                onIncrementMidiChannel: controller.incrementMidiChannel(index)
                onDecrementMidiChannel: controller.decrementMidiChannel(index)
                onIncrementActionValue: controller.incrementActionValue(index)
                onDecrementActionValue: controller.decrementActionValue(index)
                onIncrementVelocity: controller.incrementVelocity(index)
                onDecrementVelocity: controller.decrementVelocity(index)
                onIncrementValue: controller.incrementValue(index)
                onDecrementValue: controller.decrementValue(index)
                onIncrementReleaseValue:
                    controller.incrementReleaseValue(index)
                onDecrementReleaseValue:
                    controller.decrementReleaseValue(index)
            }
        }
    }
}
