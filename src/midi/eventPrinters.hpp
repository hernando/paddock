#include "events.hpp"
#include "noteNames.hpp"

#include <iostream>

namespace paddock
{
namespace midi
{
std::ostream& operator<<(std::ostream& out, const events::NoteOn& event)
{
    out << "Note on " << noteNames[event.note] << " " << int(event.velocity)
        << " channel: " << int(event.channel);
    return out;
}

std::ostream& operator<<(std::ostream& out, const events::NoteOff& event)
{
    out << "Note off " << noteNames[event.note] << " " << int(event.velocity)
        << " channel: " << int(event.channel);
    return out;
}

std::ostream& operator<<(std::ostream& out, const events::KeyPressure& event)
{
    out << "Key pressure " << int(event.velocity)
        << " channel: " << int(event.channel);
    return out;
}

std::ostream& operator<<(std::ostream& out, const events::PitchBend& event)
{
    out << "Pitch bend " << int(event.value)
        << " channel: " << int(event.channel);
    return out;
}

std::ostream& operator<<(std::ostream& out,
                         const events::ChannelPressure& event)
{
    out << event.description << " " << int(event.pressure)
        << " channel: " << int(event.channel);
    return out;
}

std::ostream& operator<<(std::ostream& out, const events::Controller& event)
{
    out << "CC#" << int(event.parameter) << " " << int(event.value)
        << " channel: " << int(event.channel);
    return out;
}

std::ostream& operator<<(std::ostream& out, const events::Event& event)
{
    std::visit([&out](const auto& event) { out << event.description; }, event);
    return out;
}

} // namespace midi
} // namespace paddock
