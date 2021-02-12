import Paddock.Pads.KorgPadKontrol 1.0 as PadUi
import Paddock 1.0

PadUi.Controller {
    property KorgPadKontrol device: null
    program: device.program

    enabled: device !== null && device.isConnected

    width: 760
    height: 1250
}
