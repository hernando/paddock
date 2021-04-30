import Paddock.Pads.KorgPadKontrol 1.0 as PadUi
import Paddock 1.0

PadUi.Controller {
    property KorgPadKontrol controller: null
    program: controller.program

    enabled: controller !== null && controller.isConnected

    width: 760
    height: 1250
}
