import QtQuick 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0
import Paddock.Pads.KorgPadKontrol 1.0
import Paddock.Controls 1.0

ColumnLayout {
    id: root

    property Program program

    Rectangle {
        id: header
        color: "#000000"
        Layout.preferredHeight: (headerText.implicitHeight +
                                 Styling.sizes.spacings.min * 2)
        Layout.fillWidth: true
        Layout.topMargin: Styling.sizes.spacings.min
        Layout.leftMargin: Styling.sizes.spacings.min
        Layout.rightMargin: Styling.sizes.spacings.min

        Text {
            id: headerText
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: Styling.sizes.spacings.min
            anchors.bottomMargin: Styling.sizes.spacings.min
            anchors.leftMargin: Styling.sizes.spacings.min
            text: "KORG padKontrol"
            font.pointSize: Styling.sizes.fonts.title
            color: "#FFFFFF"
        }
    }

    Item {
        id: topSection
        Layout.preferredHeight: 120
        Layout.fillWidth: true
    }

    Text {
        Layout.topMargin: Styling.sizes.spacings.x2
        Layout.leftMargin: Styling.sizes.spacings.x2
        text: "Pads"
        font.pointSize: Styling.sizes.fonts.title
    }

    TriggerSection {
        id: triggerPadSection
        Layout.rowSpan: 3
        Layout.fillWidth: true
        Layout.preferredHeight: width
        Layout.maximumHeight: width
        Layout.rightMargin: Styling.sizes.spacings.x2
        Layout.leftMargin: Styling.sizes.spacings.x2

        program: root.program
    }

    property int _onePadWidth: (triggerPadSection.width -
                                Styling.sizes.spacings.x2 * 3) * 0.25

    RowLayout {
        width: parent.width

        Layout.topMargin: Styling.sizes.spacings.x2
        Layout.fillWidth: true

        spacing: Styling.sizes.spacings.x2

        Text {
            Layout.leftMargin: Styling.sizes.spacings.x2
            text: "Pedal"
            font.pointSize: Styling.sizes.fonts.title
            Layout.preferredWidth: _onePadWidth
        }
        Text {
            text: "Knobs"
            font.pointSize: Styling.sizes.fonts.title
            Layout.preferredWidth: _onePadWidth
        }
        Text {
            text: "X-Y Pad"
            font.pointSize: Styling.sizes.fonts.title
            Layout.preferredWidth: _onePadWidth
        }
        Text {
            text: "Repeaters"
            font.pointSize: Styling.sizes.fonts.title
            Layout.preferredWidth: _onePadWidth
        }
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.rightMargin: Styling.sizes.spacings.x2
        Layout.leftMargin: Styling.sizes.spacings.x2
        Layout.bottomMargin: Styling.sizes.spacings.x2

        spacing: Styling.sizes.spacings.x2

        Pad {
            Layout.alignment: Qt.AlignTop
            implicitWidth: _onePadWidth
            height: _onePadWidth
        }

        KnobSection {
            Layout.maximumWidth: _onePadWidth
            height: _onePadWidth
            program: root.program
        }

        XySection {
            Layout.maximumWidth: _onePadWidth
            height: _onePadWidth
            program: root.program
        }

        RepeaterSection {
            Layout.maximumWidth: _onePadWidth
            height: _onePadWidth
            program: root.program
        }
    }

    Item {
        Layout.fillHeight: true
    }
}
