import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC

import Paddock 1.0

QQC.MenuItem {
    id: root

    property alias shortcut: shortcut.sequence

    property color _textColor: enabled
                               ? Styling.colors.layers.foreground
                               : Styling.colors.layers.foregroundDisabled

    Shortcut {
        id: shortcut
        enabled: root.enabled
        onActivated: root.triggered()
    }

    contentItem: Item {
       Text {
           anchors.fill: parent
           leftPadding: root.indicator.width
           rightPadding: root.arrow.width

           text: root.text
           font: root.font
           color: root._textColor

           horizontalAlignment: Text.AlignLeft
           verticalAlignment: Text.AlignVCenter
        }

       Text {
           anchors.fill: parent
           leftPadding: root.indicator.width
           rightPadding: root.arrow.width

           text: visible ? shortcut.nativeText : ""
           font: root.font
           color: root._textColor

           visible: shortcut.nativeText.length > 0

           horizontalAlignment: Text.AlignRight
           verticalAlignment: Text.AlignVCenter
       }
   }
}
