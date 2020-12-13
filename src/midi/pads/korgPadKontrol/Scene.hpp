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
            Curve1 = 0,
            Curve2,
            Curve3,
            Curve4,
            Curve5,
            Curve6,
            Curve7,
            Curve8,
        };
        // Curve or constant value in 1-127
        std::variant<VelocityCurve, Value7bit> velocity;
        Value7bit offVelocity;
    };

    struct Control
    {
        Value7bit param;
        Value7bit value;
        Value7bit releaseValue;
    };

    enum class SwitchType { Momentary, Toggle };

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
        unsigned char minVolume;
        unsigned char maxVolume;
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

Expected<std::vector<std::byte>> encodeScene(const Scene& scene);
Expected<Scene> decodeScene(std::span<const std::byte> payload);

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
