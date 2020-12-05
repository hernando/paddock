import Paddock.Pads.KorgPadKontrol 1.0 as PadUi
import Paddock 1.0

PadUi.Controller {
    property KorgPadKontrol device: null

    enabled: device === undefined
}
