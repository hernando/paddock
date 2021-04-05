import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0
import Paddock.Pads.KorgPadKontrol 1.0

Item {
    id: root

    width: Styling.sizes.widths.pad

    implicitHeight: title.implicitHeight + parameters.implicitHeight +
                    Styling.sizes.spacings.x4

    property string name
    property int parameter: 1
    property int minSpeed: 0
    property int maxSpeed: 255
    property int minVolume: 1
    property int maxVolume: 127

    signal incrementParameter(int parameter)
    signal decrementParameter(int parameter)

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

        GridLayout {
            id: parameters

            anchors.fill: parent
            anchors.margins: Styling.sizes.spacings.x2
            columnSpacing: Styling.sizes.spacings.min
            rowSpacing: Styling.sizes.spacings.min

            columns: 2
            rows: 2

            Text {
                Layout.fillWidth: true
                text: "\u2190" + root.minSpeed
                opacity: enabled ? 1.0 : 0.3
                fontSizeMode: Text.Fit
                font.pointSize: Styling.sizes.fonts.bigLabel
                horizontalAlignment: Text.AlignLeft

                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementParameter(RepeaterModel.MinSpeed)
                        } else {
                            root.decrementParameter(RepeaterModel.MinSpeed)
                        }
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: "\u2192" + root.maxSpeed
                opacity: enabled ? 1.0 : 0.3
                fontSizeMode: Text.Fit
                font.pointSize: Styling.sizes.fonts.bigLabel
                horizontalAlignment: Text.AlignLeft

                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementParameter(RepeaterModel.MaxSpeed)
                        } else {
                            root.decrementParameter(RepeaterModel.MaxSpeed)
                        }
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: "\u2193" + root.minVolume
                opacity: enabled ? 1.0 : 0.3
                fontSizeMode: Text.Fit
                font.pointSize: Styling.sizes.fonts.bigLabel
                horizontalAlignment: Text.AlignLeft

                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementParameter(RepeaterModel.MinVolume)
                        } else {
                            root.decrementParameter(RepeaterModel.MinVolume)
                        }
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: "\u2191" + root.maxVolume
                opacity: enabled ? 1.0 : 0.3
                fontSizeMode: Text.Fit
                font.pointSize: Styling.sizes.fonts.bigLabel
                horizontalAlignment: Text.AlignLeft

                WheelHandler {
                    cursorShape: Qt.CrossCursor
                    onWheel: {
                        if (event.angleDelta.y > 0) {
                            root.incrementParameter(RepeaterModel.MaxVolume)
                        } else {
                            root.decrementParameter(RepeaterModel.MaxVolume)
                        }
                    }
                }
            }
        }
    }
}
