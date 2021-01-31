#pragma once

#include "nativeEvents.hpp"

#include <iostream>

namespace paddock::midi::korgPadKontrol::events
{
std::ostream& operator<<(std::ostream& out, const PadOutput& event)
{
    out << "Pad " << (int(event.number) + 1) << " " << int(event.velocity)
        << (event.on ? " on" : " off");
    return out;
}

std::ostream& operator<<(std::ostream& out, const PedalOutput& event)
{
    out << "Pedal " << int(event.data);
    return out;
}

std::ostream& operator<<(std::ostream& out, const WheelOutput& event)
{
    out << "Wheel " << (event.turn == WheelTurn::clockwise ? "ckw" : "cckw");
    return out;
}

std::ostream& operator<<(std::ostream& out, const SwitchOutput& event)
{
    out << "Switch " << int(event.name)
        << (event.pressed ? " pressed" : " released");
    return out;
}

std::ostream& operator<<(std::ostream& out, const KnobOutput& event)
{
    out << "Knob " << (int(event.knob) + 1) << " " << int(event.value);
    return out;
}

std::ostream& operator<<(std::ostream& out, const XyOutput& event)
{
    out << "XY " << int(event.x) << " " << int(event.y);
    return out;
}

std::ostream& operator<<(std::ostream& out, const Event& event)
{
    std::visit([&out](auto&& event) { out << event; }, event);
    return out;
}
} // namespace paddock::midi::korgPadKontrol::events
