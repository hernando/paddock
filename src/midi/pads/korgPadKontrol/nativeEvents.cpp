#include "nativeEvents.hpp"
#include "sysex.hpp"

#include <iostream>

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{
std::optional<Event> decodeEvent(std::span<const std::byte> message)
{
    using namespace events;
    using namespace sysex;

    if (message[0] != KORG || message[1] != 0x40_b ||
        message[2] != SW_PROJECT || message[3] != PADKONTROL)
        return std::nullopt;

    auto payload = message.subspan(5);

    switch (message[4])
    {
    case PAD_OUTPUT:
    {
        PadOutput event;
        event.number = int(payload[0] & 0x0F_b);
        event.on = (payload[0] & 0x40_b) != 0x00_b;
        event.velocity = Value7bit(payload[1]);
        return event;
    }
    case PEDAL_OUTPUT:
    {
        PedalOutput event;
        event.data = Value7bit(payload[0]);
        return event;
    }
    case SW_OUTPUT:
    {
        SwitchOutput event;
        event.name = Switch(payload[0]);
        event.on = payload[1] == 0x7F_b;
        return event;
    }
    case KNOB_OUTPUT:
    {
        KnobOutput event;
        event.knob = payload[0] == 0x00_b ? Knob::knob1 : Knob::knob2;
        event.value = Value7bit(payload[1]);
        return event;
    }
    case ENCODER_OUTPUT:
    {
        WheelOutput event;
        event.turn = payload[1] == 0x01_b ? WheelTurn::clockwise
                                          : WheelTurn::counterClockwise;
        return event;
    }
    case XY_OUTPUT:
    {
        XyOutput event;
        event.x = Value7bit(payload[0]);
        event.y = Value7bit(payload[1]);
        return event;
    }
    }
    return std::nullopt;
}

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
