import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0

Item {
    id: root

    width: Styling.sizes.widths.pad
    height: width

    property bool on: true
    property int knobType: 0
    property int param: 1
    property bool reversed: false

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
                text: checked ? "ON" : "OFF"
                checked: root.on
                onClicked: root.toggleEnabled()
            }
            Text {
                enabled: root.on
                Layout.fillWidth: true
                opacity: enabled ? 1.0 : 0.3
                horizontalAlignment: Text.AlignRight
                text: "Ch " +  midiChannel
                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementMidiChannel()
                        } else {
                            root.decrementMidiChannel()
                        }
                    }
                }
            }
            TextButton {
                enabled: root.on
                Layout.alignment: Qt.AlignRight
                text: "Port " + port
                onClicked: root.togglePort()
            }
            RowLayout {
                spacing: 0
                Layout.alignment: Qt.AlignRight
                IconButton {
                    enabled: root.on
                    icon.name: root.isMomentary ? "momentary" : "toggle"
                    onClicked: root.toggleSwitchType()
                }
                IconButton {
                    enabled: root.on
                    checked: root.hasFlamRoll
                    icon.name: "flam_roll"
                    onClicked: root.toggleFlamRoll()
                }
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: Styling.sizes.spacings.x2
                TextButton {
                    text: "X"
                    checked: root.xAxisAssigned
                    onClicked: root.toggleKnobAssignment(2)
                }
                TextButton {
                    text: "Y"
                    checked: root.yAxisAssigned
                    onClicked: root.toggleKnobAssignment(3)
                }
            }
            TextButton {
                Layout.alignment: Qt.AlignRight
                text: "Knob 1"
                checked: root.knob1Assigned
                onClicked: root.toggleKnobAssignment(0)
            }
            TextButton {
                Layout.alignment: Qt.AlignRight
                text: "Knob 2"
                checked: root.knob2Assigned
                onClicked: root.toggleKnobAssignment(1)
            }
            Item {
                Layout.fillHeight: true
            }
        }

        Item {
            id: notePad
            visible: root.isNote
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: Styling.sizes.spacings.x3
            width: parent.width * 3.2 / 5.0

            enabled: root.on
            opacity: enabled ? 1.0 : 0.3

            Text {
                id: noteName
                width: parent.width
                height: parent.height * 2.5 / 5.0
                anchors.top: parent.top
                anchors.left: parent.left

                text: "\u266A" + root.noteName.replace("#", "\u266F")
                fontSizeMode: Text.VerticalFit
                font.pointSize: 24
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementActionValue()
                        } else {
                            root.decrementActionValue()
                        }
                    }
                }
            }
            Item
            {
                id: velocity
                width: parent.width
                height: parent.height * 1.8 / 5.0
                anchors.top: noteName.bottom
                anchors.left: parent.left

                Text {
                    visible: root.velocity >= 0
                    anchors.fill: parent
                    text: root.velocity
                    fontSizeMode: Text.Fit
                    font.pointSize: 24
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                Image {
                    visible: root.velocity < 1
                    anchors.fill: parent
                    anchors.horizontalCenter: parent.horizontalCenter
                    sourceSize: Qt.size(velocity.width * 3, velocity.height * 3)
                    fillMode: Image.PreserveAspectFit
                    source: visible ? "qrc:/paddock/img/curve-%1.svg".arg(
                                         -root.velocity)
                                    : ""
                }

                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementVelocity()
                        } else {
                            root.decrementVelocity()
                        }
                    }
                }
            }
        }

        ColumnLayout {
            id: controlPad
            visible: !root.isNote
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: Styling.sizes.spacings.x3
            width: parent.width * 3.2 / 5.0

            enabled: root.on
            opacity: enabled ? 1.0 : 0.3
            spacing: Styling.sizes.spacings.min

            Item {
                Layout.fillHeight: true
            }

            Text {
                Layout.fillWidth: true
                text: "#" + root.parameter
                fontSizeMode: Text.Fit
                font.pointSize: 24
                horizontalAlignment: Text.AlignLeft

                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementActionValue()
                        } else {
                            root.decrementActionValue()
                        }
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: "\u2193" + root.value
                fontSizeMode: Text.Fit
                font.pointSize: 24
                horizontalAlignment: Text.AlignLeft

                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementValue()
                        } else {
                            root.decrementValue()
                        }
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: "\u2191" + root.releaseValue
                fontSizeMode: Text.Fit
                font.pointSize: 24
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

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
