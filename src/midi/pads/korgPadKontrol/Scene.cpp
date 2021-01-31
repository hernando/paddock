#include "Scene.hpp"

#include "scenePrinters.hpp"

#include "midi/types.hpp"

#include "utils/encodings.hpp"

namespace paddock::midi::korgPadKontrol
{
namespace
{
constexpr int channelBytes[] = {1,  2,  3,  4,  5,  6,  7,  9, //
                                10, 11, 12, 13, 14, 15, 17, 18, 93};
constexpr int enabledBit = 4;

constexpr int swTypeBytes[] = {1,  2,  3,  4,  5,  6,  7,  9, //
                               10, 11, 12, 13, 14, 15, 17, 18, 93};
constexpr int swTypeBit = 5;

constexpr int portBytes[] = {131, 131, 131, 131, 131, 131, 131, 128, //
                             132, 132, 132, 132, 132, 132, 132, 128, 133};
constexpr int portBits[] = {0, 1, 2, 3, 4, 5, 6, 2, //
                            0, 1, 2, 3, 4, 5, 6, 3, 1};

constexpr int actionTypeBytes[] = {16, 16, 16, 16, 16, 24, 24, 24, //
                                   24, 24, 24, 24, 32, 32, 32, 32, 88};
constexpr int actionTypeBits[] = {2, 3, 4, 5, 6, 0, 1, 2, //
                                  3, 4, 5, 6, 0, 1, 2, 3, 5};
constexpr int idBytes[] = {19, 20, 21, 22, 23, 25, 26, 27, //
                           28, 29, 30, 31, 33, 34, 35, 36, 94};
constexpr int releaseValueBytes[] = {55, 57, 58, 59, 60, 61, 62, 63, //
                                     65, 66, 67, 68, 69, 70, 71, 73, 97};

constexpr int valueBytes[] = {74, 75, 76, 77, 78, 79, 81, 82, //
                              83, 84, 85, 86, 87, 89, 90, 91, 98};
constexpr int velocityBytes[] = {37, 38, 39, 41, 42, 43, 44, 45, //
                                 46, 47, 49, 50, 51, 52, 53, 54, 95};

constexpr int velocityTypeBytes[] = {32, 32, 32, 40, 40, 40, 40, 40, //
                                     40, 40, 48, 48, 48, 48, 48, 48};
constexpr int velocityTypeBits[] = {4, 5, 6, 0, 1, 2, 3, 4, //
                                    5, 6, 0, 1, 2, 3, 4, 5};

constexpr int hasFlamRollBytes[] =          //
    {99,  99,  99,  99,  99,  99,  99,  96, //
     100, 100, 100, 100, 100, 100, 100, 96, 93};
constexpr int hasFlamRollBits[] = {0, 1, 2, 3, 4, 5, 6, 2, //
                                   0, 1, 2, 3, 4, 5, 6, 3, 6};

Scene::Trigger decodeTrigger(std::span<const std::byte> payload, size_t index)
{
    auto testBit = [&payload, index](const int bytes[], const int bits[]) {
        return (payload[bytes[index]] & (1_b << bits[index])) != 0_b;
    };

    Scene::Trigger trigger;

    const auto channelByte = payload[channelBytes[index]];
    trigger.enabled = (channelByte & (1_b << enabledBit)) != 0_b;
    trigger.midiChannel =
        trigger.enabled ? static_cast<Value7bit>(channelByte & 0x0F_b) + 1 : 0;

    trigger.type = (payload[swTypeBytes[index]] & (1_b << swTypeBit)) == 0_b
                       ? Scene::SwitchType::Momentary
                       : Scene::SwitchType::Toggle;

    trigger.port =
        testBit(portBytes, portBits) ? Scene::Port::B : Scene::Port::A;
    trigger.hasFlamRoll = testBit(hasFlamRollBytes, hasFlamRollBits);

    auto id = static_cast<Value7bit>(payload[idBytes[index]]);
    if (testBit(actionTypeBytes, actionTypeBits))
    {
        Scene::Control control;
        control.param = id;
        control.value = static_cast<Value7bit>(payload[valueBytes[index]]);
        control.releaseValue =
            static_cast<Value7bit>(payload[releaseValueBytes[index]]);
        trigger.action = control;
    }
    else
    {
        Scene::Note note;
        note.note = id;
        const auto velocity = payload[velocityBytes[index]];
        // The pedal has always fixed velocity
        if (testBit(velocityTypeBytes, velocityTypeBits) || index == 16)
            note.velocity = Value7bit(velocity);
        else
            note.velocity = Scene::Note::VelocityCurve(velocity);
        trigger.action = note;
    }
    return trigger;
}

std::error_code encodeTrigger(const Scene::Trigger& trigger,
                              std::span<std::byte> payload, size_t index)
{
    auto setBit = [&payload, index](bool condition, const int bytes[],
                                    const int bits[]) {
        if (condition)
            payload[bytes[index]] |= 1_b << bits[index];
    };

    auto& channelByte = payload[channelBytes[index]];
    if (trigger.enabled)
    {
        channelByte |= 1_b << enabledBit;
        if (trigger.midiChannel > 16)
            return std::make_error_code(std::errc::invalid_argument);
        channelByte |= static_cast<std::byte>(trigger.midiChannel - 1);
    }
    if (trigger.type == Scene::SwitchType::Toggle)
        payload[swTypeBytes[index]] |= 1_b << swTypeBit;

    setBit(trigger.port == Scene::Port::B, portBytes, portBits);
    setBit(trigger.hasFlamRoll, hasFlamRollBytes, hasFlamRollBits);

    return std::visit(
        overloaded{
            [&](const Scene::Note& note) {
                payload[idBytes[index]] = static_cast<std::byte>(note.note);

                if (index == 16 &&
                    !std::holds_alternative<Value7bit>(note.velocity))
                    return std::make_error_code(std::errc::invalid_argument);

                payload[velocityBytes[index]] = std::visit(
                    overloaded{[&](Scene::Note::VelocityCurve curve) {
                                   return static_cast<std::byte>(curve);
                               },
                               [&](Value7bit velocity) {
                                   setBit(index != 16, velocityTypeBytes,
                                          velocityTypeBits);
                                   return static_cast<std::byte>(velocity);
                               }},
                    note.velocity);

                // Thas seems to be the default initialization in the controller
                payload[valueBytes[index]] = 0x7F_b;
                return std::error_code{};
            },
            [&](const Scene::Control& control) {
                setBit(true, actionTypeBytes, actionTypeBits);

                payload[idBytes[index]] = static_cast<std::byte>(control.param);
                payload[valueBytes[index]] =
                    static_cast<std::byte>(control.value);
                payload[releaseValueBytes[index]] =
                    static_cast<std::byte>(control.releaseValue);
                return std::error_code{};
            }},
        trigger.action);
}

// indices 0-1 for knobs, 2-3 for axes
constexpr int polarityBytes[] = {101, 106, 110, 116};
constexpr int polarityBits[] = {1, 1, 1, 1};

constexpr int knobTypeBytes[] = {101, 106, 110, 116};
constexpr auto knobTypeMask = 0xF0_b;
constexpr int knobTypeShift = 4;

constexpr int paramBytes[] = {102, 107, 111, 117};

constexpr int padAssignmentBytes[][4] = {
    {103, 108, 113, 118}, {103, 108, 113, 118}, {103, 108, 113, 118},
    {103, 108, 113, 118}, {103, 108, 113, 118}, {103, 108, 113, 118},
    {103, 108, 113, 118}, {96, 104, 112, 112},  {105, 109, 114, 119},
    {105, 109, 114, 119}, {105, 109, 114, 119}, {105, 109, 114, 119},
    {105, 109, 114, 119}, {105, 109, 114, 119}, {105, 109, 114, 119},
    {104, 104, 112, 112}};
constexpr int padAssignmentBits[][4] = {
    {0, 0, 0, 0}, {1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3},
    {4, 4, 4, 4}, {5, 5, 5, 5}, {6, 6, 6, 6}, {6, 3, 0, 5},
    {0, 0, 0, 0}, {1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3},
    {4, 4, 4, 4}, {5, 5, 5, 5}, {6, 6, 6, 6}, {0, 4, 1, 6}};

constexpr int pedalAssignmentBytes[] = {101, 106, 110, 116};
constexpr int pedalAssignmentBits[] = {0, 0, 0, 0};

Scene::Knob decodeKnob(std::span<const std::byte> payload, size_t index)
{
    auto testBit = [&payload, index](const int bytes[], const int bits[]) {
        return (payload[bytes[index]] & (1_b << bits[index])) != 0_b;
    };

    Scene::Knob knob;

    const auto type =
        (payload[knobTypeBytes[index]] & knobTypeMask) >> knobTypeShift;
    knob.enabled = type != 0_b;
    if (knob.enabled)
        knob.type = Scene::KnobType(type);

    knob.reversePolarity = testBit(polarityBytes, polarityBits);
    knob.param = Value7bit(payload[paramBytes[index]]);

    knob.padAssignmentBits = 0;
    for (int i = 0; i != 16; ++i)
    {
        knob.padAssignmentBits |=
            int(testBit(padAssignmentBytes[i], padAssignmentBits[i])) << i;
    }
    knob.pedalAssigned = testBit(pedalAssignmentBytes, pedalAssignmentBits);

    return knob;
}

void encodeKnob(const Scene::Knob& knob, std::span<std::byte> payload,
                size_t index)
{
    auto setBit = [&payload, index](bool condition, const int bytes[],
                                    const int bits[]) {
        if (condition)
            payload[bytes[index]] |= 1_b << bits[index];
    };

    if (knob.enabled)
    {
        payload[knobTypeBytes[index]] |= static_cast<std::byte>(knob.type)
                                         << knobTypeShift;
    }

    setBit(knob.reversePolarity, polarityBytes, polarityBits);

    payload[paramBytes[index]] = static_cast<std::byte>(knob.param);

    for (int i = 0; i != 16; ++i)
    {
        setBit(knob.padAssignmentBits & (1 << i), padAssignmentBytes[i],
               padAssignmentBits[i]);
    }
    setBit(knob.pedalAssigned, pedalAssignmentBytes, pedalAssignmentBits);
}

constexpr int axisReleaseValueSignBytes[] = {112, 120};
constexpr int axisReleaseValueSignBits[] = {2, 0};

constexpr int axisReleaseValueBytes[] = {115, 121};

Scene::Axis decodeAxis(std::span<const std::byte> payload, size_t index)
{
    Scene::Axis axis;
    static_cast<Scene::Knob&>(axis) = decodeKnob(payload, index + 2);

    auto testBit = [&payload, index](const int bytes[], const int bits[]) {
        return (payload[bytes[index]] & (1_b << bits[index])) != 0_b;
    };

    const auto value = Value7bit(payload[axisReleaseValueBytes[index]]);
    if (axis.type == Scene::KnobType::PitchBend)
    {
        axis.releaseValue =
            testBit(axisReleaseValueSignBytes, axisReleaseValueSignBits)
                ? value
                : -128 + value;
    }
    else
    {
        axis.releaseValue =
            (value >> 1) +
            (testBit(axisReleaseValueSignBytes, axisReleaseValueSignBits) ? 0x40
                                                                          : 0);
    }

    return axis;
}

std::error_code encodeAxis(const Scene::Axis& axis,
                           std::span<std::byte> payload, size_t index)
{
    auto setBit = [&payload, index](bool condition, const int bytes[],
                                    const int bits[]) {
        if (condition)
            payload[bytes[index]] |= 1_b << bits[index];
    };

    encodeKnob(axis, payload, index + 2);

    auto& valueByte = payload[axisReleaseValueBytes[index]];
    const auto value = axis.releaseValue;
    if (axis.type == Scene::KnobType::PitchBend)
    {
        valueByte = static_cast<std::byte>(value < 0 ? value + 128 : value);
        setBit(value >= 0, axisReleaseValueSignBytes, axisReleaseValueSignBits);
    }
    else
    {
        valueByte = static_cast<std::byte>(value << 1);
        setBit(value >= 64, axisReleaseValueSignBytes,
               axisReleaseValueSignBits);
    }

    return std::error_code{};
}

constexpr int minSpeedBytes[] = {124, 122};
constexpr int minSpeedLastbitBytes[] = {120, 120};
constexpr int minSpeedLastbitBits[] = {3, 1};

constexpr int maxSpeedBytes[] = {125, 123};
constexpr int maxSpeedLastbitBytes[] = {120, 120};
constexpr int maxSpeedLastbitBits[] = {4, 2};

constexpr int minVolumeBytes[] = {126, 129};
constexpr int maxVolumeBytes[] = {127, 130};

Scene::Repeater decodeRepeater(std::span<const std::byte> payload, size_t index)
{
    Scene::Repeater repeater;

    auto lastBit = [&payload, index](const int bytes[],
                                     const int bits[]) -> unsigned char {
        return (payload[bytes[index]] & (1_b << bits[index])) != 0_b ? 0x80
                                                                     : 0x00;
    };

    repeater.minSpeed = (unsigned char)(payload[minSpeedBytes[index]]) +
                        lastBit(minSpeedLastbitBytes, minSpeedLastbitBits);
    repeater.maxSpeed = (unsigned char)(payload[maxSpeedBytes[index]]) +
                        lastBit(maxSpeedLastbitBytes, maxSpeedLastbitBits);
    repeater.minVolume = (unsigned char)payload[minVolumeBytes[index]];
    repeater.maxVolume = (unsigned char)payload[maxVolumeBytes[index]];

    return repeater;
}

std::error_code encodeRepeater(const Scene::Repeater& repeater,
                               std::span<std::byte> payload, size_t index)
{
    auto setLastBit = [&payload, index](unsigned char value, const int bytes[],
                                        const int bits[]) {
        if (value > 127)
            payload[bytes[index]] |= 1_b << bits[index];
    };

    if (index == 1 && (repeater.minSpeed < 40 || repeater.maxSpeed > 240))
        return std::make_error_code(std::errc::invalid_argument);

    payload[minSpeedBytes[index]] =
        static_cast<std::byte>(repeater.minSpeed & 0x7F);
    setLastBit(repeater.minSpeed, minSpeedLastbitBytes, minSpeedLastbitBits);
    payload[maxSpeedBytes[index]] =
        static_cast<std::byte>(repeater.maxSpeed & 0x7F);
    setLastBit(repeater.maxSpeed, maxSpeedLastbitBytes, maxSpeedLastbitBits);

    if (repeater.minVolume < 0 || repeater.maxVolume < 0)
        return std::make_error_code(std::errc::invalid_argument);

    payload[minVolumeBytes[index]] = static_cast<std::byte>(repeater.minVolume);
    payload[maxVolumeBytes[index]] = static_cast<std::byte>(repeater.maxVolume);

    return std::error_code{};
}

constexpr int fixedVelocityByte = 92;

} // namespace

Expected<std::array<std::byte, 138>> encodeScene(const Scene& scene)
{
    std::array<std::byte, 138> payload;
    std::fill(payload.begin(), payload.end(), 0_b);

    for (size_t i = 0; i != 16; ++i)
    {
        if (auto error = encodeTrigger(scene.pads[i], payload, i))
            return tl::make_unexpected(error);
    }
    if (auto error = encodeTrigger(scene.pedal, payload, 16))
        return tl::make_unexpected(error);

    encodeKnob(scene.knobs[0], payload, 0);
    encodeKnob(scene.knobs[1], payload, 1);

    if (auto error = encodeAxis(scene.x, payload, 0))
        return tl::make_unexpected(error);
    if (auto error = encodeAxis(scene.y, payload, 1))
        return tl::make_unexpected(error);
    if (auto error = encodeRepeater(scene.flam, payload, 0))
        return tl::make_unexpected(error);
    if (auto error = encodeRepeater(scene.roll, payload, 1))
        return tl::make_unexpected(error);
    payload[fixedVelocityByte] = static_cast<std::byte>(scene.fixedVelocity);

    return payload;
}

Expected<Scene> decodeScene(std::span<const std::byte> payload)
{
    Scene scene;

    for (size_t i = 0; i != 16; ++i)
    {
        scene.pads[i] = decodeTrigger(payload, i);
    }
    scene.pedal = decodeTrigger(payload, 16);
    scene.knobs[0] = decodeKnob(payload, 0);
    scene.knobs[1] = decodeKnob(payload, 1);
    scene.x = decodeAxis(payload, 0);
    scene.y = decodeAxis(payload, 1);
    scene.flam = decodeRepeater(payload, 0);
    scene.roll = decodeRepeater(payload, 1);
    scene.fixedVelocity = Value7bit(payload[fixedVelocityByte]);

    return scene;
}

bool operator==(const Scene::Note& lhs, const Scene::Note& rhs)
{
    return lhs.note == rhs.note && lhs.velocity == rhs.velocity;
}

bool operator==(const Scene::Control& lhs, const Scene::Control& rhs)
{
    return lhs.param == rhs.param && lhs.value == rhs.value &&
           lhs.releaseValue == rhs.releaseValue;
}

bool operator==(const Scene::Trigger& lhs, const Scene::Trigger& rhs)
{
    return lhs.enabled == rhs.enabled && lhs.midiChannel == rhs.midiChannel &&
           lhs.type == rhs.type && lhs.action == rhs.action &&
           lhs.hasFlamRoll == rhs.hasFlamRoll && lhs.port == rhs.port;
}

bool operator==(const Scene::Knob& lhs, const Scene::Knob& rhs)
{
    return lhs.enabled == rhs.enabled && lhs.type == rhs.type &&
           lhs.param == rhs.param &&
           lhs.reversePolarity == rhs.reversePolarity &&
           lhs.padAssignmentBits == rhs.padAssignmentBits &&
           lhs.pedalAssigned == rhs.pedalAssigned;
}

bool operator==(const Scene::Axis& lhs, const Scene::Axis& rhs)
{
    return static_cast<const Scene::Knob&>(lhs) ==
               static_cast<const Scene::Knob&>(rhs) &&
           lhs.releaseValue == rhs.releaseValue;
}

bool operator==(const Scene::Repeater& lhs, const Scene::Repeater& rhs)
{
    return lhs.minSpeed == rhs.minSpeed && lhs.maxSpeed == rhs.maxSpeed &&
           lhs.minVolume == rhs.minVolume && lhs.maxVolume == rhs.maxVolume;
}

bool operator==(const Scene& lhs, const Scene& rhs)
{
    for (size_t i = 0; i != 16; ++i)
    {
        lhs.pads[i] != rhs.pads[i];
        return false;
    }
    return lhs.knobs[0] == rhs.knobs[0] && lhs.knobs[1] == rhs.knobs[1] &&
           lhs.x == rhs.x && lhs.y == rhs.y && lhs.flam == rhs.flam &&
           lhs.roll == rhs.roll && lhs.fixedVelocity == rhs.fixedVelocity;
    return true;
}

} // namespace paddock::midi::korgPadKontrol
