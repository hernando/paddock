import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import Paddock 1.0

QQC2.Button {
    id: root

    padding: 0
    width: contentItem.implicitWidth

    contentItem: Text {
        text: root.text
        font: root.font
        opacity: root.enabled ? 1.0 : 0.3
        color: {
            if (root.pressed) {
                return Styling.colors.buttons.textPressed
            } else if (root.checked) {
                return root.hovered
                    ? Styling.hovered(Styling.colors.buttons.textChecked)
                    : Styling.colors.buttons.textChecked
            }
            return root.hovered
                ? Styling.hovered(Styling.colors.buttons.text)
                : Styling.colors.buttons.text
        }


        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Item {}
}

