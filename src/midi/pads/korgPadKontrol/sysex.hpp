#pragma once

#include "enums.hpp"

#include "midi/sysex.hpp"

#include "utils/byte.hpp"

#include <array>

namespace paddock::midi::korgPadKontrol::sysex
{
constexpr size_t maxMessageSize = 128;

using midi::sysex::END;
using midi::sysex::START;
constexpr auto NON_REALTIME_MESSAGE = 0x7E_b;
constexpr auto GENERAL_INFORMATION = 0x06_b;

constexpr auto KORG = 0x42_b;
constexpr auto SW_PROJECT = 0x6E_b;
constexpr auto PADKONTROL = 0x08_b;

#define SYSEX_HEADER START, KORG, 0x40_b, SW_PROJECT, PADKONTROL

// clang-format off

// Values for 6th bytes
// 6th Byte = cd : 0dvmmmmm  d (0: Host->Controller, 1: Controller->Host)
//                           v (0: 2 Bytes Data Format, 1: Variable)
//                           mmmmm (Command Number)
// * = only in native mode

// Host commands
constexpr auto NATIVE_MODE_REQ = 0x00_b; // 000 00000
constexpr auto DISPLAY_LED     = 0x01_b; // 000 00001 *
constexpr auto DISPLAY_LCD     = 0x22_b; // 001 00010 *
constexpr auto PORT_DETECT_REQ = 0x1E_b; // 000 11110
constexpr auto DATA_DUMP_REQ   = 0x1F_b; // 000 11111
constexpr auto PACKET_COMM_REQ = 0x3F_b; // 001 11111

constexpr auto IDENTITY_REQ    = 0x01_b;

// Controller replies
constexpr auto NATIVE_MODE     = 0x40_b; // 010 00000
constexpr auto ENCODER_OUTPUT  = 0x43_b; // 010 00011 *
constexpr auto PAD_OUTPUT      = 0x45_b; // 010 00101 *
constexpr auto PEDAL_OUTPUT    = 0x47_b; // 010 00111 *
constexpr auto SW_OUTPUT       = 0x48_b; // 010 01000 *
constexpr auto KNOB_OUTPUT     = 0x49_b; // 010 01001 *
constexpr auto XY_OUTPUT       = 0x4B_b; // 010 01011 *
constexpr auto PORT_DETECT     = 0x7E_b; // 011 11110
constexpr auto PACKET_COMM     = 0x5F_b; // 010 11111
constexpr auto DATA_DUMP       = 0x7F_b; // 011 11111

constexpr auto IDENTITY        = 0x02_b;

// Function codes for requests and replies
constexpr auto SCENE_CHANGE_REQ       = 0x14_b;
constexpr auto SCENE_CHANGE           = 0x4F_b;
constexpr auto CURRENT_SCENE_DUMP_REQ = 0x10_b;
constexpr auto CURRENT_SCENE_DUMP     = 0x40_b;
constexpr auto GLOBAL_DATA_DUMP_REQ   = 0x0E_b;
constexpr auto GLOBAL_DATA_DUMP       = 0x51_b;
constexpr auto SCENE_WRITE_REQ        = 0x11_b;
constexpr auto DATA_FORMAT_ERROR      = 0x26_b;
constexpr auto DATA_LOAD_COMPLETED    = 0x23_b;
constexpr auto DATA_LOAD_ERROR        = 0x24_b;
constexpr auto WRITE_COMPLETED        = 0x21_b;
constexpr auto WRITE_ERROR            = 0x22_b;
// Bytes 00-03 are reserved for Native KORG mode Dump Data

#define PACKET_COMM_HOST_DATA_TYPE_1 PACKET_COMM_REQ, 0x2A_b, 0x00_b
#define PACKET_COMM_HOST_DATA_TYPE_2 PACKET_COMM_REQ, 0x0A_b, 0x01_b
#define PACKET_COMM_CONTROLLER_DATA_TYPE_1 PACKET_COMM, 0x00_b
#define PACKET_COMM_CONTROLLER_DATA_TYPE_2 PACKET_COMM, 0x01_b

constexpr auto nativeModeOffReq = std::to_array({
    SYSEX_HEADER, NATIVE_MODE_REQ, 0x00_b, 0x00_b, END});

constexpr auto nativeModeOffReply = std::to_array({
    SYSEX_HEADER, 0x40_b, 0x00_b, 0x02_b, END});

constexpr auto nativeModeOnReq = std::to_array({
    SYSEX_HEADER, NATIVE_MODE_REQ, 0x00_b, 0x01_b, END});

constexpr auto nativeModeOnReply = std::to_array({
    SYSEX_HEADER, 0x40_b, 0x00_b, 0x03_b, END});

constexpr auto nativeEnableOutput = std::to_array({
    SYSEX_HEADER, PACKET_COMM_HOST_DATA_TYPE_1,
    //0       1       2       3       4       5       6       7
    0x00_b, 0x05_b, 0x05_b, 0x05_b, 0x7F_b, 0x7E_b, 0x7F_b, 0x7F_b,
    //8       9       10      11      12      13      14      15
    0x03_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b,
    //16      17      18      19      20      21      22      23
    0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b,
    //24      25      26      27      28      29      30      31
    0x0A_b, 0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b, 0x07_b,
    //32      33      34      35      36      37      38      39
    0x08_b, 0x09_b, 0x0A_b, 0x0B_b, 0x0C_b, 0x0d_b, 0x0E_b, 0x0F_b,
    //40
    0x10_b, END});

constexpr auto nativeModeInit = std::to_array({
    SYSEX_HEADER, PACKET_COMM_HOST_DATA_TYPE_2,
    0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x29_b, 0x29_b, 0x29_b,
    END});

constexpr auto inquiryMessageRequest = std::to_array({
    START, NON_REALTIME_MESSAGE, 0x7F_b, 0x06_b, IDENTITY_REQ, END});

constexpr auto currentSceneDataDumpReq = std::to_array({
    SYSEX_HEADER, DATA_DUMP_REQ, CURRENT_SCENE_DUMP_REQ, 0x00_b, END});

constexpr auto globalDataDumpReq = std::to_array({
    SYSEX_HEADER, DATA_DUMP_REQ, GLOBAL_DATA_DUMP_REQ, 0x00_b, END});

constexpr auto dataFormatError =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, DATA_FORMAT_ERROR, 0x00_b, END});

constexpr auto dataLoadCompleted =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, DATA_LOAD_COMPLETED, 0x00_b, END});

constexpr auto dataLoadError =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, DATA_LOAD_ERROR, 0x00_b, END});

constexpr auto writeCompleted =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, WRITE_COMPLETED, 0x00_b, END});

constexpr auto writeError =  std::to_array({
    SYSEX_HEADER, PACKET_COMM, WRITE_ERROR, 0x00_b, END});

constexpr auto resetDefaultScene =  std::to_array({
    SYSEX_HEADER, 0x7F_b, 0x7F_b, 0x02_b, 0x0A_b, 0x02_b, CURRENT_SCENE_DUMP,
    //   0       1       2       3       4       5       6       7       9
    0x00_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x00_b,
    //   9      10      11      12      13      14      15      16      17
    0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x19_b, 0x00_b, 0x19_b,
    //  18      19      20      21      22      23      24      25      26
    0x19_b, 0x3D_b, 0x45_b, 0x41_b, 0x3F_b, 0x3C_b, 0x00_b, 0x3B_b, 0x39_b,
    //  27      28      29      30      31      32      33      34      35
    0x37_b, 0x31_b, 0x33_b, 0x44_b, 0x38_b, 0x00_b, 0x30_b, 0x34_b, 0x36_b,
    //  36      37      38      39      40      41      42      43      44
    0x3A_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    //  45      46      47      48      49      50      51      52      53
    0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    //  54      55      56      57      58      59      60      61      62
    0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    //  63      64      65      66      67      68      69      70      71
    0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    //  72      73      74      75      76      77      78      79      80
    0x00_b, 0x00_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x00_b,
    //  81      82      83      84      85      86      87      88      89
    0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x7F_b, 0x00_b, 0x7F_b,
    //  90      91      92      93      94      95      96      97      98
    0x7F_b, 0x7F_b, 0x7F_b, 0x19_b, 0x2F_b, 0x7F_b, 0x40_b, 0x00_b, 0x7F_b,
    //  99     100     101     102     103     104     105     106     107
    0x00_b, 0x20_b, 0x31_b, 0x14_b, 0x7F_b, 0x19_b, 0x7F_b, 0x31_b, 0x15_b,
    // 108     109     110     111     112     113     114     115     116
    0x7F_b, 0x7F_b, 0x11_b, 0x06_b, 0x67_b, 0x7F_b, 0x7F_b, 0x00_b, 0x31_b,
    // 117     118     119     120     121     122     123     124     125
    0x01_b, 0x7F_b, 0x7F_b, 0x14_b, 0x00_b, 0x32_b, 0x0C_b, 0x00_b, 0x02_b,
    // 126     127     128     129     130     131     132     133     134
    0x3C_b, 0x7F_b, 0x40_b, 0x14_b, 0x7F_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    // 135     136     137
    0x00_b, 0x00_b, 0x00_b, END});

// clang-format on

constexpr auto sceneWriteReq(std::byte sceneNumber)
{
    return std::to_array(
        {SYSEX_HEADER, DATA_DUMP_REQ, SCENE_WRITE_REQ, sceneNumber, END});
}

constexpr auto sceneChangeReq(std::byte sceneNumber)
{
    return std::to_array(
        {SYSEX_HEADER, DATA_DUMP_REQ, SCENE_CHANGE_REQ, sceneNumber, END});
}

constexpr auto displayLedCommand(LedName led, LedStatus status,
                                 unsigned char timerX9ms)
{
    if (timerX9ms > 32)
        timerX9ms = 31;
    return std::to_array({SYSEX_HEADER, DISPLAY_LED, std::byte(led),
                          (std::byte(status) << 5) | std::byte(timerX9ms),
                          END});
}

constexpr auto displayLcdCommand(const char text[3], bool blink = false)
{
    auto checked = [](unsigned char c) {
        if (c < 0x20 || c > 0x7F)
            return 0x20_b;
        return std::byte{c};
    };

    return std::to_array({SYSEX_HEADER, DISPLAY_LCD, 0x04_b,
                          blink ? 0x01_b : 0x00_b, checked(text[0]),
                          checked(text[1]), checked(text[2]), END});
}

constexpr auto packetCommunicationType2(std::span<const LedName> onLeds,
                                        const char text[3])
{
    auto message = std::to_array({SYSEX_HEADER, PACKET_COMM_HOST_DATA_TYPE_2,
                                  0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
                                  0x00_b, 0x00_b, 0x00_b, 0x00_b, END});

    for (auto led : onLeds)
    {
        switch (led)
        {
        case LedName::pad1:
        case LedName::pad2:
        case LedName::pad3:
        case LedName::pad4:
        case LedName::pad5:
        case LedName::pad6:
        case LedName::pad7:
            message[8] |= 0x01_b << int(led);
            break;
        case LedName::pad8:
        case LedName::pad9:
        case LedName::pad10:
        case LedName::pad11:
        case LedName::pad12:
        case LedName::pad13:
        case LedName::pad14:
            message[9] |= 0x01_b << (int(led) - int(LedName::pad8));
            break;
        case LedName::pad15:
        case LedName::pad16:
        case LedName::buttonScene:
        case LedName::buttonMessage:
        case LedName::buttonSetting:
        case LedName::buttonNote:
        case LedName::buttonMidiCh:
            message[10] |= 0x01_b << (int(led) - int(LedName::pad15));
            break;
        case LedName::buttonSwType:
        case LedName::buttonRelVal:
        case LedName::buttonVelocity:
        case LedName::buttonPort:
        case LedName::buttonFixedVel:
        case LedName::buttonProgChange:
        case LedName::buttonX:
            message[11] |= 0x01_b << (int(led) - int(LedName::buttonSwType));
            break;
        case LedName::buttonY:
        case LedName::buttonKnob1:
        case LedName::buttonKnob2:
        case LedName::buttonPedal:
        case LedName::buttonRoll:
        case LedName::buttonFlam:
        case LedName::buttonHold:
            message[12] |= 0x01_b << (int(led) - int(LedName::buttonSwType));
            break;
        case LedName::led7_0_dot:
            message[13] |= 0x01_b;
            break;
        case LedName::led7_1_dot:
            message[13] |= 0x02_b;
            break;
        case LedName::led7_2_dot:
            message[13] |= 0x04_b;
            break;
        default:; // The 7 segment leds labels are ignored.
        }
    }

    auto checked = [](unsigned char c) {
        if (c < 0x20 || c > 0x7F)
            return 0x20_b;
        return std::byte{c};
    };

    message[14] = checked(text[2]);
    message[15] = checked(text[1]);
    message[16] = checked(text[0]);

    return message;
}

} // namespace paddock::midi::korgPadKontrol::sysex
