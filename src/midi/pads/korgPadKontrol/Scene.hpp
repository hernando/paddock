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
    struct Note
    {
        Value7bit value;
        enum class VelocityCurve
        {
            A,
            B
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

    enum class ActionType { Momentary, Toggle };

    struct Trigger
    {
        bool enabled{true};
        int midiChannel{11}; // 1-16
        ActionType type{ActionType::Momentary};
        std::variant<Note, Control> action;
        bool hasRollFlam{true};
        enum class Port { A = 0, B = 1};
        Port port;
    };

    struct Knob
    {
        bool reversePolarity{false};
        bool enabled{true};
        // Missing fields
    };

    struct Axis : public Knob
    {
        unsigned char retPosition; // what's this?
    };

    struct Repeater
    {
        char minSpeed;
        char maxSpeed;
        char minVolume;
        char maxVolume;
    };

    std::array<Trigger, 16> pads;
    Trigger pedal;

    std::array<Knob, 2> knobs;

    Axis x;
    Axis y;

    Repeater flam;
    Repeater roll;
};

Expected<std::vector<std::byte>> encodeScene(const Scene& scene);
Expected<Scene> decodeScene(std::span<const std::byte> payload);

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
