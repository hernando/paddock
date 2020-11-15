import QtQuick 2.15
import QtQuick.Controls 2.15

import Paddock 1.0

MouseArea {
    id: root

    width: Styling.sizes.widths.pad
    height: width

    hoverEnabled: true

    property color offColor: Styling.colors.layers.control
    property color onColor: Styling.colors.state.blinking

    Rectangle {
        anchors.fill: parent

        property color baseColor: offColor

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
    }
}
