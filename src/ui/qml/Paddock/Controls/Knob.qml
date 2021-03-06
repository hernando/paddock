import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0
import Paddock.Pads.KorgPadKontrol 1.0

Item {
    id: root

    width: Styling.sizes.widths.pad

    implicitHeight: parameter.implicitHeight + Styling.sizes.spacings.x4

    property bool on: true
    property int type: 0
    property int parameter: 1
    property bool reversed: false

    signal toggleEnabled()
    signal togglePolarity()
    signal incrementParameter()
    signal decrementParameter()

    Rectangle {
        anchors.fill: parent

        property color baseColor: Styling.colors.layers.control

        color: {
            if (root.containsMouse) {
                return Styling.hovered(baseColor)
            } else {
                return baseColor
            }
        }

        border.width: Styling.sizes.twoPoints
        border.color: Styling.colors.layers.border
        radius: Styling.sizes.radii.pad

        ColumnLayout {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.rightMargin: Styling.sizes.spacings.x2
            anchors.topMargin: Styling.sizes.spacings.x2
            width: parent.width * 1.8 / 5.0
            spacing: Styling.sizes.spacings.min

            TextButton {
                Layout.alignment: Qt.AlignRight
                text: ticked ? "ON" : "OFF"
                ticked: root.on
                onClicked: root.toggleEnabled()
            }

            TextButton {
                Layout.alignment: Qt.AlignRight
                text: ticked ? "\u2796" : "\u2795"
                ticked: root.reversed
                enabled: root.on
                onClicked: root.togglePolarity()
            }

            Item {
                Layout.fillHeight: true
            }
        }

        KnobAction {
            id: parameter

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: Styling.sizes.spacings.x3
            anchors.left: parent.left

            width: parent.width * 3.2 / 5.0
            enabled: root.on

            type: root.type
            parameter: root.parameter

            onIncrement: root.incrementParameter()
            onDecrement: root.decrementParameter()
        }
    }
}
