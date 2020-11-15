import QtQuick 2.15
import QtQuick.Layouts 1.15

import Paddock 1.0
import Paddock.Controls 1.0

GridLayout {
    id: root
    columns: 4
    rows: 4
    columnSpacing: Styling.sizes.spacings.min
    rowSpacing: Styling.sizes.spacings.min

    Repeater {
        model: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
        delegate: Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Pad {
                anchors.fill: parent
            }
        }
    }
}
