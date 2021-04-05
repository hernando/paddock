import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0

Item {
    id: root

    width: Styling.sizes.widths.pad

    implicitHeight: title.implicitHeight + parameters.implicitHeight +
                    Styling.sizes.spacings.x4

    property string name
    property bool on: true
    property int type: 0
    property int parameter: 1
    property bool reversed: false
    property int releaseValue: 127

    signal toggleEnabled()
    signal togglePolarity()
    signal incrementParameter()
    signal decrementParameter()
    signal incrementReleaseValue()
    signal decrementReleaseValue()

    Text {
        id: title
        text: name
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        font.pointSize: Styling.sizes.fonts.subtitle
    }

    Rectangle {
        anchors.top: title.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

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
                enabled: root.on
                ticked: root.reversed
                onClicked: root.togglePolarity()
            }

            Item {
                Layout.fillHeight: true
            }
        }

        ColumnLayout {
            id: parameters

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: Styling.sizes.spacings.x3
            width: parent.width * 3.2 / 5.0

            enabled: root.on
            spacing: Styling.sizes.spacings.min

            KnobAction {
                id: parameter

                type: root.type
                parameter: root.parameter
                Layout.fillWidth: true

                onIncrement: root.incrementParameter()
                onDecrement: root.decrementParameter()
            }

            Text {
                Layout.fillWidth: true
                text: "\u2191" + root.releaseValue
                enabled: root.on
                opacity: enabled ? 1.0 : 0.3
                fontSizeMode: Text.Fit
                font.pointSize: Styling.sizes.fonts.bigLabel
                horizontalAlignment: Text.AlignLeft

                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementReleaseValue()
                        } else {
                            root.decrementReleaseValue()
                        }
                    }
                }
            }
        }
    }
}
