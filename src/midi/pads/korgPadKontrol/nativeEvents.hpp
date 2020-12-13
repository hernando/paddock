#pragma once

#include "enums.hpp"
#include "midi/types.hpp"

#include "utils/mp.hpp"

#include <optional>
#include <span>
#include <variant>
#include <vector>

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{
namespace events
{
struct LedOnCommand
{
    LedName led;
};

struct LedOffCommand
{
    LedName led;
};

struct BlinkLedCommand
{
    LedName led;
};

struct TriggerLedCommand
{
    LedName led;
    unsigned int milliseconds{90};
};

struct SetAllLedCommand
{
    std::vector<LedName> on;
    char text[3];
};

struct PadOutput
{
    int number;
    Value7bit velocity;
    bool on;
};

struct PedalOutput
{
    Value7bit data;
};

struct WheelOutput
{
    WheelTurn turn;
};

struct SwitchOutput
{
    Switch name;
    bool on;
};

struct KnobOutput
{
    Knob knob;
    Value7bit value;
};

struct XyOutput
{
    Value7bit x;
    Value7bit y;
};

using LedTypes =
    mp::Types<LedOnCommand, LedOffCommand, BlinkLedCommand, TriggerLedCommand>;
using Led = mp::apply<std::variant, LedTypes>;

using OutputTypes = mp::Types<PadOutput, PedalOutput, SwitchOutput, KnobOutput,
                              WheelOutput, XyOutput>;
using Output = mp::apply<std::variant, OutputTypes>;

} // namespace events

using Event = mp::apply<std::variant, events::OutputTypes>;
using Command = mp::apply<std::variant, events::LedTypes>;

std::optional<Event> decodeEvent(std::span<const std::byte> payload);

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
