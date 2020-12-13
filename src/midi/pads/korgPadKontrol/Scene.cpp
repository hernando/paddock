#include "Scene.hpp"

#include "scenePrinters.hpp"

#include "midi/types.hpp"

#include "utils/encodings.hpp"

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{
namespace
{
Scene::Trigger decodePad(std::span<const std::byte> payload, size_t index)
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
    constexpr int velocityTypeBits[] = {0, 1, 2, 0, 1, 2, 3, 4, //
                                        5, 6, 0, 1, 2, 3, 4, 5};

    constexpr int hasFlamRollBytes[] =          //
        {99,  99,  99,  99,  99,  99,  99,  96, //
         100, 100, 100, 100, 100, 100, 100, 96, 93};
    constexpr int hasFlamRollBits[] = {0, 1, 2, 3, 4, 5, 6, 2, //
                                       0, 1, 2, 3, 4, 5, 6, 3, 6};

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

Scene::Knob decodeKnob(std::span<const std::byte> payload, size_t index)
{
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
        {4, 4, 4, 4}, {5, 5, 5, 5}, {6, 6, 6, 6}, {6, 1, 0, 5},
        {0, 0, 0, 0}, {1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3},
        {4, 4, 4, 4}, {5, 5, 5, 5}, {6, 6, 6, 6}, {0, 4, 1, 6}};

    constexpr int pedalAssignmentBytes[] = {101, 106, 110, 116};
    constexpr int pedalAssignmentBits[] = {0, 0, 0, 0};

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

Scene::Axis decodeAxis(std::span<const std::byte> payload, size_t index)
{
    Scene::Axis axis;
    static_cast<Scene::Knob&>(axis) = decodeKnob(payload, index + 2);

    constexpr int releaseValueSignBytes[] = {112, 120};
    constexpr int releaseValueSignBits[] = {2, 0};

    constexpr int releaseValueBytes[] = {115, 121};

    auto testBit = [&payload, index](const int bytes[], const int bits[]) {
        return (payload[bytes[index]] & (1_b << bits[index])) != 0_b;
    };

    const auto value = Value7bit(payload[releaseValueBytes[index]]);
    if (axis.type == Scene::KnobType::PitchBend)
    {
        axis.releaseValue = testBit(releaseValueSignBytes, releaseValueSignBits)
                                ? value
                                : -128 + value;
    }
    else
    {
        axis.releaseValue =
            (value >> 1) +
            (testBit(releaseValueSignBytes, releaseValueSignBits) ? 0x40 : 0);
    }

    return axis;
}

Scene::Repeater decodeRepeater(std::span<const std::byte> payload, size_t index)
{
    constexpr int minSpeedBytes[] = {124, 122};
    constexpr int minSpeedLastbitBytes[] = {120, 120};
    constexpr int minSpeedLastbitBit[] = {3, 1};

    constexpr int maxSpeedBytes[] = {125, 123};
    constexpr int maxSpeedLastbitBytes[] = {120, 120};
    constexpr int maxSpeedLastbitBit[] = {4, 2};

    constexpr int minVolumeBytes[] = {126, 129};
    constexpr int maxVolumeBytes[] = {127, 130};

    Scene::Repeater repeater;

    auto lastBit = [&payload, index](const int bytes[],
                                     const int bits[]) -> unsigned char {
        return (payload[bytes[index]] & (1_b << bits[index])) != 0_b ? 0x80
                                                                     : 0x00;
    };

    repeater.minSpeed = (unsigned char)(payload[minSpeedBytes[index]]) +
                        lastBit(minSpeedLastbitBytes, minSpeedLastbitBit);
    repeater.maxSpeed = (unsigned char)(payload[maxSpeedBytes[index]]) +
                        lastBit(maxSpeedLastbitBytes, maxSpeedLastbitBit);
    repeater.minVolume = (unsigned char)payload[minVolumeBytes[index]];
    repeater.maxVolume = (unsigned char)payload[maxVolumeBytes[index]];

    return repeater;
}

} // namespace

Expected<std::vector<std::byte>> encodeScene(const Scene& scene)
{
    (void)scene;
    return {};
}

Expected<Scene> decodeScene(std::span<const std::byte> payload)
{
    std::cout << "Decoded" << std::endl;
    int i = 0;
    for (auto byte : payload)
    {
        printf("%.2X ", int(byte));
        if (++i == 10)
            printf(" ");
        if (i == 20)
        {
            printf("\n");
            i = 0;
        }
    }
    printf("\n");

    Scene scene;

    for (size_t i = 0; i != 16; ++i)
    {
        scene.pads[i] = decodePad(payload, i);
    }
    scene.pedal = decodePad(payload, 16);
    scene.knobs[0] = decodeKnob(payload, 0);
    scene.knobs[1] = decodeKnob(payload, 1);
    scene.x = decodeAxis(payload, 0);
    scene.y = decodeAxis(payload, 1);
    scene.flam = decodeRepeater(payload, 0);
    scene.roll = decodeRepeater(payload, 1);
    constexpr int fixedVelocityByte = 92;
    scene.fixedVelocity = Value7bit(payload[fixedVelocityByte]);

    return scene;
}

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
