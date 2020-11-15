import QtQuick 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0

GridLayout {
    id: root
    columns: 3
    rows: 4
    columnSpacing: Styling.sizes.spacings.min
    rowSpacing: Styling.sizes.spacings.min

    Rectangle {
        color: "transparent"
        border.width: 1
        id: topKnobs
        Layout.columnSpan: 3
        Layout.preferredHeight: root.height * 0.2
        Layout.fillWidth: true
    }

    Rectangle {
        color: "transparent"
        border.width: 1
        id: settingSection
        Layout.preferredHeight: root.height * 0.32
        Layout.preferredWidth: root.width * 0.3
    }

    Rectangle {
        color: "transparent"
        border.width: 1
        id: triggerPadSectionLeftButtons
        Layout.rowSpan: 2
        Layout.preferredHeight: root.height * 0.48
        Layout.preferredWidth: root.width * 0.1
        Layout.fillHeight: true
    }

    TriggerSection {
        id: triggerPadSection
        Layout.rowSpan: 3
        Layout.preferredHeight: root.height * 0.8
        Layout.preferredWidth: root.width * 0.6
        Layout.fillHeight: true
        Layout.fillWidth: true
    }

    Rectangle {
        color: "transparent"
        border.width: 1
        id: paremeterSection
        Layout.preferredHeight: root.height * 0.16
        Layout.preferredWidth: root.width * 0.3
    }

    Rectangle {
        color: "transparent"
        border.width: 1
        id: xySection
        Layout.columnSpan: 2
        Layout.preferredWidth: root.width * 0.4
        Layout.preferredHeight: root.height * 0.32
        Layout.fillWidth: true
    }
}
