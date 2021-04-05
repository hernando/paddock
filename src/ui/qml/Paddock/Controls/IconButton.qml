import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import Paddock 1.0

QQC2.Button {
    id: root

    width: Styling.sizes.widths.iconButton
    height: width
    property bool ticked: false

    padding: 0

    opacity: root.enabled ? 1.0 : 0.3
    icon.color: {
        if (root.pressed) {
            return Styling.colors.buttons.iconPressed
        } else if (root.ticked) {
            return root.hovered
                ? Styling.hovered(Styling.colors.buttons.iconChecked)
                : Styling.colors.buttons.iconChecked
        }
        return root.hovered
            ? Styling.hovered(Styling.colors.buttons.icon)
            : Styling.colors.buttons.icon
    }


    background: Item {}
}

