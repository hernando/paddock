import QtQuick 2.15

import Paddock 1.0
import Paddock.Pads.KorgPadKontrol 1.0

Text {
    id: root

    property int type
    property int parameter

    opacity: enabled ? 1.0 : 0.3

    fontSizeMode: Text.VerticalFit
    font.pointSize: Styling.sizes.fonts.bigLabel
    horizontalAlignment: Text.AlignHLeft
    verticalAlignment: Text.AlignVCenter

    signal decrement()
    signal increment()

    text: {
        switch (root.type) {
        case KnobModel.PitchBend:
            return "Bend"
        case KnobModel.AfterTouch:
            return "AFT"
        case KnobModel.Controller:
            return "#" + root.parameter
        default:
            return "?"
        }
    }

    WheelHandler {
        cursorShape: Qt.CrossCursor
        onWheel: {
            if (event.angleDelta.y > 0) {
                root.increment()
            } else {
                root.decrement()
            }
        }
    }
}
