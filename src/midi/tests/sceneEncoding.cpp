#include <gtest/gtest.h>

#include "midi/pads/korgPadKontrol/Scene.hpp"
#include "midi/pads/korgPadKontrol/scenePrinters.hpp"
#include "midi/pads/korgPadKontrol/sysex.hpp"

#include <iostream>

namespace paddock
{
namespace
{
using namespace midi::korgPadKontrol;

constexpr auto defaultScenePayload = std::to_array(
    {0x00_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x00_b,
     0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x00_b, 0x19_b,
     0x19_b, 0x3D_b, 0x45_b, 0x41_b, 0x3F_b, 0x3C_b, 0x00_b, 0x3B_b, 0x39_b,
     0x37_b, 0x31_b, 0x33_b, 0x44_b, 0x38_b, 0x00_b, 0x30_b, 0x34_b, 0x36_b,
     0x3A_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
     0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
     0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
     0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
     0x00_b, 0x00_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x00_b,
     0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x00_b, 0x7F_b,
     0x7F_b, 0x7F_b, 0x7F_b, 0x19_b, 0x2F_b, 0x7F_b, 0x40_b, 0x00_b, 0x7F_b,
     0x00_b, 0x20_b, 0x31_b, 0x14_b, 0x7F_b, 0x19_b, 0x7F_b, 0x31_b, 0x15_b,
     0x7F_b, 0x7F_b, 0x11_b, 0x06_b, 0x67_b, 0x7F_b, 0x7F_b, 0x00_b, 0x31_b,
     0x01_b, 0x7F_b, 0x7F_b, 0x14_b, 0x00_b, 0x32_b, 0x0C_b, 0x00_b, 0x02_b,
     0x3C_b, 0x7F_b, 0x00_b, 0x14_b, 0x7F_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
     0x00_b, 0x00_b, 0x00_b});
// Byte 128 is different from the default scene taken from the device. It's
// unknown what bit 7 represents there.

constexpr Scene makeTestScene()
{
    using Axis = Scene::Axis;
    using Control = Scene::Control;
    using KnobType = Scene::KnobType;
    using Note = Scene::Note;
    using Port = Scene::Port;
    using Repeater = Scene::Repeater;
    using SwitchType = Scene::SwitchType;
    using Trigger = Scene::Trigger;
    using VelocityCurve = Scene::Note::VelocityCurve;

    Scene scene;
    scene.pads[0] = Trigger{.enabled = false,
                            .midiChannel = 0,
                            .type = SwitchType::Momentary,
                            .action = Note{.note = 0,
                                           .velocity = VelocityCurve::curve1},
                            .hasFlamRoll = true,
                            .port = Port::A};
    scene.pads[1] = Trigger{.enabled = true,
                            .midiChannel = 1,
                            .type = SwitchType::Momentary,
                            .action = Note{.note = 10,
                                           .velocity = VelocityCurve::curve3},
                            .hasFlamRoll = false,
                            .port = Port::B};
    scene.pads[2] = Trigger{.enabled = true,
                            .midiChannel = 10,
                            .type = SwitchType::Toggle,
                            .action = Note{.note = 20,
                                           .velocity = VelocityCurve::curve5},
                            .hasFlamRoll = true,
                            .port = Port::A};
    scene.pads[3] = Trigger{.enabled = true,
                            .midiChannel = 6,
                            .type = SwitchType::Toggle,
                            .action = Note{.note = 30,
                                           .velocity = VelocityCurve::curve7},
                            .hasFlamRoll = false,
                            .port = Port::B};
    scene.pads[4] = Trigger{.enabled = true,
                            .midiChannel = 9,
                            .type = SwitchType::Momentary,
                            .action = Note{.note = 40,
                                           .velocity = midi::Value7bit{10}},
                            .hasFlamRoll = true,
                            .port = Port::A};
    scene.pads[5] = Trigger{.enabled = true,
                            .midiChannel = 3,
                            .type = SwitchType::Momentary,
                            .action = Note{.note = 50,
                                           .velocity = midi::Value7bit{20}},
                            .hasFlamRoll = false,
                            .port = Port::B};
    scene.pads[6] = Trigger{.enabled = true,
                            .midiChannel = 7,
                            .type = SwitchType::Toggle,
                            .action = Note{.note = 60,
                                           .velocity = midi::Value7bit{30}},
                            .hasFlamRoll = true,
                            .port = Port::A};
    scene.pads[7] = Trigger{.enabled = true,
                            .midiChannel = 5,
                            .type = SwitchType::Toggle,
                            .action = Note{.note = 70,
                                           .velocity = midi::Value7bit{40}},
                            .hasFlamRoll = false,
                            .port = Port::B};
    scene.pads[8] = Trigger{.enabled = true,
                            .midiChannel = 1,
                            .type = SwitchType::Momentary,
                            .action = Control{.param = 11,
                                              .value = 21,
                                              .releaseValue = 31},
                            .hasFlamRoll = true,
                            .port = Port::A};
    scene.pads[9] = Trigger{.enabled = true,
                            .midiChannel = 1,
                            .type = SwitchType::Momentary,
                            .action = Control{.param = 12,
                                              .value = 22,
                                              .releaseValue = 32},
                            .hasFlamRoll = false,
                            .port = Port::B};
    scene.pads[10] = Trigger{.enabled = true,
                             .midiChannel = 1,
                             .type = SwitchType::Toggle,
                             .action = Control{.param = 13,
                                               .value = 23,
                                               .releaseValue = 33},
                             .hasFlamRoll = true,
                             .port = Port::A};
    scene.pads[11] = Trigger{.enabled = true,
                             .midiChannel = 1,
                             .type = SwitchType::Toggle,
                             .action = Control{.param = 14,
                                               .value = 24,
                                               .releaseValue = 34},
                             .hasFlamRoll = false,
                             .port = Port::B};
    scene.pads[12] = Trigger{.enabled = true,
                             .midiChannel = 1,
                             .type = SwitchType::Momentary,
                             .action = Control{.param = 15,
                                               .value = 25,
                                               .releaseValue = 35},
                             .hasFlamRoll = true,
                             .port = Port::A};
    scene.pads[13] = Trigger{.enabled = true,
                             .midiChannel = 1,
                             .type = SwitchType::Momentary,
                             .action = Control{.param = 16,
                                               .value = 26,
                                               .releaseValue = 36},
                             .hasFlamRoll = false,
                             .port = Port::B};
    scene.pads[14] = Trigger{.enabled = true,
                             .midiChannel = 1,
                             .type = SwitchType::Toggle,
                             .action = Control{.param = 17,
                                               .value = 27,
                                               .releaseValue = 37},
                             .hasFlamRoll = true,
                             .port = Port::A};
    scene.pads[15] = Trigger{.enabled = true,
                             .midiChannel = 1,
                             .type = SwitchType::Toggle,
                             .action = Control{.param = 18,
                                               .value = 28,
                                               .releaseValue = 38},
                             .hasFlamRoll = false,
                             .port = Port::B};

    scene.pedal = Trigger{.enabled = true,
                          .midiChannel = 1,
                          .type = SwitchType::Toggle,
                          .action = Note{.note = 0,
                                         .velocity = midi::Value7bit{127}},
                          .hasFlamRoll = true,
                          .port = Port::A};
    scene.knobs[0] = Scene::Knob{.enabled = true,
                                 .type = KnobType::AfterTouch,
                                 .param = 0,
                                 .reversePolarity = false,
                                 .padAssignmentBits = 0xFFFF,
                                 .pedalAssigned = true};
    scene.knobs[1] = Scene::Knob{.enabled = true,
                                 .type = KnobType::AfterTouch,
                                 .param = 0,
                                 .reversePolarity = false,
                                 .padAssignmentBits = 0xFFFF,
                                 .pedalAssigned = true};

    scene.x = Axis{true, KnobType::AfterTouch, 0, false, 0xFFFF, true, 18},
    scene.y = Axis{true, KnobType::AfterTouch, 0, false, 0xFFFF, true, 42},
    scene.flam = Repeater{57, 89, 37, 111},
    scene.roll = Repeater{40, 78, 3, 56}, scene.fixedVelocity = 87;

    return scene;
}

constexpr auto testScene = makeTestScene();

} // namespace

TEST(korgPadKontrolEncoding, defaultScene)
{
    auto scene = decodeScene(defaultScenePayload);
    ASSERT_TRUE(scene);
    auto payload = encodeScene(*scene);
    ASSERT_TRUE(payload);
    ASSERT_EQ(defaultScenePayload, *payload);
}

TEST(korgPadKontrolEncoding, pads)
{
    auto payload = encodeScene(testScene);
    ASSERT_TRUE(payload);
    auto scene = decodeScene(*payload);
    ASSERT_TRUE(scene);
    for (int i = 0; i != 16; ++i)
        ASSERT_EQ(scene->pads[i], testScene.pads[i]) << "index " << i;
}

TEST(korgPadKontrolEncoding, pedal)
{
    auto payload = encodeScene(testScene);
    ASSERT_TRUE(payload);
    auto scene = decodeScene(*payload);
    ASSERT_TRUE(scene);
    ASSERT_EQ(scene->pedal, testScene.pedal);
}

TEST(korgPadKontrolEncoding, knobs)
{
    auto payload = encodeScene(testScene);
    ASSERT_TRUE(payload);
    auto scene = decodeScene(*payload);
    ASSERT_TRUE(scene);
    ASSERT_EQ(scene->knobs[0], testScene.knobs[0]);
    ASSERT_EQ(scene->knobs[1], testScene.knobs[1]);
}

TEST(korgPadKontrolEncoding, xy)
{
    auto payload = encodeScene(testScene);
    ASSERT_TRUE(payload);
    auto scene = decodeScene(*payload);
    ASSERT_TRUE(scene);
    ASSERT_EQ(scene->x, testScene.x);
    ASSERT_EQ(scene->y, testScene.y);
}

TEST(korgPadKontrolEncoding, repeaters)
{
    auto payload = encodeScene(testScene);
    ASSERT_TRUE(payload);
    auto scene = decodeScene(*payload);
    ASSERT_TRUE(scene);
    ASSERT_EQ(scene->flam, testScene.flam);
    ASSERT_EQ(scene->roll, testScene.roll);
}

} // namespace paddock
