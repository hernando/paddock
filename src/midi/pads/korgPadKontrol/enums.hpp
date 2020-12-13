#pragma once

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{
enum class LedName : char
{
    pad1 = 0,
    pad2 = 1,
    pad3 = 2,
    pad4 = 3,
    pad5 = 4,
    pad6 = 5,
    pad7 = 6,
    pad8 = 7,
    pad9 = 8,
    pad10 = 9,
    pad11 = 10,
    pad12 = 11,
    pad13 = 12,
    pad14 = 13,
    pad15 = 14,
    pad16 = 15,

    buttonScene = 16,
    buttonMessage = 17,
    buttonSetting = 18,
    buttonNote = 19,
    buttonMidiCh = 20,
    buttonSwType = 21,
    buttonRelVal = 22,
    buttonVelocity = 23,
    buttonPort = 24,
    buttonFixedVel = 25,
    buttonProgChange = 26,
    buttonX = 27,
    buttonY = 28,
    buttonKnob1 = 29,
    buttonKnob2 = 30,
    buttonPedal = 31,
    buttonRoll = 32,
    buttonFlam = 33,
    buttonHold = 34,

    led7_0_top = 48,
    led7_0_topRight = 49,
    led7_0_bottomRight = 50,
    led7_0_bottom = 51,
    led7_0_bottomLeft = 52,
    led7_0_topLeft = 53,
    led7_0_middle = 54,
    led7_0_dot = 55,
    led7_1_top = 56,
    led7_1_topRight = 57,
    led7_1_bottomRight = 58,
    led7_1_bottom = 59,
    led7_1_bottomLeft = 60,
    led7_1_topLeft = 61,
    led7_1_middle = 62,
    led7_1_dot = 63,
    led7_2_top = 64,
    led7_2_topRight = 65,
    led7_2_bottomRight = 66,
    led7_2_bottom = 67,
    led7_2_bottomLeft = 68,
    led7_2_topLeft = 69,
    led7_2_middle = 70,
    led7_2_dot = 71,
};

enum class LedStatus
{
    off = 0,
    on = 1,
    oneShot = 2,
    blink = 3,
};

enum class Switch : char
{
    scene = 0,
    message = 1,
    setting = 2,
    note = 3,
    midiCh = 4,
    swType = 5,
    relVal = 6,
    velocity = 7,
    port = 8,
    fixedVel = 9,
    progChange = 10,
    x = 11,
    y = 12,
    Knob1 = 13,
    Knob2 = 14,
    pedal = 15,
    roll = 16,
    flam = 17,
    hold = 18
};

enum class Knob : char
{
    knob1 = 0,
    knob2 = 1
};

enum class WheelTurn : char
{
    counterClockwise,
    clockwise
};

} // namespace korgPadKontrol
} // namespace midi
} // namespace paddock
