#pragma once

#include "utils/mp.hpp"

#include <variant>

namespace paddock
{
namespace midi
{
struct NoteOnEvent
{
    char note;
    char velocity;
};

struct NoteOffEvent
{
    char note;
    char velocity;
};

using VoiceEventTypes = mp::Types<NoteOnEvent, NoteOffEvent>;
using VoiceEvent = mp::apply<std::variant, VoiceEventTypes>;

using ControlEventTypes = mp::Types<>;

using EventTypes = mp::join<VoiceEventTypes, ControlEventTypes>;

using Event = mp::apply<std::variant, EventTypes>;

} // namespace midi
} // namespace paddock
