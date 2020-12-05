#pragma once

#include "midi/sysex.hpp"

#include "utils/byte.hpp"

#include <array>

namespace paddock
{
namespace midi
{
namespace korgPadKontrol
{
namespace sysex
{
constexpr size_t maxMessageSize = 128;

using midi::sysex::START;
using midi::sysex::END;
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

constexpr auto nativeModeOff = std::to_array({
    SYSEX_HEADER, NATIVE_MODE_REQ, 0x00_b, 0x00_b, END});

constexpr auto nativeModeOn = std::to_array({
    SYSEX_HEADER, NATIVE_MODE_REQ, 0x00_b, 0x01_b, END});

constexpr auto nativeModeInit = std::to_array({
    SYSEX_HEADER,
    0x3F_b, 0x0A_b, 0x01_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,
    0x00_b, 0x00_b, 0x29_b, 0x29_b, 0x29_b, END});

constexpr auto nativeModeTest = std::to_array({
    SYSEX_HEADER,
    DISPLAY_LCD, 0x04_b, 0x00_b, 0x59_b, 0x45_b, 0x53_b, END});

constexpr auto nativeEnableOutput = std::to_array({
    SYSEX_HEADER,
    0x3F_b, 0x2A_b, 0x00_b, 0x00_b, 0x05_b, 0x05_b, 0x05_b,
    0x7F_b, 0x7E_b, 0x7F_b, 0x7F_b, 0x03_b, 0x0A_b, 0x0A_b,
    0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b,
    0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b, 0x0A_b,
    0x01_b, 0x02_b, 0x03_b, 0x04_b, 0x05_b, 0x06_b, 0x07_b,
    0x08_b, 0x09_b, 0x0A_b, 0x0B_b, 0x0C_b, 0x0d_b, 0x0E_b,
    0x0F_b, 0x10_b, END});

constexpr auto inquiryMessageRequest = std::to_array({
    START, 0x7E_b, 0x7F_b, 0x06_b, 0x01_b, END});

}
}
}
}
