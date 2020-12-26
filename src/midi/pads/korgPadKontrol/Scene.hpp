#pragma once

#include "midi/types.hpp"

#include "utils/Expected.hpp"
#include "utils/byte.hpp"

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

struct Scene
{
    enum class Port { A = 0, B = 1};

    struct Note
    {
        Value7bit note;
        enum class VelocityCurve
        {
            curve1 = 0,
            curve2,
            curve3,
            curve4,
            curve5,
            curve6,
            curve7,
            curve8,
        };
        // Curve or constant value in 1-127
        std::variant<VelocityCurve, Value7bit> velocity;
    };

    struct Control
    {
        Value7bit param;
        Value7bit value;
        Value7bit releaseValue;
    };

    enum class SwitchType { Momentary = 0, Toggle = 1};

    struct Trigger
    {
        bool enabled{true};
        int midiChannel{10}; // 1-16, 0 if !enabled
        SwitchType type{SwitchType::Momentary};
        std::variant<Note, Control> action;
        bool hasFlamRoll{true};
        Port port{Port::A};
    };

    enum class KnobType { PitchBend = 1, AfterTouch = 2, Controller = 3};

    struct Knob
    {
        bool enabled{true};
        KnobType type{KnobType::Controller};
        Value7bit param; // if type == Controller
        bool reversePolarity{false};
        // The event will be emitted in each of the channels of the assigned
        // pads/pedal.
        unsigned short padAssignmentBits{0xFFFF};
        bool pedalAssigned{true};
    };

    struct Axis : public Knob
    {
        char releaseValue; // -128..127 if type == KnobType::PitchBend,
                           // otherwise a value in 0..127
    };

    struct Repeater
    {
        unsigned char minSpeed; // min 40 if roll
        unsigned char maxSpeed; // max 240 if roll
        Value7bit minVolume;
        Value7bit maxVolume;
    };

    std::array<Trigger, 16> pads;
    Trigger pedal;

    std::array<Knob, 2> knobs;

    Axis x;
    Axis y;

    Repeater flam;
    Repeater roll;

    Value7bit fixedVelocity;
};

Expected<std::array<std::byte, 138>> encodeScene(const Scene& scene);
Expected<Scene> decodeScene(std::span<const std::byte> payload);

bool operator==(const Scene& lhs, const Scene& rhs);
bool operator==(const Scene::Trigger& lhs, const Scene::Trigger& rhs);
bool operator==(const Scene::Knob& lhs, const Scene::Knob& rhs);
bool operator==(const Scene::Axis& lhs, const Scene::Axis& rhs);
bool operator==(const Scene::Repeater& lhs, const Scene::Repeater& rhs);

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
